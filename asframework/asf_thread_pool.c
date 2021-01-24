#include "asl_inc.h"
#include "asf_thread_pool.h"

struct asf_tp_item
{
    struct asf_tp *at;
    struct asl_thread thr;
    struct asl_sem enableSem;
    ASF_TP_ROUTINE func;
    void *arg;
};

static s_int32_t asf_tp_cmpFunc(void *a, void *b)
{
    return (a == b ? 0 : -1);
}

static void asf_tp_delFunc(void *val)
{
    struct asf_tp_item *ati = (struct asf_tp_item*)val;
    asl_thread_cancel(&ati->thr);
    asl_thread_join(&ati->thr);
    asl_sem_destroy(&ati->enableSem);
    asl_free(ati);
}

static void* asf_tp_thr(void *arg)
{
    struct asf_tp_item *ati = (struct asf_tp_item*)arg;
    while(TRUE)
    {
        asl_sem_wait(&ati->enableSem);
        if(ati->func != NULL)
            ati->func(ati->arg);
        ati->func = NULL;
        ati->arg = NULL;
        asl_sem_wait(&ati->at->atSem);
        asd_list_del_item(ati->at->runningList, ati);
        asd_list_add(ati->at->idleList, ati);
        ati->at->idle++;
        ati->at->running--;
        asl_sem_post(&ati->at->atSem);
    }
    return NULL;
}

static void asf_tp_create_item(struct asf_tp* at)
{
    struct asf_tp_item *ati = (struct asf_tp_item*)asl_malloc(sizeof(struct asf_tp_item));
    ati->at = at;
    asl_sem_create(&ati->enableSem, "Thread Pool Item", 0);    
    asl_thread_create(&ati->thr, asf_tp_thr, ati);
    asd_list_add(at->idleList, ati);
    at->total++;
    at->idle++;
}

static void* asf_tp_maintain(void *arg)
{
    struct asf_tp *at = (struct asf_tp*)arg;
    struct asf_tp_item *ati;
    while(TRUE)
    {
        asl_sem_wait(&at->atSem);
        asl_thread_set_cancel_disable();
        if(at->total != 0 && ((double)(at->idle))/((double)(at->total)) > ASF_TP_IDLE_PERCENT)
        {
            at->idleLast++;
        }
        else
        {
            at->idleLast = 0;
        }
        //asl_print_dbg("at->idleLast:%u i:%u t:%u ", at->idleLast, at->idle,at->total);
        if(at->idleLast == ASF_TP_IDLE_NEED_CLEAN)
        {
            u_int32_t cleanCount = (u_int32_t)((double)(at->total) * ASF_TP_IDLE_CLEAN_PERCENT);
            while(cleanCount--)
            {
                ati = (struct asf_tp_item*)asd_list_head(at->idleList);
                asd_list_del_head(at->idleList);
                asf_tp_delFunc(ati);
                at->idle--;
                at->total--;
            }
            at->idleLast = 0;
        }
        asl_sem_post(&at->atSem);
        asl_thread_set_cancel_enable();
        sleep(ASF_TP_MAINTAIN_INTERVAL);
    }

    return NULL;
}

struct asf_tp* asf_tp_create()
{
    struct asf_tp *at = (struct asf_tp*)asl_malloc(sizeof(struct asf_tp));
    asl_sem_create(&at->atSem, "Thread Pool", 1);
    at->total = 0;
    at->running = 0;
    at->idle = 0;
    at->idleList = asd_list_create();
    at->idleList->cmpFunc = asf_tp_cmpFunc;
    at->runningList= asd_list_create();
    at->runningList->cmpFunc = asf_tp_cmpFunc;

    at->idleLast = 0;
    at->collectState = DISABLE;

    return at;
}
void asf_tp_start_routine(struct asf_tp *at, ASF_TP_ROUTINE func, void* arg)
{
    if(at == NULL || func == NULL)
        return;
    struct asf_tp_item *ati;
    asl_sem_wait(&at->atSem);
    if(at->idle == 0)
    {
        asf_tp_create_item(at);
    }
    ati = (struct asf_tp_item*)asd_list_head(at->idleList);
    asd_list_del_head(at->idleList);
    asd_list_add(at->runningList, ati);
    at->idle--;
    at->running++;
    asl_sem_post(&at->atSem);
    ati->func = func;
    ati->arg = arg;
    asl_sem_post(&ati->enableSem);
}

void asf_tp_destroy(struct asf_tp **atp)
{
    if(atp == NULL || *atp == NULL)
        return;
    struct asf_tp *at = *atp;
    asl_sem_wait(&at->atSem);
    if(at->collectState == ENABLE)
    {
        asl_thread_cancel(&at->maintainThr);
        asl_thread_join(&at->maintainThr);
    }
    at->idleList->delFunc = asf_tp_delFunc;
    asd_list_destroy(at->idleList);
    at->runningList->delFunc = asf_tp_delFunc;
    asd_list_destroy(at->runningList);
    asl_sem_post(&at->atSem);
    asl_sem_destroy(&at->atSem);
    asl_free(at);
    *atp = NULL;
}

result_t asf_tp_enable_collect(struct asf_tp *at)
{
    if(at == NULL)
        return FAILURE;
    result_t ret = FAILURE;
    asl_sem_wait(&at->atSem);
    if(at->collectState == ENABLE || asl_thread_create(&at->maintainThr, asf_tp_maintain, at) == SUCCESS)
    {
        at->collectState = ENABLE;
        ret = SUCCESS;
    }
    asl_sem_post(&at->atSem);
    return ret;
}
result_t asf_tp_disable_collect(struct asf_tp *at)
{
    if(at == NULL)
        return FAILURE;
    result_t ret = FAILURE;
    asl_sem_wait(&at->atSem);
    if(at->collectState != DISABLE)
    {
        asl_thread_cancel(&at->maintainThr);
        asl_thread_join(&at->maintainThr);
        at->collectState = DISABLE;
        ret = SUCCESS;
    }
    asl_sem_post(&at->atSem);
    return ret;
}
