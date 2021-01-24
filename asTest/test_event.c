#include "asl_inc.h"
#include "asf_event.h"

#define TEST_EVENT_COUNT 4
#define TEST_EVENT_INTERVAL 5


struct test_event_suit
{
    struct asf_event *asfEvent;
    s_int32_t eid;
};
void *test_event_post(void *arg)
{
    struct test_event_suit *t = (struct test_event_suit*)arg;
    srand(TEST_EVENT_INTERVAL + t->eid);
    result_t ret = FAILURE;
    s_int32_t waitTime;
    while(TRUE)
    {
        waitTime = 1 + rand()%TEST_EVENT_INTERVAL;
        sleep(waitTime);
        ret = asf_event_post(t->asfEvent, t->eid);
        asl_print_dbg("Post Event[%p|%d] %s", t->asfEvent, t->eid, asl_get_ret_str(ret));
    }
}
void *test_event_wait(void *arg)
{
    struct test_event_suit *t = (struct test_event_suit*)arg;
    struct asf_event *ae = t->asfEvent;
    s_int32_t eid = 0;
    while(TRUE)
    {
        eid = asf_event_pend(ae);
        asl_print_dbg("Pend Event[%p|%d]", ae, eid);
    }
}
void test_event()
{
    s_int32_t index;
    struct test_event_suit tes[8];
    struct test_event_suit *t;
    struct asf_event *asfEvent[2];
    asfEvent[0] = asf_event_create();
    asfEvent[1] = asf_event_create();
    asl_print_dbg("asfEvent[0]:%p  asfEvent[1]:%p", asfEvent[0], asfEvent[1]);
    for(index = 0; index < 8; index++)
    {
        tes[index].asfEvent = (index < 4 ? asfEvent[0] : asfEvent[1]);
    }
    for(index = 0; index < 8; index++)
    {
        tes[index].eid = asf_event_get_eid(tes[index].asfEvent);
        asl_print_dbg("tes[%d].eid:%d", index, tes[index].eid);
    }
    struct asl_thread pthr[8];
    struct asl_thread wthr[2];
    
    asl_thread_create(&wthr[0], test_event_wait, &tes[0]);
    asl_thread_create(&wthr[1], test_event_wait, &tes[4]);
    
    for(index = 0; index < 8; index++)
    {
        asl_thread_create(&pthr[index], test_event_post, &tes[index]);
    }

    while(TRUE)
    {
        sleep(1000);
    }
}
