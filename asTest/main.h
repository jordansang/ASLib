#ifndef ASTEST_MAIN_H
#define ASTEST_MAIN_H

#include "asl_inc.h"

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

struct test_container tc[] = 
{
    {"memp", test_memp},
    {"memg", test_memg},
    {"event", test_event}
};

#define TC_COUNT ASL_COUNTOF(tc)

#endif /* ASTEST_MAIN_H */