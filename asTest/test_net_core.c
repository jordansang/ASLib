#include "asl_inc.h"
#include "asd_stream.h"
#include "asf_net_core.h"

static void test_net_core_proc(struct asd_stream *s)
{
    sleep(3);
    //asl_print_hex(ASD_STREAM_DATA(s), ASD_STREAM_DSIZE(s), "test net core");
}

static u_int32_t test_net_core_prio_check(struct asd_stream *s)
{
    return (rand()%10);
}

static void test_net_core_thr_destroy1(void *arg)
{
    struct asf_nc *anc = (struct asf_nc*)arg;
    asf_nc_stop(&anc);
    asl_print_dbg("test_net_core_thr_destroy1");
}

static void test_net_core_thr_destroy2(void *arg)
{
    struct asd_stream *s = (struct asd_stream*)arg;
    asd_stream_destroy(&s);
    asl_print_dbg("test_net_core_thr_destroy2");
}
int flag = 1;
static void test_net_core_thr(void *arg)
{
    u_int16_t *port = (u_int16_t*)arg;
    u_int16_t delay  = 0xf & port[0];
    u_int64_t stamp;
    
    struct asl_net_info info;
    info.ip = 0x7f000001;
    info.port = port[0];

    struct asl_net_info dst;
    dst.ip = 0x7f000001;
    dst.port = port[1];

    struct asf_nc *anc = asf_nc_start(&info, &dst, test_net_core_prio_check, test_net_core_proc);
    //struct asf_nc *anc = asf_nc_start(info, NULL, NULL, NULL);
    asl_thread_cleanup_begin(test_net_core_thr_destroy1, anc);
    struct asd_stream *s = asd_stream_create(128);
    //asl_print_dbg("%04x anc:%p s:%p",port[0], anc, s);
    asl_thread_cleanup_begin(test_net_core_thr_destroy2, s);
    while(flag)
    {
        sleep(delay);
        if(port[0] == 10001)
        {
            asd_stream_clear(s);
            asd_stream_put_short(s, port);
            stamp = 0;
            asd_stream_put_longlong(s, &stamp);
            stamp = asl_time_get_stamp();
            asd_stream_put_longlong(s, &stamp);
            //asf_nc_tras_to(anc, s, info.ip, port[1]);
            asf_nc_tras(anc, s);
        }
    }
    asl_thread_cleanup_end(1);
    asl_thread_cleanup_end(1);
}

void test_net_core()
{
    u_int16_t thr1Port[2] = {10001, 10005}; //0x2711 0x2715
    u_int16_t thr2Port[2] = {10005, 10001};
    struct asl_thread thr1, thr2;

    asl_thread_create(&thr1, test_net_core_thr, thr1Port);
    asl_thread_create(&thr2, test_net_core_thr, thr2Port);

    /* Test for pthread cleanup and pthread cancel */
    //sleep(5);

    //asl_thread_cancel(&thr1);
    //asl_thread_join(&thr1);

    //asl_thread_cancel(&thr2);
    //asl_thread_join(&thr2);

    //asl_thread_create(&thr2, test_net_core_thr, thr2Port);
    /* Test for pthread cleanup and pthread cancel */
    
    while(TRUE)
        sleep(10000);
}
