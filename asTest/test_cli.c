#include "asl_inc.h"

#define TEST_CLI_REG_FUNC(func) asl_cli_register(cli, #func, func)


#define TEST_CLI_GEN_CMD(cmd, arg) \
    int cmd ## _param = arg; \
    void* cmd(void* val) \
    { \
        asl_print_dbg("%d", cmd ## _param); \
        return NULL; \
    }


TEST_CLI_GEN_CMD(tcf_1, 1)
TEST_CLI_GEN_CMD(tcf_2, 2)
TEST_CLI_GEN_CMD(tcf_3, 3)
TEST_CLI_GEN_CMD(tcf_4, 4)
TEST_CLI_GEN_CMD(tcf_5, 5)
TEST_CLI_GEN_CMD(tcf_2_1, 21)
TEST_CLI_GEN_CMD(tcf_2_2, 22)
TEST_CLI_GEN_CMD(tcf_3_1, 31)
TEST_CLI_GEN_CMD(tcf_3_2, 32)
TEST_CLI_GEN_CMD(tcf_3_3, 33)
TEST_CLI_GEN_CMD(tcf_3_2_1, 321)
TEST_CLI_GEN_CMD(tcf_3_2_2, 322)

void test_cli()
{
    struct asl_cli *cli = asl_cli_create("Test CLI");

    TEST_CLI_REG_FUNC(tcf_1);
    TEST_CLI_REG_FUNC(tcf_2);
    TEST_CLI_REG_FUNC(tcf_3);
    TEST_CLI_REG_FUNC(tcf_4);
    TEST_CLI_REG_FUNC(tcf_5);

    asl_printf("%s\n", asl_cli_get_list_all(cli));

    asl_cli_to_top(cli);
    asl_cli_set_cur(cli, "tcf_2");
    TEST_CLI_REG_FUNC(tcf_2_1);
    TEST_CLI_REG_FUNC(tcf_2_2);

    asl_printf("%s\n", asl_cli_get_list_all(cli));

    asl_cli_to_top(cli);
    asl_cli_set_cur(cli, "tcf_3");
    TEST_CLI_REG_FUNC(tcf_3_1);
    TEST_CLI_REG_FUNC(tcf_3_2);
    TEST_CLI_REG_FUNC(tcf_3_3);

    asl_printf("%s\n", asl_cli_get_list_all(cli));

    asl_cli_set_cur(cli, "tcf_3_2");
    TEST_CLI_REG_FUNC(tcf_3_2_1);
    TEST_CLI_REG_FUNC(tcf_3_2_2);

    asl_printf("%s\n", asl_cli_get_list_all(cli));
    
    asl_cli_to_top(cli);
    asl_cli_set_cur(cli, "tcf_3");
    asl_printf("%s\n", asl_cli_get_list(cli));

    asl_cli_release_list(cli);

    asl_cli_to_top(cli);
    asl_cli_exec_cmd(cli, "tcf_5", NULL);

    asl_cli_to_top(cli);
    asl_cli_set_cur(cli, "tcf_3");
    asl_cli_exec_cmd(cli, "tcf_3_2", NULL);

    asl_cli_destroy(&cli);

    return;
}
