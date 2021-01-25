#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asl_time.h"
#include "asf_telnet.h"

#define asf_telnet_send_out(at) asl_send(at->csock, ASD_STR_CONTENT(at->out), ASD_STR_LEN(at->out));
#define asf_telnet_send_in(at) asl_send(at->csock, ASD_ARRAY_DATA(at->in), ASD_ARRAY_COUNT(at->in));

#define asf_telnet_newline(at) asl_send(at->csock, "\r\n", 3)
static s_int32_t asf_telnet_cmd_do(struct asl_socket *sock, char telopt)
{
    char cmd[4] = { ASF_TEL_IAC, ASF_TEL_DO, 0, ASF_TEL_EOF };
    cmd[2] = telopt;
    return asl_send(sock, cmd, 4);
}

static s_int32_t asf_telnet_cmd_dont(struct asl_socket *sock, char telopt)
{
    char cmd[4] = { ASF_TEL_IAC, ASF_TEL_DONT, 0, ASF_TEL_EOF };
    cmd[2] = telopt;
    return asl_send(sock, cmd, 4);
}

static s_int32_t asf_telnet_cmd_will(struct asl_socket *sock, char telopt)
{
    char cmd[4] = { ASF_TEL_IAC, ASF_TEL_WILL, 0, ASF_TEL_EOF };
    cmd[2] = telopt;
    return asl_send(sock, cmd, 4);
}

static s_int32_t asf_telnet_cmd_wont(struct asl_socket *sock, char telopt)
{
    char cmd[4] = { ASF_TEL_IAC, ASF_TEL_WONT, 0, ASF_TEL_EOF };
    cmd[2] = telopt;
    return asl_send(sock, cmd, 4);
}

#define asf_telnet_do_echo(sock)    asf_telnet_cmd_do(sock, ASF_TELOPT_ECHO)
#define asf_telnet_do_sga(sock)     asf_telnet_cmd_do(sock, ASF_TELOPT_SGA)
#define asf_telnet_do_naws(sock)    asf_telnet_cmd_do(sock, ASF_TELOPT_NAWS)
#define asf_telnet_will_echo(sock)  asf_telnet_cmd_will(sock, ASF_TELOPT_ECHO)
#define asf_telnet_will_sga(sock)   asf_telnet_cmd_will(sock, ASF_TELOPT_SGA)
#define asf_telnet_wont_naws(sock)  asf_telnet_cmd_wont(sock, ASF_TELOPT_NAWS)
#define asf_telnet_dont_lmode(sock) asf_telnet_cmd_dont(sock, ASF_TELOPT_LINEMODE)

static void asf_telnet_prenegotiate(struct asl_socket *sock)
{
    asf_telnet_do_echo(sock);
    asf_telnet_do_sga(sock);
    asf_telnet_do_naws(sock);
    asf_telnet_will_echo(sock);
    asf_telnet_will_sga(sock);
    asf_telnet_wont_naws(sock);
    asf_telnet_dont_lmode(sock);
}

static void asf_telnet_cmddump(u_int8_t *buf, s_int32_t size)
{
    struct
    {
        u_int8_t ch;
        const char *str;
    }asf_telnet_iac_str[] = 
    {
        {ASF_TEL_IAC,     "IAC "},
        {ASF_TEL_WILL,    "WILL "},
        {ASF_TEL_WONT,    "WONT "},
        {ASF_TEL_DO,      "DO "},
        {ASF_TEL_DONT,    "DONT "},
        {ASF_TEL_SB,      "SB "},
        {ASF_TEL_SE,      "SE "},
        {ASF_TELOPT_SGA,  "TELOPT_SGA \n"},
        {ASF_TELOPT_ECHO, "TELOPT_ECHO \n"},
        {ASF_TELOPT_NAWS, "TELOPT_NAWS \n"}
    };
    int i;
    u_int32_t index;
    for (i = 0; i < size; i++)
    {
        for(index = 0; index < ASL_COUNTOF(asf_telnet_iac_str); index++)
        {
            if(buf[i] == asf_telnet_iac_str[index].ch)
            {
                asl_printf("%s", asf_telnet_iac_str[index].str);
                break;
            }
        }
        if(index == ASL_COUNTOF(asf_telnet_iac_str))
        {
            asl_printf("%02x ", buf[i]);
        }
    }
    asl_printf("\n");
}

static void asl_telnet_loop_release(void *arg)
{
    struct asf_telnet *at = (struct asf_telnet*)arg;
    asl_thread_cancel(&at->timeoutThr);
    asl_thread_join(&at->timeoutThr);
    asl_net_destroy_sock(&at->csock);
    asd_array_clear(at->in);
    asd_str_clear(at->out);
    asl_free(*(at->out));
    at->opFunc(at);
}
#define asl_telnet_send_str(at, str) asl_send(at->csock, str, asl_strtlen(str))

static void asl_telnet_send_prompt(struct asf_telnet *at)
{
    u_int8_t prompt[128] = {0};
    sprintf((char*)prompt, "%s> ", at->name);
    asl_telnet_send_str(at, prompt);
}

static void asl_telnet_ops_iac(struct asf_telnet *at, u_int8_t *buf, s_int32_t size)
{
    int i;
    asf_telnet_cmddump(buf, size);
    for(i = 0; i < size - 8; i++)
    {
        if(buf[i] == ASF_TEL_IAC 
            && buf[i + 1] == ASF_TEL_SB 
            && buf[i + 2] == ASF_TELOPT_NAWS)
        {
            at->width = (u_int16_t)(buf[i + 4]);
            at->height = (u_int16_t)(buf[i + 6]);
            //asl_print_dbg("w:0x%04x(%d) h:0x%04x(%d)", at->width, at->width, at->height, at->height);
            break;
        }
    }
}

static void asl_telnet_ops_backspace(struct asf_telnet *at, u_int8_t* buf, s_int32_t len)
{
    u_int8_t backspace[4] = { 0x08, ' ', 0x08, '\0'};
    if(ASD_ARRAY_COUNT(at->in) != 0)
    {
        asd_array_truncate(at->in, ASD_ARRAY_COUNT(at->in) - 1);
        asl_send(at->csock, backspace, 4);
    }
}
        
static void asl_telnet_ops_complete(struct asf_telnet *at, u_int8_t* buf, s_int32_t len)
{
    asd_array_append(at->in, buf, len);
    at->cFunc(at);
    
    if(ASD_STR_LEN(at->out) == 2 && ASD_STR_CONTENT(at->out)[0] == ASF_TELNET_COMLEMENTED)
    {
        /* Complete current command */
        asd_str_clear(at->out);
        asf_telnet_newline(at);
        asl_telnet_send_prompt(at);
        asf_telnet_send_in(at);
    }
    else if(at->outSize != 0)
    {
        /* String matches more than one command */
        asf_telnet_newline(at);
        asf_telnet_send_out(at);
        asd_str_clear(at->out);
        asl_telnet_send_prompt(at);
        asd_array_truncate(at->in, ASD_ARRAY_COUNT(at->in) - 2);
        asf_telnet_send_in(at);
    }
    else
    {
        /* No match command */
        asd_array_truncate(at->in, ASD_ARRAY_COUNT(at->in) - 2);
    }
}
static void asl_telnet_ops_exec(struct asf_telnet *at, u_int8_t* buf, s_int32_t len)
{
    at->opFunc(at);
    if(at->newClient == FALSE)
    {
        if(ASD_STR_LEN(at->out) != 0)
        {
            asf_telnet_newline(at);
            asf_telnet_send_out(at);
            asd_str_clear(at->out);
        }
        asf_telnet_newline(at);
        asl_telnet_send_prompt(at);
        asd_array_clear(at->in);
    }
}
static void asl_telnet_ops_help(struct asf_telnet *at, u_int8_t* buf, s_int32_t len)
{
    asd_array_append(at->in, buf, len);
    asl_send(at->csock, buf, len);
    asf_telnet_newline(at);
    at->hFunc(at);
    asf_telnet_send_out(at);
    if(ASD_STR_LEN(at->out) >= 2 && (ASD_STR_CONTENT(at->out))[ASD_STR_LEN(at->out) - 2] != '\n')
        asf_telnet_newline(at);
    asd_str_clear(at->out);
    
    /* delete '?' and '\0' */
    asd_array_truncate(at->in, ASD_ARRAY_COUNT(at->in) - 2);
    asl_telnet_send_prompt(at);
    asf_telnet_send_in(at);
}

static void asl_telnet_ops_close(struct asf_telnet *at, u_int8_t* buf, s_int32_t len)
{
    asf_telnet_newline(at);
    at->newClient = TRUE;
}
void asf_telnet_close_client(struct asf_telnet *at)
{
    asl_telnet_ops_close(at, NULL, 0);
    asl_telnet_loop_release(at);
}

static void asl_telnet_ops_ctrl(struct asf_telnet *at, u_int8_t* buf, s_int32_t len)
{
    asl_print_hex(buf, len, "Telnet Control Commands");
    return;
}

void asl_telnet_default_ops_func(struct asf_telnet *at, u_int8_t* buf, s_int32_t len)
{
    asd_array_append(at->in, buf, len);
    asl_send(at->csock, buf, len);
}

static ASF_TELNET_OPS_FUNC asl_telnet_get_ops_func(u_int8_t ch)
{
    struct
    {
        u_int8_t ch;
        void(*func)(struct asf_telnet*, u_int8_t*, s_int32_t);
    }asl_telnet_ops[] = 
    {
        {ASF_TEL_IAC,             asl_telnet_ops_iac},
        {ASF_TELNET_CONTROL('C'), asl_telnet_ops_close},
        {0x1b,                    asl_telnet_ops_ctrl},
        {0x7f,                    asl_telnet_ops_backspace},
        {'\t',                    asl_telnet_ops_complete},
        {'\n',                    asl_telnet_ops_exec},
        {'\r',                    asl_telnet_ops_exec},
        {'?',                     asl_telnet_ops_help}
    };
    u_int32_t index;
    for(index = 0; index < ASL_COUNTOF(asl_telnet_ops); index++)
    {
        if(ch == asl_telnet_ops[index].ch)
            return asl_telnet_ops[index].func;
    }
    return asl_telnet_default_ops_func;
}
static void asl_telnet_timeout(void *arg)
{
    struct asf_telnet *at = (struct asf_telnet*)arg;
    u_int64_t cur;
    while(TRUE)
    {
        asl_sem_wait(&at->timeoutSem);
        cur = asl_time_get_stamp() - at->lastAccess;
        asl_sem_post(&at->timeoutSem);
        //asl_print_dbg("%lu vs %u", cur, at->timeout);
        if(cur  >= at->timeout)
            break;
        sleep(1);
    }
    asl_telnet_send_str(at, "Time Out");
    asf_telnet_newline(at);
    at->newClient = TRUE;
    asl_net_destroy_sock(&at->csock);
}
result_t asl_telnet_set_timeout(struct asf_telnet *at, u_int32_t timeout)
{
    if(at == NULL)
        return FAILURE;
    asl_sem_wait(&at->timeoutSem);
    if(at->timeout == 0 && timeout != 0 && at->newClient == FALSE)
    {
        at->lastAccess = asl_time_get_stamp();
        asl_thread_create(&at->timeoutThr, asl_telnet_timeout, at);
    }
    else if(timeout == 0)
    {
        asl_thread_cancel(&at->timeoutThr);
        asl_thread_join(&at->timeoutThr);
    }
    at->timeout = timeout;
    asl_sem_post(&at->timeoutSem);
    return SUCCESS;
}

static void asl_telnet_loop(void *arg)
{
    struct asf_telnet *at = (struct asf_telnet*)arg;
    asl_tcp_listen(at->sock, 1);
    s_int32_t rlen = 0;
    u_int8_t buf[4096] = {0};
    asl_print_dbg("ASL Telnet Start!!!");
    while(TRUE)
    {
        at->csock = asl_tcp_accept(at->sock);
        asl_thread_cleanup_begin(asl_telnet_loop_release, at->csock);
        if(at->timeout != 0)
        {
            at->lastAccess = asl_time_get_stamp();
            asl_thread_create(&at->timeoutThr, asl_telnet_timeout, at);
        }
        at->newClient = FALSE;
        asf_telnet_prenegotiate(at->csock);
        asf_telnet_newline(at);
        asl_telnet_send_prompt(at);
        while(!at->newClient)
        {
            asl_memset(buf, 0, 4096);
            rlen = asl_recv(at->csock, buf, 4096);
            if(rlen <= 0)
                break;
            //asl_print_hex(buf, rlen, "Telnet Receive");
            asl_sem_wait(&at->timeoutSem);
            at->lastAccess = asl_time_get_stamp();
            asl_sem_post(&at->timeoutSem);
            (asl_telnet_get_ops_func(buf[0]))(at, buf, rlen);
        }
        asl_telnet_loop_release(at);
        asl_thread_cleanup_end(ASL_THREAD_NOT_NEED_CLEANUP);
    }
}

struct asf_telnet* asf_telnet_create(struct asl_net_info *info, void *opFunc, void *hFunc, void *cFunc, char* name)
{
    if(info == NULL || opFunc == NULL || hFunc == NULL || cFunc == NULL)
        return NULL;
    struct asf_telnet *at = ASL_MALLOC_T(struct asf_telnet);
    const char* nstr = (name == NULL ? ASL_TELNET_DEFAULT_NAME : name);
    at->name = asd_str_create();
    asd_str_puts(at->name, nstr);
    at->opFunc = (ASF_TELNET_FUNC)opFunc;
    at->hFunc = (ASF_TELNET_FUNC)hFunc;
    at->cFunc = (ASF_TELNET_FUNC)cFunc;
    at->sock = asl_net_create_sock(info, ASL_NET_TCP);
    at->newClient = TRUE;
    at->in = asd_array_create(sizeof(u_int8_t));
    at->out = asd_str_create();
    asl_thread_create(&at->thr, asl_telnet_loop, at);

    asl_sem_create(&at->timeoutSem, "Telnet Timeout", 1);
    at->lastAccess = 0;
    at->timeout = 0;
    
    return at;
}


result_t asf_telnet_destroy(struct asf_telnet **atp)
{
    if(atp != NULL && *atp != NULL)
    {
        struct asf_telnet *at = *atp;
        asl_thread_cancel(&at->thr);
        asl_thread_join(&at->thr);
        asd_str_destroy(&at->out);
        asd_array_destroy(&at->in);
        asl_net_destroy_sock(&at->sock);
        asd_str_destroy(&at->name);
        asl_free(at);
        *atp = NULL;
    }
    
    return SUCCESS;
}
