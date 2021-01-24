#ifndef ASF_CTRL_H
#define ASF_CTRL_H

#include "asl_stdio.h"
#include "asl_thread.h"
#include "asd_array.h"
#include "asd_stream.h"
#include "asd_string.h"

#define ASF_CTRL_DEFAULT_INSIZE  (1024)
#define ASF_CTRL_INFINIT_TIMEOUT (-1)

enum ASF_CTRL_INTF_TYPE
{
    ASF_CTRL_INTF_TELNET,
    ASF_CTRL_INTF_SHELL,
    ASF_CTRL_INTF_CUSTOM,
    ASF_CTRL_INTF_ERR
};


struct asf_ctrl;
struct asf_ctrl_intf
{
    u_int8_t type;
    void* server;
    void* client;
    /* Initilize Interface */
    result_t (*init)(struct asf_ctrl*);
    /* Can be NULL */
    result_t (*accept)(struct asf_ctrl*);
    /* Mainly operation on ? \r \n \t ^C and echo */
    s_int32_t (*recv)(struct asf_ctrl*);
    /* Send string to client */
    s_int32_t (*send)(struct asf_ctrl*);
    /* Release intf resources */
    result_t (*close)(struct asf_ctrl*);
};
struct asf_ctrl
{
    struct asd_str *name;
    struct asd_str *prompt;
    struct asl_thread loopThr;
    struct asl_sem cliSem;
    struct asl_cli *cli;

    struct asf_ctrl_intf *intf;
    u_int8_t tmp[64];
    struct asd_stream *s;
    struct asd_array *in;
    struct asd_str *out;
    
    struct asd_str *cmd;
    struct asd_str *cmdList;
    bool helpFlag;
    char **argv;
    s_int32_t argc;

    struct asl_sem timeoutSem;
    u_int64_t lastAccess;
    u_int32_t timeout;
};

struct asf_ctrl* asf_ctrl_create(u_int8_t type, const char* name);
result_t asf_ctrl_start(struct asf_ctrl *ac);
result_t asf_ctrl_destroy(struct asf_ctrl **ac);
bool asf_ctrl_is_avalilable_input(void *input, s_int32_t len);
result_t asf_ctrl_add_dir(struct asf_ctrl *ac, const char* dir);
result_t asf_ctrl_add_cmd(struct asf_ctrl *ac, const char* dir, const char *cmd, void *func);

#define asf_ctrl_is_help(ac) (ac != NULL ? ac->helpFlag : FALSE)
#define asf_ctrl_ret_putf(ac, fmt, ...) asd_str_putf(ac->out, "\r\n"fmt, ##__VA_ARGS__)
#define asf_ctrl_ret_puts(ac, src) asd_str_putf(ac->out, "\r\n%s", src)
#define asf_ctrl_ret_putns(ac, src, n) asd_str_putns(ac->out, src, n)

#endif /* ASF_CTRL_H */
