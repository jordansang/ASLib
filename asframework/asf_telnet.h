#ifndef ASF_TELNET_H
#define ASF_TELNET_H

#include "asl_net.h"
#include "asl_ipv4.h"
#include "asl_tcp.h"
#include "asl_thread.h"

#define ASL_TELNET_DEFAULT_NAME   "Default"
#define ASL_TELNET_DEFAULT_REPLAY "NO OPFUNC"
#define ASL_TELNET_DEFAULT_HELP   "NO HELP in DEFAULT MODE"

/* Basic Commands */
#define ASF_TEL_IAC     255 /* All Command Initial */
#define ASF_TEL_DONT    254 /* Remote Not enable the option */
#define ASF_TEL_DO      253 /* Remote Enable the option */
#define ASF_TEL_WONT    252 /* Local Not enable the option */
#define ASF_TEL_WILL    251 /* Local Enable the option */
#define ASF_TEL_SB      250 /* Subcommand Initial */
#define ASF_TEL_SE      240 /* Subcommand end */
#define ASF_TEL_EOF     '\0'

/* Subcommands */
#define ASF_TELOPT_ECHO 1   /* echo */
#define ASF_TELOPT_SGA  3   /* suppress go ahead */
#define ASF_TELOPT_NAWS 31   /* approximate message size */
#define ASF_TELOPT_LINEMODE 34  /* Linemode option */

/* Complemented inBuf */
#define ASF_TELNET_COMLEMENTED 0xfe

struct asf_telnet;

typedef u_int32_t(*ASF_TELNET_FUNC)(struct asf_telnet *at);

struct asf_telnet
{
    struct asd_str *name;
    struct asl_thread thr;
    struct asl_socket *sock;
    bool newClient;
    struct asl_socket *csock;
    s_int16_t width;
    s_int16_t height;
    
    ASF_TELNET_FUNC opFunc;
    void* opArg;
    ASF_TELNET_FUNC hFunc;
    void* hArg;
    ASF_TELNET_FUNC cFunc;
    void* cArg;

    struct asd_array *in;
    struct asd_str *out;

    struct asl_thread timeoutThr;
    struct asl_sem timeoutSem;
    u_int64_t lastAccess;
    u_int32_t timeout;
};

#define ASF_TELNET_CONTROL(ch) (ch - '@')
typedef void(*ASF_TELNET_OPS_FUNC)(struct asf_telnet*, u_int8_t*, s_int32_t);
struct asf_telnet* asf_telnet_create(struct asl_net_info *info, void *opFunc, void *hFunc, void *cFunc, char* name);
result_t asf_telnet_destroy(struct asf_telnet **atp);
void asf_telnet_close_client(struct asf_telnet *at);
#define asf_telnet_is_new_client(at) (at->newClient)

#endif /* ASF_TELNET_H */
