#include "asl_inc.h"
#include "asf_thread_pool.h"

#define TEST_TP_COUNT 20

static void test_tp_thr1(void *arg)
{
    u_int64_t *stamp = (u_int64_t*)arg;
    asl_print_dbg("THR Stamp:%llu", *stamp);
}

static void test_tp1()
{
    struct asf_tp *at = asf_tp_create();
    u_int32_t count = TEST_TP_COUNT;
    u_int64_t stamp;
    asl_print_dbg("t:%d r:%d i:%d", at->total, at->running, at->idle);

    sleep(2);
    
    while(count--)
    {
        stamp = asl_time_get_stamp();
        asf_tp_start_routine(at, test_tp_thr1, &stamp);
        //asl_print_dbg("count:%d t:%d r:%d i:%d", count, at->total, at->running, at->idle);
    }

    asl_print_dbg("t:%d r:%d i:%d", at->total, at->running, at->idle);

    sleep(2);

    asl_print_dbg("t:%d r:%d i:%d", at->total, at->running, at->idle);

    count = TEST_TP_COUNT;
    while(count--)
    {
        sleep(1);
        if(count == TEST_TP_COUNT - 5)
            asf_tp_enable_collect(at);
        if(count == 5)
            asf_tp_disable_collect(at);
        stamp = asl_time_get_stamp();
        asf_tp_start_routine(at, test_tp_thr1, &stamp);
        //asl_print_dbg("count:%d t:%d r:%d i:%d", count, at->total, at->running, at->idle);
    }
    asl_print_dbg("t:%d r:%d i:%d", at->total, at->running, at->idle);

    asf_tp_destroy(&at);

    sleep(10);
}

static void test_tp_thr2_callback(void *arg)
{
    asl_print_dbg("test_tp_thr2_callback");
}

static void test_tp_thr2(void *arg)
{
    u_int64_t *stamp = (u_int64_t*)arg;
    while(TRUE)
    {
        asl_thread_cleanup_begin(test_tp_thr2_callback, NULL);
        sleep(1);
        asl_print_dbg("THR Stamp:%llu", *stamp);
        asl_thread_cleanup_end(0);
    }
}

static void test_tp2()
{
    struct asf_tp *at = asf_tp_create();
    asf_tp_enable_collect(at);

    u_int64_t stamp;
    int count = 5;
    while(count--)
    {
        sleep(1);
        stamp = asl_time_get_stamp();
        asf_tp_start_routine(at, test_tp_thr2, &stamp);
    }
    asl_print_dbg("t:%d r:%d i:%d", at->total, at->running, at->idle);
    sleep(2);
    asl_print_dbg("t:%d r:%d i:%d", at->total, at->running, at->idle);

    asf_tp_destroy(&at);

    asl_print_dbg("destroyed");

    sleep(10);
}

void test_thread_pool()
{
    test_tp2();
    return;
}
