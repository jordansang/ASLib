#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_syscmd.h"
#include "asl_memory.h"
#include "asf_ctrl.h"
#include <termio.h>

static void asf_ctrl_intf_shell_nolineset(u_int8_t state)
{
    static u_int8_t got = 0;
    static struct termio old;
    struct termio arg;
    if(state == ENABLE)
    {
        ioctl(0, TCGETA, &old);
        got = 1;

        arg = old;
        arg.c_lflag &= ~(ECHO | ICANON);
        arg.c_iflag &= ~(ISTRIP | INPCK);
        arg.c_cc[VMIN] = 1;
        arg.c_cc[VTIME] = 0;
        ioctl(0, TCSETAW, &arg);
    }
    else if(got != 0)
    {
        ioctl(0, TCSETAW, &old);
    }
}

static result_t asf_ctrl_intf_shell_init(struct asf_ctrl *ac)
{
    asf_ctrl_intf_shell_nolineset(ENABLE);
    return SUCCESS;
}
static result_t asf_ctrl_intf_shell_accept(struct asf_ctrl *ac)
{
    return SUCCESS;
}
static result_t asf_ctrl_intf_shell_recv(struct asf_ctrl *ac)
{
    s_int32_t rlen = 0;
    do
    {
        //asl_print_dbg("Ready to fread...");
        rlen = fread(ac->tmp, 1, 1, stdin);
        if(rlen < 0)
        {
            asl_print_err("Disconnected from Client...");
            return FAILURE;
        }
        //asl_print_hex(ac->tmp, rlen, "recv");
        if(asf_ctrl_is_avalilable_input(ac->tmp, rlen))
        {
            break;
        }
        asl_memset(ac->tmp, 0, sizeof(ac->tmp));
    }while(TRUE);
    asd_stream_put(ac->s, ac->tmp, rlen);
    return SUCCESS;
}
static result_t asf_ctrl_intf_shell_send(struct asf_ctrl *ac)
{
    fwrite(ASD_STR_CONTENT(ac->out), ASD_STR_TLEN(ac->out), 1, stdout);
    return SUCCESS;
}
static result_t asf_ctrl_intf_shell_close(struct asf_ctrl *ac)
{
    fwrite("\n", 2, 1, stdout);
    struct asl_syscmd cmd;
    cmd.cmd = "/bin/bash";
    cmd.retBuf = NULL;
    cmd.retSize = -1;
    asf_ctrl_intf_shell_nolineset(DISABLE);
    asl_syscmd_exec(&cmd);
    asf_ctrl_intf_shell_nolineset(ENABLE);
    return SUCCESS;
}

struct asf_ctrl_intf shellIntf = 
{
    .type = ASF_CTRL_INTF_SHELL,
    .init = asf_ctrl_intf_shell_init,
    .accept = asf_ctrl_intf_shell_accept,
    .recv = asf_ctrl_intf_shell_recv,
    .send = asf_ctrl_intf_shell_send,
    .close = asf_ctrl_intf_shell_close
};

