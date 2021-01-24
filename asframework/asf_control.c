#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asl_cli.h"
#include "asl_ipv4.h"
#include "asf_telnet.h"
#include "asf_control.h"

#define asf_control_param_decode(arg, count) asl_str_split(arg, ' ', count)
static void asf_control_release_in(struct asf_ctrl *ac)
{
    while(ac->inCount-- > 0)
        asl_free(ac->in[ac->inCount]);
    asl_free(ac->in);
    ac->in = NULL;
}
static void asf_control_gen_help_in(struct asf_ctrl *ac)
{
    if(ac->inCount != 0)
        asf_control_release_in(ac);
    ac->in = (char**)asl_malloc(sizeof(char*));
    *(ac->in) = (char*)asl_malloc(1);
    **(ac->in) = '?';
}

static s_int32_t asf_control_get_avalible_aci(struct asl_cli *cli, char* cmd, struct asl_cli_item **acip)
{
    s_int32_t count = 0;
    struct asl_cli_item *tmp = cli->cur->child;
    while(tmp != NULL)
    {
        //asl_print_dbg("%s vs %s", tmp->cmd, cmd);
        if(asl_strncmp((char*)tmp->cmd, cmd, asl_strlen(cmd)) == 0)
        {
        	assert(acip);
            *acip = tmp;
            count++;
            /* If is exactly this command */
            if(asl_strncmp((char*)tmp->cmd, cmd, asl_strlen((*acip)->cmd)) == 0)
            {
                count = 1;
                break;
            }
        }
        tmp = tmp->next;
    }

    return count;
}

static void asf_rc_gen_out(struct asf_ctrl *ac)
{
	assert(ac);
    struct asf_telnet *at = ac->at;
    if(ASD_STR_TLEN(ac->out) > 0)
    {
        if(at->outSize > 0)
        {
            asl_free(*(at->out));
        }
        at->outSize = ASD_STR_TLEN(ac->out);
        *(at->out) = (u_int8_t*)asl_malloc(at->outSize);
        asl_memcpy(*(at->out), ASD_STR_CONTENT(ac->out), at->outSize);
        asd_str_clear(ac->out);
    }
}

#define asf_control_cmd_can_be_completed(aciCount) (aciCount == 1)

static void asf_rc_deal_new_client(struct asf_ctrl *ac)
{
    if(asf_telnet_is_new_client(ac->at))
    {
        ac->at->name = (char*)asl_realloc(ac->at->name, asl_strtlen(ac->name));
        asl_memcpy(ac->at->name, ac->name, asl_strtlen(ac->name));
        asl_free(*(ac->at->out));
        *(ac->at->out) = NULL;
        asl_cli_to_top(ac->cli);
    }
}

static u_int32_t asf_rc_cfunc(struct asf_telnet *at)
{
    struct asf_ctrl *ac = (struct asf_ctrl*)(at->cArg);
    struct asl_cli_item *aci = ac->cli->cur->child;
    asd_str_putns(ac->cmd, (char*)(at->in), (u_int8_t*)asl_strchr(at->in, '\t') - at->in);
    u_int32_t aciCount = 0;
    while(aci != NULL)
    {
        if(asl_strncmp(aci->cmd, ASD_STR_CONTENT(ac->cmd), ASD_STR_LEN(ac->cmd)) == 0)
        {
            asd_str_appendf(ac->out, "%s\r\n", aci->cmd);
            aciCount++;
        }
        aci = aci->next;
    }    
    if(asl_strncmp(ASF_CTRL_EXIT, ASD_STR_CONTENT(ac->cmd), ASD_STR_LEN(ac->cmd)) == 0)
    {
        asd_str_appendf(ac->out, "%s\r\n", ASF_CTRL_EXIT);
        aciCount++;
    }
    //asl_print_dbg("aciCount:%d", aciCount);
    if(asf_control_cmd_can_be_completed(aciCount))
    {
        /* exclude '\r' and '\n' */
        asl_memset(at->in, 0, ASD_STR_TLEN(ac->out) - 2);
        asl_memcpy(at->in, ASD_STR_CONTENT(ac->out), ASD_STR_TLEN(ac->out) - 3);
        asd_str_clear(ac->out);
        asd_str_putc(ac->out, ASF_TELNET_COMLEMENTED);
    }
    asf_rc_gen_out(ac);
    return (*(at->out) == NULL ? 0 : asl_strtlen(*(at->out)));
}



static u_int32_t asf_rc_opfunc(struct asf_telnet *at)
{
    struct asf_ctrl *ac = (struct asf_ctrl*)(at->hArg);
    if(asl_strlen(at->in) == 0)
        return 0;
    asl_sem_wait(&ac->cliSem);
    asf_rc_deal_new_client(ac);
    struct asl_cli_item *aci = NULL;
    s_int32_t aciCount = 0;
    
    u_int8_t* p = (u_int8_t*)asl_strchr(at->in, ' ');
    u_int32_t cmdLen = (p == NULL ? asl_strlen(at->in) : p - at->in);
    asd_str_putns(ac->cmd, (char*)(at->in) ,cmdLen);
    
    aciCount = asf_control_get_avalible_aci(ac->cli, ASD_STR_CONTENT(ac->cmd), &aci);
    
    //asl_print_dbg("aciCount:%d", aciCount);
    if(aciCount == 1)
    {
        if(asl_cli_is_dir(aci) == TRUE)
        {
            asl_print_dbg("Change to Dir:%s", aci->cmd);
            at->name = (char*)asl_realloc(at->name, asl_strtlen(aci->cmd));
            asl_memcpy(at->name, aci->cmd, asl_strtlen(aci->cmd));
            asl_cli_set_cur(ac->cli, (char*)(aci->cmd));
        }
        else
        {
            if(p != NULL)
            {
                ac->in = asf_control_param_decode((char*)(++p), &ac->inCount);
                //asl_print_dbg("inCount:%d arg1:%s", ac->inCount, *(ac->in));
            }
            asl_cli_exec_cmd(ac->cli, aci->cmd, ac);
            asf_rc_gen_out(ac);
            asf_control_release_in(ac);
        }
    }
    else if(aciCount == 0 && asl_strncmp(ASF_CTRL_EXIT, ASD_STR_CONTENT(ac->cmd), cmdLen) == 0)
    {
        if(asl_cli_cur_is_top(ac->cli))
        {
            asf_telnet_close_client(at);
        }
        else
        {
            char* upperDir = ac->cli->cur->upper->cmd;
            asl_print_dbg("Change to Dir:%s", upperDir);
            at->name = (char*)asl_realloc(at->name, asl_strtlen(upperDir));
            asl_memcpy(at->name, upperDir, asl_strtlen(upperDir));
            asl_free(*(at->out));
            *(at->out) = NULL;
            ac->cli->cur = ac->cli->cur->upper;
        }
    }
    else
    {
        char* error_str = "No suitable commad";
        *(at->out) = (u_int8_t*)asl_malloc(asl_strtlen(error_str));
        asl_strncpy((char*)*(at->out), error_str, asl_strlen(error_str));
    }
    
    asl_sem_post(&ac->cliSem);
    return (*(at->out) == NULL ? 0 : asl_strtlen(*(at->out)));
}
static u_int32_t asf_rc_hfunc(struct asf_telnet *at)
{
    struct asf_ctrl *ac = (struct asf_ctrl*)(at->hArg);
    asl_sem_wait(&ac->cliSem);
    asf_rc_deal_new_client(ac);
    struct asl_cli_item *aci = NULL;
    s_int32_t aciCount = 0;
    
    if(at->in[0] == '?')
    {
        /* Show List */
        asl_cli_get_list(ac->cli);
        *(at->out) = (u_int8_t*)asl_malloc(asl_strtlen(ac->cli->listBuf));
        asl_memcpy(*(at->out), asl_cli_get_list(ac->cli), asl_strlen(ac->cli->listBuf));
    }
    else
    {
        u_int8_t* p = (u_int8_t*)asl_strchr(at->in, ' ');
        if(p == NULL)
            p = (u_int8_t*)asl_strchr(at->in, '?');
        u_int32_t cmdLen = p - at->in;
        asd_str_putns(ac->cmd, (char*)(at->in) , cmdLen);
        aciCount = asf_control_get_avalible_aci(ac->cli, ASD_STR_CONTENT(ac->cmd), &aci);
        //asl_print_dbg("aciCount:%d", aciCount);
        if(aciCount == 1 && asl_cli_is_dir(aci) == FALSE)
        {
            asf_control_gen_help_in(ac);
            asl_cli_exec_cmd(ac->cli, aci->cmd, ac);
            asf_rc_gen_out(ac);
            asf_control_release_in(ac);
        }
        else
        {
            /* Show List */
            asl_cli_get_list(ac->cli);
            *(at->out) = (u_int8_t*)asl_malloc(asl_strtlen(ac->cli->listBuf));
            asl_memcpy(*(at->out), asl_cli_get_list(ac->cli), asl_strlen(ac->cli->listBuf));
        }
    }
    
    asl_sem_post(&ac->cliSem);
    return (*(at->out) == NULL ? 0 : asl_strtlen(*(at->out)));
}
static void asf_lc_gen_out(struct asf_ctrl *ac)
{
    struct asl_sh *as = ac->as;
    if(ASD_STR_TLEN(ac->out) > 0)
    {
        if(as->outSize > 0)
        {
            asl_free(*(as->out));
        }
        as->outSize = ASD_STR_TLEN(ac->out);
        *(as->out) = (u_int8_t*)asl_malloc(as->outSize);
        asl_memcpy(*(as->out), ASD_STR_CONTENT(ac->out), as->outSize);
        asd_str_clear(ac->out);
    }
}

static void asf_lc_deal_new_client(struct asf_ctrl *ac)
{
    if(asl_sh_is_new_client(ac->as))
    {
        ac->as->name = (char*)asl_realloc(ac->as->name, asl_strtlen(ac->name));
        asl_memcpy(ac->as->name, ac->name, asl_strtlen(ac->name));
        asl_free(*(ac->as->out));
        *(ac->as->out) = NULL;
        asl_cli_to_top(ac->cli);
    }
}

static u_int32_t asf_lc_cfunc(struct asl_sh *as)
{
    struct asf_ctrl *ac = (struct asf_ctrl*)(as->cArg);
    struct asl_cli_item *aci = ac->cli->cur->child;
    asd_str_putns(ac->cmd, (char*)(as->in), (u_int8_t*)asl_strchr(as->in, '\t') - as->in);
    u_int32_t aciCount = 0;
    while(aci != NULL)
    {
        if(asl_strncmp(aci->cmd, ASD_STR_CONTENT(ac->cmd), ASD_STR_LEN(ac->cmd)) == 0)
        {
            asd_str_appendf(ac->out, "%s\r\n", aci->cmd);
            aciCount++;
        }
        aci = aci->next;
    }    
    if(asl_strncmp(ASF_CTRL_EXIT, ASD_STR_CONTENT(ac->cmd), ASD_STR_LEN(ac->cmd)) == 0)
    {
        asd_str_appendf(ac->out, "%s\r\n", ASF_CTRL_EXIT);
        aciCount++;
    }
    //asl_print_dbg("aciCount:%d", aciCount);
    if(asf_control_cmd_can_be_completed(aciCount))
    {
        /* exclude '\r' and '\n' */
        asl_memset(as->in, 0, ASD_STR_TLEN(ac->out) - 2);
        asl_memcpy(as->in, ASD_STR_CONTENT(ac->out), ASD_STR_TLEN(ac->out) - 3);
        asd_str_clear(ac->out);
        asd_str_putc(ac->out, ASL_SH_COMLEMENTED);
    }
    asf_lc_gen_out(ac);
    return (*(as->out) == NULL ? 0 : asl_strtlen(*(as->out)));
}

static u_int32_t asf_lc_opfunc(struct asl_sh *as)
{
    struct asf_ctrl *ac = (struct asf_ctrl*)(as->hArg);
    if(asl_strlen(as->in) == 0)
        return 0;
    asl_sem_wait(&ac->cliSem);
    asf_lc_deal_new_client(ac);
    struct asl_cli_item *aci = NULL;
    s_int32_t aciCount = 0;
    
    u_int8_t* p = (u_int8_t*)asl_strchr(as->in, ' ');
    u_int32_t cmdLen = (p == NULL ? asl_strlen(as->in) : p - as->in);
    asd_str_putns(ac->cmd, (char*)(as->in) ,cmdLen);
    
    aciCount = asf_control_get_avalible_aci(ac->cli, ASD_STR_CONTENT(ac->cmd), &aci);
    
    asl_print_dbg("aciCount:%d", aciCount);
    if(aciCount == 1)
    {
        if(asl_cli_is_dir(aci) == TRUE)
        {
            asl_print_dbg("Change to Dir:%s", aci->cmd);
            as->name = (char*)asl_realloc(as->name, asl_strtlen(aci->cmd));
            asl_memcpy(as->name, aci->cmd, asl_strtlen(aci->cmd));
            asl_cli_set_cur(ac->cli, (char*)(aci->cmd));
        }
        else
        {
            if(p != NULL)
            {
                ac->in = asf_control_param_decode((char*)(++p), &ac->inCount);
                //asl_print_dbg("inCount:%d arg1:%s", ac->inCount, *(ac->in));
            }
            asl_cli_exec_cmd(ac->cli, aci->cmd, ac);
            asf_lc_gen_out(ac);
            asf_control_release_in(ac);
        }
    }
    else if(aciCount == 0 && asl_strncmp(ASF_CTRL_EXIT, ASD_STR_CONTENT(ac->cmd), cmdLen) == 0)
    {
        if(asl_cli_cur_is_top(ac->cli))
        {
            asl_sh_close_client(as);
        }
        else
        {
            char* upperDir = ac->cli->cur->upper->cmd;
            asl_print_dbg("Change to Dir:%s", upperDir);
            as->name = (char*)asl_realloc(as->name, asl_strtlen(upperDir));
            asl_memcpy(as->name, upperDir, asl_strtlen(upperDir));
            asl_free(*(as->out));
            *(as->out) = NULL;
            ac->cli->cur = ac->cli->cur->upper;
        }
    }
    else
    {
        char* error_str = "No suitable commad";
        *(as->out) = (u_int8_t*)asl_malloc(asl_strtlen(error_str));
        asl_strncpy((char*)*(as->out), error_str, asl_strlen(error_str));
    }
    
    asl_sem_post(&ac->cliSem);
    return (*(as->out) == NULL ? 0 : asl_strtlen(*(as->out)));
}
static u_int32_t asf_lc_hfunc(struct asl_sh *as)
{
    struct asf_ctrl *ac = (struct asf_ctrl*)(as->hArg);
    asl_sem_wait(&ac->cliSem);
    asf_lc_deal_new_client(ac);
    struct asl_cli_item *aci = NULL;
    s_int32_t aciCount = 0;
    
    if(as->in[0] == '?')
    {
        /* Show List */
        asl_cli_get_list(ac->cli);
        *(as->out) = (u_int8_t*)asl_malloc(asl_strtlen(ac->cli->listBuf));
        asl_memcpy(*(as->out), asl_cli_get_list(ac->cli), asl_strlen(ac->cli->listBuf));
    }
    else
    {
        u_int8_t* p = (u_int8_t*)asl_strchr(as->in, ' ');
        if(p == NULL)
            p = (u_int8_t*)asl_strchr(as->in, '?');
        u_int32_t cmdLen = p - as->in;
        asd_str_putns(ac->cmd, (char*)(as->in) , cmdLen);
        aciCount = asf_control_get_avalible_aci(ac->cli, ASD_STR_CONTENT(ac->cmd), &aci);
        asl_print_dbg("aciCount:%d", aciCount);
        if(aciCount == 1 && asl_cli_is_dir(aci) == FALSE)
        {
            asf_control_gen_help_in(ac);
            asl_cli_exec_cmd(ac->cli, aci->cmd, ac);
            asf_lc_gen_out(ac);
            asf_control_release_in(ac);
        }
        else
        {
            /* Show List */
            asl_cli_get_list(ac->cli);
            *(as->out) = (u_int8_t*)asl_malloc(asl_strtlen(ac->cli->listBuf));
            asl_memcpy(*(as->out), asl_cli_get_list(ac->cli), asl_strlen(ac->cli->listBuf));
        }
    }
    
    asl_sem_post(&ac->cliSem);
    return (*(as->out) == NULL ? 0 : asl_strtlen(*(as->out)));
}

static struct asf_ctrl* asf_control_create(const char* name, u_int8_t type)
{
    const char* ASF_CONTROL_NAME = "Default";
    struct asf_ctrl *ac = ASL_MALLOC_T(struct asf_ctrl);
    const char* nptr = (name == NULL ? ASF_CONTROL_NAME : name);
    ac->name = (char*)asl_malloc(asl_strtlen(nptr));
    asl_strncpy(ac->name, nptr, asl_strtlen(nptr));
    asl_sem_create(&ac->cliSem, ac->name, 1);
    ac->cli = asl_cli_create(ac->name);
    ac->type = type;

    ac->cmd = asd_str_create();

    ac->in = NULL;
    ac->inCount = 0;

    ac->out = asd_str_create();
    
    return ac;
}

result_t asf_control_add_cmd(struct asf_ctrl *ac, char* path, char* cmd, ASF_CTRL_CALLBACK func)
{
    if(ac == NULL || cmd == NULL || asl_strcmp(cmd, ASF_CTRL_EXIT) == 0)
        return FAILURE;
    result_t ret = FAILURE;
    struct asl_cli *cli = ac->cli;
    
    asl_sem_wait(&ac->cliSem);
    asl_cli_to_top(cli);
    s_int32_t count = 0;
    char** param = asf_control_param_decode(path, &count);
    char** p = param;
    while(count-- > 0)
    {
        //asl_print_dbg("count:%d p:%s", count, *p);
        if(asl_cli_set_cur(cli, *p) == FAILURE)
        {
            asl_cli_register_dir(cli, *p);
            asl_cli_set_cur(cli, *p);
        }
        
        asl_free(*p);
        p++;
    }

    ret = asl_cli_register(cli, cmd, (ASL_CLI_FUNC)func);
    asl_free(param);
    asl_cli_to_top(cli);
    asl_sem_post(&ac->cliSem);

    return ret;
}
result_t asf_control_rem_cmd(struct asf_ctrl *ac, char* path, char* cmd)
{
    if(ac == NULL || cmd == NULL)
        return FAILURE;
    result_t ret = FAILURE;
    struct asl_cli *cli = ac->cli;
    
    asl_sem_wait(&ac->cliSem);
    asl_cli_to_top(cli);
    s_int32_t count = 0;
    char** param = asf_control_param_decode(path, &count);
    char** p = param;
    while(count-- > 0)
    {
        asl_cli_set_cur(cli, *p);
        asl_free(*p);
        p++;
    }

    ret = asl_cli_unregister(cli, cmd);
    asl_free(param);
    asl_cli_to_top(cli);
    asl_sem_post(&ac->cliSem);

    return ret;
}


/* Local Control */
struct asf_ctrl * asf_lc_create(const char* name)
{
    struct asf_ctrl *ac = asf_control_create(name, ASF_CTRL_TYPE_LOCAL);
    
    return ac;
}

result_t asf_lc_start(struct asf_ctrl *ac)
{
    if(ac != NULL)
    {
        ac->as = asl_sh_create(asf_lc_opfunc, asf_lc_hfunc, asf_lc_cfunc, ac->name);
        ac->as->hArg = ac->as->opArg = ac->as->cArg = ac;
        return SUCCESS;
    }
    return FAILURE;
}
/* Remote Control */
struct asf_ctrl * asf_rc_create(const char* name)
{
    struct asf_ctrl *ac = asf_control_create(name, ASF_CTRL_TYPE_REMOTE);
    
    return ac;
}
result_t asf_rc_start(struct asf_ctrl *ac)
{
    if(ac != NULL)
    {
        struct asl_net_info info;
        info.ip = ASL_IPV4_INADDR_ANY;
        info.port = ASF_RC_PORT;
        ac->at = asf_telnet_create(&info, asf_rc_opfunc, asf_rc_hfunc, asf_rc_cfunc, ac->name);
        ac->at->hArg = ac->at->opArg = ac->at->cArg = ac;
        return SUCCESS;
    }
    return FAILURE;
}
static void asf_control_destroy(struct asf_ctrl **acp)
{
    struct asf_ctrl *ac = *acp;
    asl_cli_destroy(&ac->cli);
    asl_sem_destroy(&ac->cliSem);
    asl_free(ac->name);
    asd_str_destroy(&ac->cmd);
    asd_str_destroy(&ac->out);
    while(ac->inCount-- > 0)
        asl_free(ac->in[ac->inCount]);
    asl_free(ac->in);
    asl_free(ac);
    *acp = NULL;
}
void asf_lc_destroy(struct asf_ctrl **acp)
{
    if(acp != NULL && *acp != NULL)
    {
        asf_control_destroy(acp);
    }
}
void asf_rc_destroy(struct asf_ctrl **acp)
{
    if(acp != NULL && *acp != NULL)
    {
        struct asf_ctrl *ac = *acp;
        asf_telnet_destroy(&ac->at);
        asf_control_destroy(acp);
    }
}
