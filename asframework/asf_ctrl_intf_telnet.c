#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asl_file.h"
#include "asl_net.h"
#include "asl_ipv4.h"
#include "asl_tcp.h"
#include "asd_array.h"
#include "asd_string.h"
#include "asd_stream.h"
#include "asf_ctrl.h"

#define ASF_CTRL_INTF_TELNET_PORT 10001
/* Basic Commands */
#define ASF_CTRL_INTF_TEL_IAC     255 /* All Command Initial */
#define ASF_CTRL_INTF_TEL_DONT    254 /* Remote Not enable the option */
#define ASF_CTRL_INTF_TEL_DO      253 /* Remote Enable the option */
#define ASF_CTRL_INTF_TEL_WONT    252 /* Local Not enable the option */
#define ASF_CTRL_INTF_TEL_WILL    251 /* Local Enable the option */
#define ASF_CTRL_INTF_TEL_SB      250 /* Subcommand Initial */
#define ASF_CTRL_INTF_TEL_SE      240 /* Subcommand end */
#define ASF_CTRL_INTF_TEL_EOF     '\0'

/* Subcommands */
#define ASF_CTRL_INTF_TELOPT_ECHO 1   /* echo */
#define ASF_CTRL_INTF_TELOPT_SGA  3   /* suppress go ahead */
#define ASF_CTRL_INTF_TELOPT_NAWS 31   /* approximate message size */
#define ASF_CTRL_INTF_TELOPT_LINEMODE 34  /* Linemode option */

static result_t asf_ctrl_intf_telnet_init(struct asf_ctrl *ac)
{
    struct asf_ctrl_intf *at = ac->intf;
    struct asl_socket *ssock;;
    struct asl_net_info info;
    info.ip = ASL_IPV4_INADDR_ANY;
    info.port = ASF_CTRL_INTF_TELNET_PORT;
    ssock = asl_net_create_sock(&info, ASL_NET_TCP);
    asl_tcp_listen(ssock, 1);
    at->server = ssock;
    //asl_print_dbg("Create telnet server socket %d", ssock->s);
    return SUCCESS;
}
static void asf_ctrl_intf_telnet_cmddump(struct asf_ctrl *ac, s_int32_t size)
{
    u_int8_t *buf = ac->tmp;
    struct
    {
        u_int8_t ch;
        const char *str;
    }asf_telnet_iac_str[] = 
    {
        {ASF_CTRL_INTF_TEL_IAC,     "IAC "},
        {ASF_CTRL_INTF_TEL_WILL,    "WILL "},
        {ASF_CTRL_INTF_TEL_WONT,    "WONT "},
        {ASF_CTRL_INTF_TEL_DO,      "DO "},
        {ASF_CTRL_INTF_TEL_DONT,    "DONT "},
        {ASF_CTRL_INTF_TEL_SB,      "SB "},
        {ASF_CTRL_INTF_TEL_SE,      "SE "},
        {ASF_CTRL_INTF_TELOPT_SGA,  "TELOPT_SGA \n"},
        {ASF_CTRL_INTF_TELOPT_ECHO, "TELOPT_ECHO \n"},
        {ASF_CTRL_INTF_TELOPT_NAWS, "TELOPT_NAWS \n"}
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
static s_int32_t asf_ctrl_intf_telnet_cmd_do(struct asl_socket *sock, char telopt)
{
    char cmd[4] = { ASF_CTRL_INTF_TEL_IAC, ASF_CTRL_INTF_TEL_DO, 0, ASF_CTRL_INTF_TEL_EOF };
    cmd[2] = telopt;
    return asl_send(sock, cmd, 4);
}

static s_int32_t asf_ctrl_intf_telnet_cmd_dont(struct asl_socket *sock, char telopt)
{
    char cmd[4] = { ASF_CTRL_INTF_TEL_IAC, ASF_CTRL_INTF_TEL_DONT, 0, ASF_CTRL_INTF_TEL_EOF };
    cmd[2] = telopt;
    return asl_send(sock, cmd, 4);
}

static s_int32_t asf_ctrl_intf_telnet_cmd_will(struct asl_socket *sock, char telopt)
{
    char cmd[4] = { ASF_CTRL_INTF_TEL_IAC, ASF_CTRL_INTF_TEL_WILL, 0, ASF_CTRL_INTF_TEL_EOF };
    cmd[2] = telopt;
    return asl_send(sock, cmd, 4);
}

static s_int32_t asf_ctrl_intf_telnet_cmd_wont(struct asl_socket *sock, char telopt)
{
    char cmd[4] = { ASF_CTRL_INTF_TEL_IAC, ASF_CTRL_INTF_TEL_WONT, 0, ASF_CTRL_INTF_TEL_EOF };
    cmd[2] = telopt;
    return asl_send(sock, cmd, 4);
}

static void asf_ctrl_intf_telnet_prenegotiate(struct asl_socket *sock)
{
    asf_ctrl_intf_telnet_cmd_do(sock, ASF_CTRL_INTF_TELOPT_ECHO);
    asf_ctrl_intf_telnet_cmd_do(sock, ASF_CTRL_INTF_TELOPT_SGA);
    asf_ctrl_intf_telnet_cmd_do(sock, ASF_CTRL_INTF_TELOPT_NAWS);
    asf_ctrl_intf_telnet_cmd_will(sock, ASF_CTRL_INTF_TELOPT_ECHO);
    asf_ctrl_intf_telnet_cmd_will(sock, ASF_CTRL_INTF_TELOPT_SGA);
    asf_ctrl_intf_telnet_cmd_wont(sock, ASF_CTRL_INTF_TELOPT_NAWS);
    asf_ctrl_intf_telnet_cmd_dont(sock, ASF_CTRL_INTF_TELOPT_LINEMODE);
}
#define ASF_CTRL_INTF_TELNET_BANNER "ASF CONTROL TELNET\r\n\"exit\" to disconnect\r\n"
static result_t asf_ctrl_intf_telnet_accept(struct asf_ctrl *ac)
{
    struct asf_ctrl_intf *at = ac->intf;
    struct asl_socket *csock;
    csock = asl_tcp_accept((struct asl_socket*)(at->server));
    asf_ctrl_intf_telnet_prenegotiate(csock);
    asl_send(csock, ASF_CTRL_INTF_TELNET_BANNER, asl_strtlen(ASF_CTRL_INTF_TELNET_BANNER));
    at->client = csock;
    //asl_print_dbg("Get new Client socket %d", csock->s);
    return SUCCESS;
}

static void asf_ctrl_intf_telnet_ctrl(struct asf_ctrl *ac, s_int32_t len)
{
    //TODO: Proccess Telnet control commands start with '[^'
}
static void asf_ctrl_intf_telnet_deliver(struct asf_ctrl *ac, s_int32_t len)
{
    u_int8_t *p = ac->tmp;
    struct
    {
        u_int8_t ch;
        void(*func)(struct asf_ctrl *ac, s_int32_t);
    }asl_telnet_ops[] = 
    {
        {ASF_CTRL_INTF_TEL_IAC,   asf_ctrl_intf_telnet_cmddump},
        {0x1b,                    asf_ctrl_intf_telnet_ctrl},
    };
    u_int32_t index;
    u_int32_t count = 0;
    while(count < len)
    {
        for(index = 0; index < ASL_COUNTOF(asl_telnet_ops); index++)
        {
            if(p[count] == asl_telnet_ops[index].ch)
            {
                (asl_telnet_ops[index].func)(ac, len);
                return;
            }
        }
        count++;
    }
}
static result_t asf_ctrl_intf_telnet_recv(struct asf_ctrl *ac)
{
    struct asf_ctrl_intf *at = ac->intf;
    struct asl_socket *csock = (struct asl_socket*)(at->client);
    s_int32_t state = 0;
    asl_net_set_nonblocking(csock, state);
    s_int32_t rlen = 0;
    do
    {
        rlen = asl_recv(at->client, ac->tmp, sizeof(ac->tmp));
        if(rlen < 0)
        {
            //asl_print_err("Disconnected from Client...");
            return FAILURE;
        }
        //asl_print_hex(ac->tmp, rlen, "recv");
        if(asf_ctrl_is_avalilable_input(ac->tmp, rlen))
        {
            break;
        }
        asf_ctrl_intf_telnet_deliver(ac, rlen);
        asl_memset(ac->tmp, 0, sizeof(ac->tmp));
    }while(TRUE);
    
    state = 1;
    asl_net_set_nonblocking(csock, state);
    while(TRUE)
    {
        asd_stream_put(ac->s, ac->tmp, rlen);
        rlen = asl_recv(at->client, ac->tmp, sizeof(ac->tmp));
        if(rlen < 0)
        {
            /* EAGAIN */
            state = 0;
            asl_net_set_nonblocking(csock, state);
            break;
        }
    }
    return SUCCESS;
}
static result_t asf_ctrl_intf_telnet_send(struct asf_ctrl *ac)
{
    struct asf_ctrl_intf *at = ac->intf;
    struct asl_socket *csock = (struct asl_socket*)(at->client);
    asl_send(csock, ASD_STR_CONTENT(ac->out), ASD_STR_TLEN(ac->out));
    return SUCCESS;
}
static result_t asf_ctrl_intf_telnet_close(struct asf_ctrl *ac)
{
    struct asf_ctrl_intf *at = ac->intf;
    struct asl_socket *csock = (struct asl_socket*)(at->client);
    asl_net_destroy_sock(&csock);
    at->client = NULL;
    return SUCCESS;
}

struct asf_ctrl_intf telnetIntf = 
{
    .type = ASF_CTRL_INTF_TELNET,
    .init = asf_ctrl_intf_telnet_init,
    .accept = asf_ctrl_intf_telnet_accept,
    .recv = asf_ctrl_intf_telnet_recv,
    .send = asf_ctrl_intf_telnet_send,
    .close = asf_ctrl_intf_telnet_close
};
