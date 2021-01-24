#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asl_cli.h"

static bool asl_cli_cmd_is_same(const char *a, const char *b)
{
    return (asl_strlen(a) == asl_strlen(b) && !asl_strcmp(a, b));
}

struct asl_cli* asl_cli_create(const char* prompt)
{
    struct asl_cli* cli = ASL_MALLOC_T(struct asl_cli);
    const char* promptp = (prompt == NULL ? ASL_CLI_DEF_PROMPT : prompt);
    u_int32_t plen = asl_strtlen(promptp);
    cli->top = ASL_MALLOC_T(struct asl_cli_item);
    cli->top->top = cli->top->next = cli->top->upper = cli->top;
    cli->top->child = NULL;
    cli->top->cmd = (char*)asl_malloc(plen);
    asl_memcpy(cli->top->cmd, promptp, plen);
    
    cli->cur = cli->top;

    cli->listBuf = NULL;

    cli->listChanged = TRUE;
    cli->listAllBuf = NULL;

    return cli;
}
static struct asl_cli_item* asl_cli_get_item(struct asl_cli *cli, const char* cmd)
{
    //asl_print_dbg("Try to Get %s command item in cur %s", cmd, cli->cur->cmd);
    struct asl_cli_item *tmp = cli->cur->child;
    while(!asl_cli_cmd_is_same(tmp->cmd, cmd) && ((tmp = tmp->next) != NULL));
    //asl_print_dbg("Get %s command item", cmd);
    return tmp;
}

result_t asl_cli_register(struct asl_cli *cli, const char* cmd, ASL_CLI_FUNC func)
{
    if(cli == NULL || cmd == NULL)
        return FAILURE;
    if(cli->cur->child != NULL && (asl_cli_get_item(cli, cmd) != NULL))
    {
        asl_print_err("Already have the command:%s", cmd);
        return FAILURE;
    }
    struct asl_cli_item *aci = ASL_MALLOC_T(struct asl_cli_item);
    struct asl_cli_item *tmp = NULL;
    aci->top = cli->top;
    aci->upper = cli->cur;
    aci->next = NULL;
    aci->child = NULL;
    aci->cmd = (char*)asl_malloc(asl_strtlen(cmd));
    asl_memcpy(aci->cmd, cmd, asl_strtlen(cmd));
    aci->func = func;
    tmp = cli->cur->child;
    if(tmp == NULL)
    {
        cli->cur->child = aci;
    }
    else
    {
        while(tmp->next != NULL) tmp = tmp->next;
        tmp->next = aci;
    }
    cli->listChanged = TRUE;
    //asl_print_dbg("Register %s command", cmd);
    return SUCCESS;
}
result_t asl_cli_unregister(struct asl_cli *cli, const char* cmd)
{
    if(cli == NULL || cli->top->child == NULL || cmd == NULL)
        return FAILURE;

    struct asl_cli_item *oldCur = cli->cur;
    struct asl_cli_item *tmp = asl_cli_get_item(cli, cmd);
    struct asl_cli_item *ctmp;
    //asl_print_dbg("Unregister %s command", cmd);
    if(tmp != NULL)
    {
        if(tmp->child != NULL)
        {
            cli->cur = tmp;
            while((ctmp = tmp->child) != NULL)
            {
                asl_cli_unregister(cli, ctmp->cmd);
            }
            cli->cur = oldCur;
        }
        cli->cur->child = tmp->next;
        asl_free(tmp->cmd);
        asl_free(tmp);
        cli->listChanged = TRUE;
        return SUCCESS;
    }
    
    return FAILURE;
}

static ASL_CLI_FUNC asl_cli_get_func(struct asl_cli *cli, const char* cmd)
{
    if(cli == NULL || cli->top->child == NULL || cmd == NULL)
        return NULL;
    struct asl_cli_item *tmp = asl_cli_get_item(cli, cmd);
    //asl_print_dbg("Get %s command func", cmd);
    return (tmp == NULL ? NULL : tmp->func);
}
static s_int32_t asl_cli_get_list_len(struct asl_cli *cli)
{
    s_int32_t listLen = 0;
    if(cli != NULL)
    {
        listLen = 1;
        struct asl_cli_item *tmp = cli->cur->child;
        char banner[128] = {0};
        sprintf(banner, "%s Command List:\r\n", tmp->cmd);
        s_int32_t bannerLen = asl_strlen(banner);
        listLen += bannerLen;
        do
        {
            listLen += asl_strlen(tmp->cmd) + (tmp->child == 0 ? 7 : 8);
        }while((tmp = tmp->next) != NULL);
    }
    return listLen;
}
char* asl_cli_get_list(struct asl_cli *cli)
{
    char *lBuf = NULL;
    if(cli != NULL)
    {
        struct asl_cli_item *tmp = cli->cur->child;
        s_int32_t lLen = asl_cli_get_list_len(cli);
        cli->listBuf = (char*)asl_realloc(cli->listBuf, lLen);
        asl_memset(cli->listBuf, 0, lLen);
        char *lp = cli->listBuf;
        sprintf(lp, "%s Command List:\r\n", tmp->cmd);
        lp += asl_strlen(lp);
        do
        {
            /* plus '\t' adn '\r' and '\n' */
            /* if tmp has children, there will be a '*' appendix */
            if(tmp->child == NULL)
                sprintf(lp, "  |--%s\r\n", tmp->cmd);
            else
                sprintf(lp, "  |--%s*\r\n", tmp->cmd);
            lp += asl_strlen(tmp->cmd) + (tmp->child == 0 ? 7 : 8);
        }while((tmp = tmp->next) != NULL);
        lBuf = cli->listBuf;
    }
    return lBuf;
}
static s_int32_t asl_cli_get_list_all_len_recurse(struct asl_cli_item *aci, s_int32_t tabCount)
{
    s_int32_t tsize = 0;
    s_int32_t wsize = 0;
    while(aci != NULL)
    {
        wsize = asl_strlen(aci->cmd) + 5 + tabCount;
        if(aci->child != NULL)
        {
            wsize = asl_cli_get_list_all_len_recurse(aci->child, tabCount + 2);
            tsize += wsize;
        }
        tsize += wsize;
        
        aci = aci->next;
    }
    return tsize;
}
static s_int32_t asl_cli_get_list_all_len(struct asl_cli *cli)
{
    s_int32_t listLen = 0;
    if(cli != NULL)
    {
        char tmp[128] = {0};
        listLen = 1;
        sprintf(tmp, "%s Command List:\r\n", cli->top->cmd);
        s_int32_t bannerLen = asl_strlen(tmp);
        listLen += bannerLen;
        listLen += asl_cli_get_list_all_len_recurse(cli->top->child, 2);
    }
    return listLen;
}

static s_int32_t asl_cli_get_list_all_recurse(struct asl_cli_item *aci, char* buf, s_int32_t tabCount)
{
    s_int32_t tsize = 0;
    s_int32_t wsize = 0;
    //asl_print_dbg("aci:%s", aci->cmd);
    while(aci != NULL)
    {
        wsize = asl_strlen(aci->cmd) + 5 + tabCount;
        asl_memset(buf, ' ', tabCount);
        sprintf(buf + tabCount, "|--%s\r\n", aci->cmd);
        buf += wsize;
        tsize += wsize;
        if(aci->child != NULL)
        {
            //asl_print_dbg("aci:%s child:%s", aci->cmd, aci->child->cmd);
            wsize = asl_cli_get_list_all_recurse(aci->child, buf, tabCount + 2);
            buf += wsize;
            tsize += wsize;
        }
        
        aci = aci->next;
    }
    //asl_printf(buf - tsize);
    return tsize;
}

char* asl_cli_get_list_all(struct asl_cli *cli)
{
    char* lBuf = NULL;
    if(cli != NULL)
    {
        if(cli->listChanged == TRUE)
        {
            s_int32_t lLen = asl_cli_get_list_all_len(cli);
            cli->listAllBuf = (char*)asl_realloc(cli->listAllBuf, lLen);
            char* lp = cli->listAllBuf;
            asl_memset(lp, 0, lLen);
            sprintf(lp, "%s Command List:\r\n", cli->top->cmd);
            lp += asl_strlen(lp);
            asl_cli_get_list_all_recurse(cli->top->child, lp, 2);
        }
        lBuf = cli->listAllBuf;
        cli->listChanged = FALSE;
    }
    return lBuf;
}
result_t asl_cli_set_cur(struct asl_cli *cli, const char* cmd)
{
    if(cli->cur->child == NULL)
        return FAILURE;
    struct asl_cli_item *aci = asl_cli_get_item(cli, cmd);
    if(aci == NULL)
        return FAILURE;
    cli->cur = aci;
    //asl_print_dbg("Set cur cli to %s command", cmd);
    return SUCCESS;
}
result_t asl_cli_destroy(struct asl_cli **clip)
{
    if(clip != NULL && *clip != NULL)
    {
        struct asl_cli *cli = *clip;
        struct asl_cli_item *tmp;
        cli->cur = cli->top;
        while((tmp = cli->top->child) != NULL)
            asl_cli_unregister(cli, tmp->cmd);
        asl_free(cli->top->cmd);
        asl_free(cli->top);
        asl_free(cli->listBuf);
        asl_free(cli->listAllBuf);
        asl_free(cli);
        *clip = NULL;
    }

    return SUCCESS;
}

void* asl_cli_exec_cmd(struct asl_cli *cli, char* cmd, void* arg)
{
    ASL_CLI_FUNC func = asl_cli_get_func(cli, cmd);
    return (func == NULL ? NULL : func(arg));
}

bool asl_cli_cur_is_top(struct asl_cli *cli)
{
    if(cli == NULL)
        return FALSE;
    return (cli->top == cli->cur);
}
bool asl_cli_cmd_is_dir(struct asl_cli *cli, const char* cmd)
{
    struct asl_cli_item *tmp = asl_cli_get_item(cli, cmd);
    if(tmp != NULL && tmp->child != NULL)
    {
        return TRUE;
    }
    else
        return FALSE;
}
