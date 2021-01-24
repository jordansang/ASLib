#include "asl_inc.h"
#include "asf_event.h"
#include "asf_timer.h"

#define ASF_TIMER_DBG 0
static void asf_timer_item_dump(struct asf_timer_item *ati)
{
    if(ASF_TIMER_DBG)
    {
        if(ati == NULL)
        {
            asl_printf("ASF Timer Item point to NULL");
            return;
        }
        asl_printf("ASF Timer Item:\n");
        asl_printf("\tid:\t\t%d\n", ati->id);
        asl_printf("\tstart:\t\t%lu:%u\n", ati->start.sec, ati->start.usec);
        asl_printf("\tinterval:\t%lu:%u\n", ati->interval.sec, ati->interval.usec);
        asl_printf("\trepeat:\t\t%lu:%u\n", ati->repeat.sec, ati->repeat.usec);
        asl_printf("\tfunc:\t\t%p\n", ati->func);
        asl_printf("\targ:\t\t%p\n", ati->arg);
    }
}

void asf_timer_delfunc(void* item)
{
    struct asf_timer_item *ati = (struct asf_timer_item*)item;
    asl_free(ati);
}
s_int32_t asf_timer_cmpfunc(void* src, void* dst)
{
    if(dst == NULL)
    {
        return -1;
    }
    struct asf_timer_item *atiSrc = (struct asf_timer_item*)src;
    struct asf_timer_item *atiDst = (struct asf_timer_item*)dst;
    asl_time_val srcTime, dstTime;
    asl_time_add(&atiSrc->start, &atiSrc->interval, &srcTime);
    asl_time_add(&atiDst->start, &atiDst->interval, &dstTime);
    return asl_time_cmp(&srcTime, &dstTime);
}
static void* asf_timer_ontime(void* arg)
{
    struct asf_timer_item *ati = (struct asf_timer_item*)arg;
    asl_time_val curTime;
    asl_time_val waitTime;
    asl_time_val lastTime;
    const u_int64_t USEC_TO_SEC = 1000000;
    if(asl_time_add(&ati->start, &ati->interval, &lastTime) == NULL)
    {
        asl_print_err("Get Timer Values FAILED");
        return NULL;
    }
    if(asl_time_get_val(&curTime) == NULL)
    {
        asl_print_err("Get Current Time FAILED");
        return NULL;
    }
    if(asl_time_cmp(&curTime, &lastTime) == -1)
    {
        asl_time_sub(&lastTime, &curTime, &waitTime);
        usleep((waitTime.sec)*USEC_TO_SEC + waitTime.usec);
    }
        
    //asl_print_dbg("Start onTime Loop for %lu %lu %lu %lu", waitTime, ati->interval, ati->start, curTime);
    //asl_print_dbg("ati:%p", ati);
    //asl_print_dbg("ati->at:%p", ati->at);
    asf_event_post(ati->at->event, ati->at->onTimeEid);
    
    return NULL;
}
static void asf_timer_loop_deal_ontime(struct asf_timer *at, struct asf_timer_item *ati)
{
    s_int32_t ret;
    struct asl_thread thr;
    //asl_print_dbg("Deal with On Time");
    asf_timer_item_dump(ati);
    asd_list_del_item(at->timerList, ati);
    asl_thread_create(&thr, ati->func, ati->arg);
    asl_thread_detach(&thr);
    if(ati->repeat.sec != 0 || ati->repeat.usec != 0)
    {
        //ati->start = time(NULL);
        //asl_print_dbg("Readd repeat timer");
        asl_time_get_val(&ati->start);
        ati->interval = ati->repeat;
        asd_list_add_sort(at->timerList, ati);
    }
    else
    {
        asl_free(ati);
    }
    if(at->timerList->count != 0)
    {
        ati = (struct asf_timer_item*)asd_list_head(at->timerList);
        asl_thread_join(&at->onTimeThr);
        ret = asl_thread_create(&at->onTimeThr, asf_timer_ontime, ati);
        if(ret != SUCCESS)
            asl_print_err("Create thread FAILED");
    }
}
static void asf_timer_loop_deal_start(struct asf_timer *at, struct asf_timer_item *ati)
{
    struct asf_timer_item *nati = NULL;
    if(asl_thread_mutex_lock(&at->plmutex) == SUCCESS)
    {
        nati = (struct asf_timer_item*)asd_list_head(at->pendList);
        asd_list_del_item(at->pendList, nati);
        asl_thread_mutex_unlock(&at->plmutex);
        //asl_print_dbg("Deal with pend");
        asf_timer_item_dump(nati);
        asd_list_add_sort(at->timerList, nati);
        //asl_print_dbg("nati:%p ati:%p result:%d", nati, ati, asf_timer_cmpfunc(nati, ati));
        if(asf_timer_cmpfunc(nati, ati) < 0)
        {
            asl_thread_cancel(&at->onTimeThr);
            asl_thread_join(&at->onTimeThr);
            asl_thread_create(&at->onTimeThr, asf_timer_ontime, nati);
        }
    }
}
static void asf_timer_loop_deal_cancel(struct asf_timer *at, struct asf_timer_item *ati)
{
    struct asf_timer_item *nati = NULL;
    if(asl_thread_mutex_lock(&at->dlmutex) == SUCCESS)
    {
        nati = (struct asf_timer_item*)asd_list_head(at->delList);
        asd_list_del_item(at->delList, nati);
        asl_thread_mutex_unlock(&at->dlmutex);
        //asl_print_dbg("Deal with delete");
        asf_timer_item_dump(nati);
        if(nati->id == ati->id)
        {
            asl_thread_cancel(&at->onTimeThr);
            asl_thread_join(&at->onTimeThr);
            //asl_print_dbg("Delete timer %p", ati);
            //asl_print_dbg("asd_list count:%d", at->timerList->count);
            asd_list_del_item(at->timerList, ati);
            asl_free(ati);
            //asl_print_dbg("asd_list count:%d", at->timerList->count);
            if(at->timerList->count != 0)
            {
                ati = (struct asf_timer_item*)asd_list_head(at->timerList);
                //asl_print_dbg("Delete current, Start next");
                asl_thread_create(&at->onTimeThr, asf_timer_ontime, ati);
            }
        }
        else
        {
            struct asd_list_item *li = NULL;
            ASD_LIST_LOOP(at->timerList, ati, li)
            {
                if(nati->id == ati->id)
                {
                    //asl_print_dbg("Delete timer %p", ati);
                    asd_list_del_item(at->timerList, ati);
                    asl_free(ati);
                }
            }
        }
        asl_free(nati);
    }
}

static void* asf_timer_loop(void *arg)
{
    struct asf_timer *at = (struct asf_timer*)arg;
    s_int32_t eid = -1;
    struct asf_timer_item *ati = NULL;
    while(TRUE)
    {
        eid = asf_event_pend(at->event);
        //asl_print_dbg("onTimeEid:%d pendEid:%d delEid:%d currentEid:%d",
        //            at->onTimeEid, at->pendEid, at->delEid, eid);
        asl_thread_mutex_lock(&at->tlmutex);
        ati = (struct asf_timer_item*)asd_list_head(at->timerList);
        if(eid == at->onTimeEid)
        {
            asf_timer_loop_deal_ontime(at, ati);
        }
        else if(eid == at->pendEid)
        {
            asf_timer_loop_deal_start(at, ati);
        }
        else if(eid == at->delEid)
        {
            asf_timer_loop_deal_cancel(at, ati);
        }
        asl_thread_mutex_unlock(&at->tlmutex);
    }
    /* Never Come Here */
    //assert(0);
    return NULL;
}
struct asf_timer* asf_timer_create()
{
    struct asf_timer *at = (struct asf_timer*)asl_malloc(sizeof(struct asf_timer));
    at->id = asl_time(NULL)*rand();
    at->state = ASF_TIMER_INITIALIZED;

    asl_thread_mutex_init(&at->tlmutex);
    asl_thread_mutex_init(&at->plmutex);
    asl_thread_mutex_init(&at->dlmutex);
    
    at->timerList = asd_list_create();
    at->timerList->cmpFunc = asf_timer_cmpfunc;
    
    at->pendList = asd_list_create();

    at->delList = asd_list_create();
    
    at->event = asf_event_create();
    at->onTimeEid = asf_event_get_eid(at->event);
    at->pendEid = asf_event_get_eid(at->event);
    at->delEid = asf_event_get_eid(at->event);
    if(asl_thread_create(&at->mainThr, asf_timer_loop, at) != SUCCESS)
    {
        asl_print_err("Start Timer Main Loop FAILED");
        asf_timer_destroy(at);
        at = NULL;
    }
    
    return at;
}
s_int32_t asf_timer_start(struct asf_timer *at, asl_time_val interval, asl_time_val repeat, void* func, void *arg)
{
    if(at->state != ASF_TIMER_INITIALIZED)
    {
        asl_print_err("Timer NOT Initialized");
        return FAILURE;
    }
    /* TODO: Timer ID maybe duplicated */
    static s_int32_t id = 0;
    struct asf_timer_item *nati = (struct asf_timer_item*)asl_malloc(sizeof(struct asf_timer_item));
    if(nati == NULL)
    {
        asl_print_err("Malloc for new timer item FAILED");
        return FAILURE;
    }
    nati->at = at;
    nati->id = ++id;
    asl_time_get_val(&nati->start);
    nati->interval = interval;
    nati->repeat = repeat;
    nati->func = func;
    nati->arg = arg;
    asl_thread_mutex_lock(&at->plmutex);
    asd_list_add(at->pendList, nati);
    asl_thread_mutex_unlock(&at->plmutex);
    asf_event_post(at->event, at->pendEid);
    return nati->id;
}
result_t asf_timer_cancel(struct asf_timer *at, s_int32_t tid)
{
    if(at->state != ASF_TIMER_INITIALIZED)
    {
        asl_print_err("Timer NOT Initialized");
        return FAILURE;
    }
    struct asf_timer_item *nati = (struct asf_timer_item*)asl_malloc(sizeof(struct asf_timer_item));
    nati->id = tid;
    asl_thread_mutex_lock(&at->dlmutex);
    asd_list_add(at->delList, nati);
    asl_thread_mutex_unlock(&at->dlmutex);
    asf_event_post(at->event, at->delEid);
    return SUCCESS;
}
result_t asf_timer_destroy(struct asf_timer *at)
{
    if(at->state != ASF_TIMER_INITIALIZED)
    {
        return SUCCESS;
    }
    asl_thread_cancel(&at->onTimeThr);
    asl_thread_join(&at->onTimeThr);
    asl_thread_cancel(&at->mainThr);
    asl_thread_join(&at->mainThr);
    asd_list_destroy(at->timerList);
    at->timerList = NULL;
    asd_list_destroy(at->pendList);
    at->pendList = NULL;
    asl_thread_mutex_destroy(&at->tlmutex);
    asl_thread_mutex_destroy(&at->plmutex);
    asf_event_destroy(at->event);
    at->onTimeEid = at->pendEid = at->delEid = at->id = 0;

    at->state = 0;

    asl_free(at);
    return SUCCESS;
}
