#include "asf_ctrl.h"

#define TEST_ECHO1_HELP_STR "one_echo str <cr>"
#define TEST_ECHO4_HELP_STR "fore_echo str str str str <cr>"

static s_int32_t one_echo(struct asf_ctrl *ac)
{
    if(asf_ctrl_is_help(ac) || ac->argc != 1)
        return asf_ctrl_ret_puts(ac, TEST_ECHO1_HELP_STR);

    return asf_ctrl_ret_putf(ac, "test_echo1:%s", *(ac->argv));
}

static s_int32_t fore_echo(struct asf_ctrl *ac)
{
    if(asf_ctrl_is_help(ac) || ac->argc != 4)
        return asf_ctrl_ret_puts(ac, TEST_ECHO4_HELP_STR);
    char** param = ac->argv;
    return asf_ctrl_ret_putf(ac, "test_echo4:%s %s %s %s", param[0],param[1],param[2],param[3]);
}

#define TEST_PRINT1_HELP_STR "test_print1 str <cr>"
#define TEST_PRINT4_HELP_STR "test_print4 str str str str <cr>"

static s_int32_t one_print(struct asf_ctrl *ac)
{
    if(asf_ctrl_is_help(ac) || ac->argc != 1)
        return asf_ctrl_ret_puts(ac, TEST_ECHO1_HELP_STR);

    return asf_ctrl_ret_putf(ac, "test_print1:%s", *(ac->argv));
}

static s_int32_t fore_print(struct asf_ctrl *ac)
{
    if(asf_ctrl_is_help(ac) || ac->argc != 4)
        return asf_ctrl_ret_puts(ac, TEST_ECHO4_HELP_STR);
    char** param = ac->argv;
    return asf_ctrl_ret_putf(ac, "test_print4:%s %s %s %s", param[0],param[1],param[2],param[3]);
}


void test_ctrl()
{
    struct asf_ctrl *act = asf_ctrl_create(ASF_CTRL_INTF_TELNET, "Test CTRL Telnet");
    asf_ctrl_add_dir(act, "echo");
    asf_ctrl_add_dir(act, "print");
    asf_ctrl_add_dir(act, "1 2 3 4 5 6");
    asf_ctrl_add_dir(act, "pri eco 1");
    asf_ctrl_add_cmd(act, "echo", "one_echo", one_echo);
    asf_ctrl_add_cmd(act, "echo", "fore_echo", fore_echo);
    asf_ctrl_add_cmd(act, "print", "one_print", one_print);
    asf_ctrl_add_cmd(act, "print", "fore_print", fore_print);
    asf_ctrl_start(act);

    struct asf_ctrl *acs = asf_ctrl_create(ASF_CTRL_INTF_SHELL, "Test CTRL Shell");
    asf_ctrl_add_dir(acs, "echo");
    asf_ctrl_add_dir(acs, "print");
    asf_ctrl_add_dir(acs, "1 2 3 4 5 6");
    asf_ctrl_add_dir(acs, "pri eco 1");
    asf_ctrl_add_cmd(acs, "echo", "one_echo", one_echo);
    asf_ctrl_add_cmd(acs, "echo", "fore_echo", fore_echo);
    asf_ctrl_add_cmd(acs, "print", "one_print", one_print);
    asf_ctrl_add_cmd(acs, "print", "fore_print", fore_print);
    asf_ctrl_start(acs);

    while(TRUE)
        sleep(1000);
}
