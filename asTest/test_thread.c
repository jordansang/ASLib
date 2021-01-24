#include "asl_inc.h"

void* test_thread_thr(void* arg)
{
    while(TRUE)
    {
        asl_print_dbg("Waiting in thr");
        //sleep(5);
        //asl_print_dbg("Get 5 sec sleep");
    }
}

void test_thread()
{
    struct asl_thread thr;
    asl_thread_create(&thr, test_thread_thr, NULL);
    sleep(1);
    asl_print_dbg("Start join thr");
    //asl_thread_join(&thr);
    pthread_cancel(thr.id);
    asl_print_dbg("After join thr");

    sleep(3);
    asl_print_dbg("Main Exit");
}
