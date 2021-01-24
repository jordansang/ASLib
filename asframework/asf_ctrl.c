#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asl_cli.h"
#include "asf_ctrl.h"

#define ASF_CTRL_C ('C' - '@')
static bool isxspace(u_int8_t ch)
{
    char using[] = {
        /* Spaces */
        '\n', '\t', ' ', '\r', '\0', 
        /* Control charactors */
        0x7f, ASF_CTRL_C, 
        /* Other availible charactors */
        '_', '?'};
    u_int32_t index;
    for(index = 0; index < ASL_COUNTOF(using); index++)
    {
        if(ch == using[index])
            return TRUE;
    }
    return FALSE;
}
bool asf_ctrl_is_avalilable_input(void *input, s_int32_t len)
{
    u_int8_t *p = (u_int8_t *)input;
    //asl_print_hex(input, len, "input");
    while(len--)
    {
        if(!isalnum(p[len]) && !isxspace(p[len]))
            return FALSE;
    }
    
    return TRUE;
}

static result_t asf_ctrl_add(struct asf_ctrl *ac, const char* path, const char* cmd, void *func)
{
    result_t ret = FAILURE;
    if(ac != NULL && path != NULL)
    {
        struct asl_cli *cli = ac->cli;
        asl_sem_wait(&ac->cliSem);
        asl_cli_to_top(cli);
        s_int32_t count = 0;
        char** param = asl_str_split(path, ' ', &count);
        char** p = param;
        while(count-- > 0)
        {
            //asl_print_dbg("count:%d p:%s", count, *p);
            if(asl_cli_set_cur(cli, *p) == FAILURE)
            {
                ret = asl_cli_register_dir(cli, *p);
                if(ret != SUCCESS)
                    break;
                asl_cli_set_cur(cli, *p);
            }
            
            asl_free(*p);
            p++;
        }
        if(cmd != NULL && func != NULL)
            ret = asl_cli_register(cli, cmd, (ASL_CLI_FUNC)func);
        asl_free(param);
        asl_cli_to_top(cli);
            
        asl_sem_post(&ac->cliSem);
    }

    return ret;
}

result_t asf_ctrl_add_dir(struct asf_ctrl *ac, const char* dir)
{
    return asf_ctrl_add(ac, dir, NULL, NULL);
}

result_t asf_ctrl_add_cmd(struct asf_ctrl *ac, const char* dir, const char *cmd, void *func)
{
    return asf_ctrl_add(ac, dir, cmd, func);
}
static result_t asf_ctrl_rem(struct asf_ctrl *ac, const char *dir, const char *cmd)
{
    result_t ret = FAILURE;
    if(ac != NULL || dir != NULL)
    {
        struct asl_cli *cli = ac->cli;
    
        asl_sem_wait(&ac->cliSem);
        
        asl_cli_to_top(cli);
        s_int32_t count = 0;
        char** param = asl_str_split(dir, ' ', &count);
        char** p = param;
        if(cmd == NULL)
        {
            cmd = param[count - 1];
            count--;
        }
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
    }
    
    return ret;
}
result_t asf_ctrl_rem_dir(struct asf_ctrl *ac, const char* dir)
{
    return asf_ctrl_rem(ac, dir, NULL);
}

result_t asf_ctrl_rem_cmd(struct asf_ctrl *ac, const char* dir, const char *cmd)
{
    return asf_ctrl_rem(ac, dir, cmd);
}

result_t asf_ctrl_rem_all(struct asf_ctrl *ac)
{
    return asf_ctrl_rem(ac, ac->cli->top->cmd, NULL);
}
static s_int32_t asf_ctrl_cmd_backspace(struct asf_ctrl *ac)
{
    u_int8_t backspace[4] = {0x08, ' ', 0x08, '\0'};
    if(ASD_ARRAY_COUNT(ac->in) != 0)
    {
        asd_array_truncate(ac->in, ASD_ARRAY_COUNT(ac->in) - 1);
        asd_str_puts(ac->out, backspace);
    }
    return 0;
}
static u_int32_t asf_ctrl_get_suitable_cmd(struct asf_ctrl *ac)
{
    u_int32_t cmdCount = 0;
    struct asd_str *tmp = asd_str_create();
    asd_strcpy(tmp, ac->cmd);
    struct asl_cli_item *aci = ac->cli->cur->child;
    asd_str_clear(ac->cmdList);
    if(ASD_STR_LEN(tmp) != 0)
        asd_str_puts(ac->cmdList, "Suitable Commands:");
    else
        asd_str_puts(ac->cmdList, "Commands List:");
    while(aci != NULL)
    {
        if(ASD_STR_LEN(tmp) == 0 || asl_strncmp(aci->cmd, ASD_STR_CONTENT(tmp), ASD_STR_LEN(tmp)) == 0)
        {
            if(aci->child == NULL)
                asd_str_appendf(ac->cmdList, "\r\n  |--%s", aci->cmd);
            else
                asd_str_appendf(ac->cmdList, "\r\n  |--%s*", aci->cmd);
            asd_str_puts(ac->cmd, aci->cmd);
            cmdCount++;
        }
        aci = aci->next;
    }
    if(cmdCount != 1)
    {
        asd_strcpy(ac->cmd, tmp);
    }
    
    asd_str_destroy(&tmp);
    return cmdCount;
}
static s_int32_t asf_ctrl_cmd_reset(struct asf_ctrl *ac)
{
    asl_cli_to_top(ac->cli);
    asd_strcpy(ac->prompt, ac->name);
    asd_str_putf(ac->out, "\r\n%s> ", ASD_STR_CONTENT(ac->prompt));
    asd_array_clear(ac->in);
    return 0;
}

static s_int32_t asf_ctrl_cmd_exec(struct asf_ctrl *ac)
{
    asl_sem_wait(&ac->cliSem);
    //asl_print_dbg("in:%s", (char*)ASD_ARRAY_DATA(ac->in));
    char space = ' ';
    u_int32_t cmdLen = asd_array_get_value_index(ac->in, &space);
    //asl_print_dbg("cmdLen:%d", cmdLen);
    if(cmdLen == -1)
        cmdLen = ASD_ARRAY_COUNT(ac->in);

    asd_str_clear(ac->cmd);
    asd_str_putns(ac->cmd, ASD_ARRAY_DATA(ac->in), cmdLen);
    //asl_print_dbg("ac->cmd:%s", ASD_STR_CONTENT(ac->cmd));
    u_int32_t cmdCount = asf_ctrl_get_suitable_cmd(ac);
    //asl_print_dbg("cmd:%s, cmdCount:%d", ASD_STR_CONTENT(ac->cmd), cmdCount);
    if(cmdCount == 1)
    {
        if(asl_cli_cmd_is_dir(ac->cli, ASD_STR_CONTENT(ac->cmd)))
        {
            asd_strcpy(ac->prompt, ac->cmd);
            asl_cli_set_cur(ac->cli, ASD_STR_CONTENT(ac->cmd));
            asd_str_putf(ac->out, "\r\n%s> ", ASD_STR_CONTENT(ac->prompt));
        }
        else
        {
            if(cmdLen != ASD_ARRAY_COUNT(ac->in))
            {
                ac->argv = asl_str_split(&((char*)ASD_ARRAY_DATA(ac->in))[cmdLen + 1], ' ', &ac->argc);
                //asl_print_dbg("inCount:%d arg1:%s", ac->inCount, *(ac->in));
            }
            asl_cli_exec_cmd(ac->cli, ASD_STR_CONTENT(ac->cmd), ac);
            while(ac->argc-- > 0)
                asl_free(ac->argv[ac->argc]);
            asl_free(ac->argv);
            ac->argv = NULL;
            asd_str_appendf(ac->out, "\r\n%s> ", ASD_STR_CONTENT(ac->prompt));
        }
        asd_array_clear(ac->in);
    }
    else if(cmdCount > 1)
    {
        asd_str_appendf(ac->out, "\r\n%s> ", ASD_STR_CONTENT(ac->prompt));
        if(ASD_ARRAY_COUNT(ac->in))
            asd_str_appends(ac->out, ASD_STR_CONTENT(ac->cmd));
    }
    else if(cmdLen != 0 && asl_strncmp("exit", ASD_STR_CONTENT(ac->cmd), cmdLen) == 0)
    {
        if(asl_cli_cur_is_top(ac->cli))
        {
            ac->intf->close(ac);
            asf_ctrl_cmd_reset(ac);
        }
        else
        {
            char* upperDir = ac->cli->cur->upper->cmd;
            //asl_print_dbg("Change to Dir:%s", upperDir);
            asd_str_puts(ac->prompt, upperDir);
            asd_str_putf(ac->out, "\r\n%s> ", ASD_STR_CONTENT(ac->prompt));
            ac->cli->cur = ac->cli->cur->upper;
        }
        asd_array_clear(ac->in);
    }
    else if(cmdLen != 0)
    {
        asd_str_putf(ac->out, "\r\nNo suitable commad\r\n%s> ", ASD_STR_CONTENT(ac->prompt));
        asd_array_clear(ac->in);
    }
    else
    {
        asd_str_putf(ac->out, "\r\n%s> ", ASD_STR_CONTENT(ac->prompt));
        asd_array_clear(ac->in);
    }
    asl_sem_post(&ac->cliSem);
    return 0;
}
static s_int32_t asf_ctrl_cmd_complete(struct asf_ctrl *ac)
{
    asl_sem_wait(&ac->cliSem);
    //asl_print_dbg("in:%s", (char*)ASD_ARRAY_DATA(ac->in));
    asd_str_clear(ac->cmd);
    asd_str_putns(ac->cmd, ASD_ARRAY_DATA(ac->in), ASD_ARRAY_COUNT(ac->in));
    u_int32_t cmdCount = asf_ctrl_get_suitable_cmd(ac);
    //asl_print_dbg("cmdCount:%d array count:%d", cmdCount, ASD_ARRAY_COUNT(ac->in));
    if(cmdCount == 1)
    {
        asd_str_putf(ac->out, "\r\n%s> ", ASD_STR_CONTENT(ac->prompt));
        asd_str_appendf(ac->out, "%s ", ASD_STR_CONTENT(ac->cmd));
        asd_array_clear(ac->in);
        asd_str_appendc(ac->cmd, ' ');
        asd_array_append(ac->in, ASD_STR_CONTENT(ac->cmd), ASD_STR_LEN(ac->cmd));
    }
    else if(cmdCount > 1)
    {
        asd_str_appendf(ac->out, "\r\n%s\r\n%s> ", 
            ASD_STR_CONTENT(ac->cmdList), ASD_STR_CONTENT(ac->prompt));
        if(ASD_ARRAY_COUNT(ac->in))
            asd_str_appends(ac->out, ASD_STR_CONTENT(ac->cmd));
    }
    else
    {
        asd_str_putf(ac->out, "\r\n%s> %s", 
            ASD_STR_CONTENT(ac->prompt), ASD_STR_CONTENT(ac->cmd));
    }
    asl_sem_post(&ac->cliSem);
    return 0;
}
static s_int32_t asf_ctrl_help(struct asf_ctrl *ac)
{
    asl_sem_wait(&ac->cliSem);
    //asl_print_dbg("in:%s", (char*)ASD_ARRAY_DATA(ac->in));
    char space = ' ';
    u_int32_t cmdLen = asd_array_get_value_index(ac->in, &space);
    asd_str_clear(ac->cmd);
    asd_str_putns(ac->cmd, ASD_ARRAY_DATA(ac->in), cmdLen);
    u_int32_t cmdCount = asf_ctrl_get_suitable_cmd(ac);
    if(cmdCount == 1)
    {
        ac->helpFlag = TRUE;
        asl_cli_exec_cmd(ac->cli, ASD_STR_CONTENT(ac->cmd), ac);
        ac->helpFlag = FALSE;
        asd_str_appendf(ac->out, "\r\n%s> %s",
            ASD_STR_CONTENT(ac->prompt), (char*)ASD_ARRAY_DATA(ac->in));
    }
    else if(cmdCount > 1)
    {
        asd_str_appendf(ac->out, "\r\n%s\r\n%s> ", 
            ASD_STR_CONTENT(ac->cmdList), ASD_STR_CONTENT(ac->prompt));
        if(ASD_ARRAY_COUNT(ac->in))
            asd_str_appends(ac->out, ASD_STR_CONTENT(ac->cmd));
    }
    else
    {
        asd_str_putf(ac->out, "\r\nNo suitable commad\r\n%s> %s",
            ASD_STR_CONTENT(ac->prompt), ASD_STR_CONTENT(ac->cmd));
    }
    asl_sem_post(&ac->cliSem);
    return 0;
}

static s_int32_t asf_ctrl_echo(struct asf_ctrl *ac)
{
    asd_array_append(ac->in, ASD_STREAM_DATA(ac->s), ASD_STREAM_DSIZE(ac->s));
    asd_str_putns(ac->out, ASD_STREAM_DATA(ac->s), ASD_STREAM_DSIZE(ac->s));
    return 0;
}
static s_int32_t asf_ctrl_deliver(struct asf_ctrl *ac)
{
    struct
    {
        u_int8_t ch;
        s_int32_t(*func)(struct asf_ctrl*);
    }asf_ctrl_ops[] = 
    {
        {'C' - '@',               asf_ctrl_cmd_reset},
        {0x7f,                    asf_ctrl_cmd_backspace},
        {'\t',                    asf_ctrl_cmd_complete},
        {'\n',                    asf_ctrl_cmd_exec},
        {'\r',                    asf_ctrl_cmd_exec},
        {'?',                     asf_ctrl_help}
    };
    u_int32_t index;
    char ch = *((char*)ASD_STREAM_DATA(ac->s));
    for(index = 0; index < ASL_COUNTOF(asf_ctrl_ops); index++)
    {
        if(ch == asf_ctrl_ops[index].ch)
            return (asf_ctrl_ops[index].func)(ac);
    }
    
    return asf_ctrl_echo(ac);
}
static void asf_ctrl_send_prompt(struct asf_ctrl *ac)
{
    asd_str_putf(ac->out, "\r\n%s> ", ASD_STR_CONTENT(ac->name));
    ac->intf->send(ac);
    asd_str_clear(ac->out);
}
static void asf_ctrl_loop(struct asf_ctrl *ac)
{
    while(ac->intf->accept(ac) == SUCCESS)
    {
        asf_ctrl_send_prompt(ac);
        while(ac->intf->recv(ac) == SUCCESS)
        {
            asf_ctrl_deliver(ac);
            ac->intf->send(ac);
            asd_str_clear(ac->out);
            asd_stream_clear(ac->s);
        }
        //asl_print_err("Disconnected from Client...");
        ac->intf->close(ac);
        asf_ctrl_cmd_reset(ac);
    }
}

extern struct asf_ctrl_intf telnetIntf;
extern struct asf_ctrl_intf shellIntf;

static struct asf_ctrl_intf* asf_ctrl_get_intf(u_int8_t type)
{
    s_int32_t index;
    struct asf_ctrl_intf *intfs[] = {&telnetIntf, &shellIntf};
    for(index = 0; index < ASL_COUNTOF(intfs); index++)
    {
        if(type == intfs[index]->type)
            return intfs[index];
    }
    return NULL;
}
static s_int32_t asf_ctrl_array_cfunc(void *a, void *b)
{
    u_int8_t *aval = (u_int8_t*)a;
    u_int8_t *bval = (u_int8_t*)b;
    if(*aval == *bval)
        return 0;
    else
        return *aval < *bval ? -1 : 1;
}

struct asf_ctrl* asf_ctrl_create(u_int8_t type, const char* name)
{
    struct asf_ctrl *ac = ASL_MALLOC_T(struct asf_ctrl);
    ac->name = asd_str_create();
    ac->prompt = asd_str_create();
    asd_str_puts(ac->name, name);
    asd_str_puts(ac->prompt, name);
    asl_sem_create(&ac->cliSem, "Control Cli Sem", 1);
    ac->cli = asl_cli_create(name);

    ac->intf = ASL_MALLOC_T(struct asf_ctrl_intf);
    asl_memcpy(ac->intf, asf_ctrl_get_intf(type), sizeof(struct asf_ctrl_intf));
    ac->s = asd_stream_create(ASF_CTRL_DEFAULT_INSIZE);
    ac->in = asd_array_create_size(sizeof(u_int8_t), 10);
    ac->in->cfunc = asf_ctrl_array_cfunc;
    ac->out = asd_str_create();
    ac->cmd = asd_str_create();
    ac->cmdList = asd_str_create();

    asl_sem_create(&ac->timeoutSem, "Control Timeout Sem", 1);
    ac->timeout = ASF_CTRL_INFINIT_TIMEOUT;

    return ac;
}

result_t asf_ctrl_start(struct asf_ctrl *ac)
{
    /* Incase not to use goto */
    while(ac != NULL && ac->intf != NULL)
    {
        if(ac->intf->init(ac) != SUCCESS)
        {
            asl_print_err("Initialize ASF Control Interface FAILED");
            break;
        }
        //asl_print_dbg("Interface Initialized");
        return asl_thread_create(&ac->loopThr, asf_ctrl_loop, ac);
    }
    return FAILURE;
}
result_t asf_ctrl_destroy(struct asf_ctrl **ac)
{
    return SUCCESS;
}
