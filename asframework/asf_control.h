#ifndef ASF_CONTROL_H
#define ASF_CONTROL_H

#include "asl_stdio.h"
#include "asl_thread.h"
#include "asl_cli.h"
#include "asf_telnet.h"
#include "asf_shell.h"
#include "asd_string.h"

#define ASF_CTRL_EXIT "exit"

enum ASF_CTRL_TYPE
{
    ASF_CTRL_TYPE_LOCAL,
    ASF_CTRL_TYPE_REMOTE,
    ASF_CTRL_TYPE_ERROR
};

#define ASF_RC_PORT 10001

struct asf_ctrl
{
    u_int8_t type;
    char* name;
    struct asl_sem cliSem;
    struct asl_cli *cli;
    struct asf_telnet *at;
    struct asl_sh *as;

    struct asd_str *cmd;
    struct asd_str *out;
    s_int32_t inCount;
    char** in;
};

typedef s_int32_t(*ASF_CTRL_CALLBACK)(struct asf_ctrl*);

result_t asf_control_add_cmd(struct asf_ctrl *ac, char* path, char* cmd, ASF_CTRL_CALLBACK func);
result_t asf_control_rem_cmd(struct asf_ctrl *ac, char* path, char* cmd);
#define ASF_CTRL_ARG(ac) (ac->in)
#define ASF_CTRL_ARG_COUNT(ac) (ac->inCount)
#define asf_ctrl_is_help_cmd(ac) (ac->inCount == 1 && **(ac->in) == '?')
#define asf_ctrl_ret_putf(ac, fmt, ...) asd_str_putf(ac->out, fmt, ##__VA_ARGS__)
#define asf_ctrl_ret_puts(ac, src) asd_str_puts(ac->out, src)
#define asf_ctrl_ret_putns(ac, src, n) asd_str_putns(ac->out, src, n)

/* Local Control */
struct asf_ctrl * asf_lc_create(const char* name);
result_t asf_lc_start(struct asf_ctrl *ac);
void asf_lc_destroy(struct asf_ctrl **acp);
#define asf_lc_add_cmd(ac, pcmd, func) asf_control_add_cmd(ac, pcmd, #func, func)
#define asf_lc_add_dir(ac, path, dir) asf_control_add_cmd(ac, path, dir, NULL)

/* Remote Control */
struct asf_ctrl * asf_rc_create(const char* name);
result_t asf_rc_start(struct asf_ctrl *ac);
void asf_rc_destroy(struct asf_ctrl **acp);
#define asf_rc_add_cmd(ac, path, func) asf_control_add_cmd(ac, path, #func, func)
#define asf_rc_add_dir(ac, path, dir) asf_control_add_cmd(ac, path, dir, NULL)

#endif /* ASF_CONTROL_H */
