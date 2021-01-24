#ifndef ASL_CLI_H
#define ASL_CLI_H

#include "asl_stdio.h"

#define ASL_CLI_DEF_PROMPT "Default CLI"

typedef void*(*ASL_CLI_FUNC)(void* arg);

struct asl_cli_item
{
    struct asl_cli_item *top;
    struct asl_cli_item *upper;
    struct asl_cli_item *next;
    struct asl_cli_item *child;

    ASL_CLI_FUNC func;
    char* cmd;
};

struct asl_cli
{
    struct asl_cli_item *top;
    struct asl_cli_item *cur;

    char* listBuf;
    
    bool  listChanged;
    char* listAllBuf;
};

struct asl_cli* asl_cli_create(const char* prompt);
result_t asl_cli_register(struct asl_cli *cli, const char* cmd, ASL_CLI_FUNC func);
result_t asl_cli_unregister(struct asl_cli *cli, const char* cmd);

char* asl_cli_get_list(struct asl_cli *cli);
char* asl_cli_get_list_all(struct asl_cli *cli);
#define asl_cli_release_list(cli) \
    do \
    { \
        asl_free(cli->listBuf); \
        cli->listBuf = NULL; \
        asl_free(cli->listAllBuf); \
        cli->listAllBuf = NULL; \
        cli->listChanged = TRUE; \
    }while(0);
    
result_t asl_cli_set_cur(struct asl_cli *cli, const char* cmd);
#define asl_cli_to_upper(cli) ((cli == NULL ? FAILURE : (cli->cur = cli->cur->upper, SUCCESS))
#define asl_cli_to_top(cli) (cli == NULL ? FAILURE : (cli->cur = cli->top, SUCCESS))
bool asl_cli_cur_is_top(struct asl_cli *cli);

result_t asl_cli_destroy(struct asl_cli **clip);
void* asl_cli_exec_cmd(struct asl_cli *cli, char* cmd, void* arg);

#define asl_cli_is_dir(aci) (aci != NULL && aci->child != NULL)
bool asl_cli_cmd_is_dir(struct asl_cli *cli, const char* cmd);
#define asl_cli_register_dir(cli, cmd) asl_cli_register(cli, cmd, NULL)

#endif /* ASL_CLI_H */
