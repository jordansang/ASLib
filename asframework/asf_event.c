#include "asl_inc.h"
#include "asd_list.h"
#include "asd_fifo.h"
#include "asf_event.h"

static void asf_event_del_func(void *val)
{
    struct asf_event_item *aei = (struct asf_event_item*)val;
    if(aei != NULL)
    {
        asl_free(aei);
    }
    
    return;
}
struct asf_event* asf_event_create()
{
    struct asf_event *ae = (struct asf_event*)asl_malloc(sizeof(struct asf_event));
    if(ae == NULL)
    {
        asl_print_err("Malloc for ASF Event FAILED");
        return NULL;
    }
    if(asl_thread_mutex_init(&ae->listMutex) != SUCCESS)
    {
        asl_print_err("Create ASF Event List mutex FAILED");
        asl_free(ae);
        return NULL;
    }

    if(asl_thread_mutex_init(&ae->fifoMutex) != SUCCESS)
    {
        asl_print_err("Create ASF Event Fifo mutex FAILED");
        asl_thread_mutex_destroy(&ae->listMutex);
        asl_free(ae);
        return NULL;
    }
    
    if(asl_sem_create(&ae->esem, "ASF Event Sem", 0) != SUCCESS)
    {
        asl_print_err("Create ASF Event Semaphore FAILED");
        asl_thread_mutex_destroy(&ae->listMutex);
        asl_thread_mutex_destroy(&ae->fifoMutex);
        asl_free(ae);
        return NULL;
    }

    ae->eventList = asd_list_create();
    ae->eventList->delFunc = asf_event_del_func;
    
    ae->eventFifo = asd_fifo_create();
    ae->eventFifo->delFunc = asf_event_del_func;
        
    return ae;
}

result_t asf_event_destroy(struct asf_event *ae)
{
    if(ae == NULL)
    {
        return SUCCESS;
    }
    
    if(asl_thread_mutex_destroy(&ae->listMutex) != SUCCESS)
    {
        asl_print_err("Destroy ASF Event List mutex FAILED");
        return FAILURE;
    }

    if(asl_thread_mutex_destroy(&ae->fifoMutex) != SUCCESS)
    {
        asl_print_err("Destroy ASF Event Fifo mutex FAILED");
        return FAILURE;
    }

    if(asl_sem_destroy(&ae->esem) != SUCCESS)
    {
        asl_print_err("Destroy ASF Event Semaphore FAILED");
        return FAILURE;
    }

    asd_list_destroy(ae->eventList);
    asd_fifo_destroy(ae->eventFifo);

    return asl_free(ae);
}

s_int32_t asf_event_get_eid(struct asf_event *ae)
{
    if(ae == NULL)
    {
        asl_print_err("ASF Event is NOT Initialized");
        return FAILURE;
    }
    s_int32_t eid = 1;
    struct asf_event_item *caei;
    struct asf_event_item *aei = (struct asf_event_item*)asl_malloc(sizeof(struct asf_event_item));
    if(aei == NULL)
    {
        asl_print_err("Malloc for ASF Event Item FAILED");
        return FAILURE;
    }
    asl_thread_mutex_lock(&ae->listMutex);
    struct asd_list_item *li = NULL;
    /* TODO: This may cause leek error when applied for too many eid */
    ASD_LIST_LOOP(ae->eventList, caei, li)
    {
        eid += caei->eid;
    }
    aei->eid = eid;
    
    if(asd_list_add(ae->eventList, aei) != SUCCESS)
    {
        asl_free(aei);
        asl_print_err("Add Event to List FAILED");
        eid = FAILURE;
    }
    asl_thread_mutex_unlock(&ae->listMutex);
    
    return eid;
}
result_t asf_event_post(struct asf_event *ae, s_int32_t eid)
{
    if(ae == NULL)
    {
        asl_print_err("ASF Event is NOT Initialized");
        return FAILURE;
    }
    struct asf_event_item *caei = NULL;
    struct asd_list_item *li = NULL;
    asl_thread_mutex_lock(&ae->listMutex);
    ASD_LIST_LOOP(ae->eventList, caei, li)
    {
        if(caei->eid == eid)
        {
            break;
        }
    }
    asl_thread_mutex_unlock(&ae->listMutex);
    if(caei == NULL)
    {
        asl_print_err("Event is NOT Initialized");
        return FAILURE;
    }
    struct asf_event_item *aei = (struct asf_event_item*)asl_malloc(sizeof(struct asf_event_item));
    aei->eid = eid;
    asl_thread_mutex_lock(&ae->fifoMutex);
    asd_fifo_enque(ae->eventFifo, aei);
    asl_thread_mutex_unlock(&ae->fifoMutex);
    asl_sem_post(&ae->esem);
    return SUCCESS;
}
s_int32_t asf_event_pend(struct asf_event *ae)
{
    if(ae == NULL)
    {
        asl_print_err("ASF Event is NOT Initialized");
        return FAILURE;
    }
    struct asf_event_item *aei;
    struct asf_event_item *caei = NULL;
    s_int32_t eid;
    asl_sem_wait(&ae->esem);
    asl_thread_mutex_lock(&ae->fifoMutex);
    aei = asd_fifo_deque(ae->eventFifo);
    asl_thread_mutex_unlock(&ae->fifoMutex);
    eid = aei->eid;
    asl_free(aei);
    asl_thread_mutex_lock(&ae->listMutex);
    struct asd_list_item *li = NULL;
    ASD_LIST_LOOP(ae->eventList, caei, li)
    {
        if(caei->eid == eid)
        {
            break;
        }
    }
    asl_thread_mutex_unlock(&ae->listMutex);
    if(caei == NULL)
    {
        asl_print_err("Event is NOT Initialized");
        eid = FAILURE;
    }
    
    return eid;
}

result_t asf_event_release_eid(struct asf_event *ae, s_int32_t eid)
{
    if(ae == NULL)
    {
        asl_print_err("ASF Event is NOT Initialized");
        return FAILURE;
    }
    result_t ret = FAILURE;
    struct asf_event_item aei;
    aei.eid = eid;
    asl_thread_mutex_lock(&ae->listMutex);
    ret = asd_list_del_item(ae->eventList, &aei);
    asl_thread_mutex_unlock(&ae->listMutex);
    return ret;
}