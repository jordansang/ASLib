#ifndef ASTEST_MAIN_H
#define ASTEST_MAIN_H

#include "asl_inc.h"
#include "asf_ctrl.h"

void test_memg();
void test_memp();
void test_event();
void test_thread();
void test_timer();
void test_list();
void test_fifo();
void test_time();
void test_btree();
void test_math();
void test_asa();
void test_map();
void test_stream();
void test_net_core();
void test_thread_pool();
void test_tcp();
void test_telnet();
void test_tree();
void test_cli();
void test_control();
void test_string();
void test_shell();
void test_array();
void test_ctrl();
void test_log();

struct test_container
{
    const char* name;
    void (*func)();
};

struct test_container ut4algo[] = 
{
    {"asa", test_asa}
};
#define UT4ALGO_COUNT ASL_COUNTOF(ut4algo)

struct test_container ut4ds[] = 
{
    {"array", test_array},
    {"btree", test_btree},
    {"fifo", test_fifo},
    {"list", test_list},
    {"map", test_map},
    {"stream", test_stream},
    {"string", test_string},
    {"tree", test_tree}
};
#define UT4DS_COUNT ASL_COUNTOF(ut4ds)

struct test_container ut4fw[] = 
{
    {"ctrl", test_ctrl},
    {"event", test_event},
    {"log", test_log},
    {"memg", test_memg},
    {"memp", test_memp},
    {"netcore", test_net_core},
    {"thrp", test_thread_pool},
    {"timer", test_timer},
    {"fifo", test_fifo},
    {"list", test_list}
};
#define UT4FW_COUNT ASL_COUNTOF(ut4fw)

struct test_container ut4lib[] = 
{
    {"math", test_math},
    {"thread", test_thread},
    {"time", test_time},
    {"tcp", test_tcp},
    // {"telnet", test_telnet},
    {"cli", test_cli},
    // {"shell", test_shell}
};
#define UT4LIB_COUNT ASL_COUNTOF(ut4lib)

#endif /* ASTEST_MAIN_H */