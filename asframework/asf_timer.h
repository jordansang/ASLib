#ifndef ASF_TIMER_H
#define ASF_TIMER_H

#include "asd_list.h"

typedef void*(*asf_time_func)(void*);

#define ASF_TIMER_INITIALIZED (TRUE)
struct asf_timer
{
    s_int32_t id;
    s_int32_t state;
    struct asf_event *event;
    s_int32_t onTimeEid;
    s_int32_t pendEid;
    s_int32_t delEid;
    
    /* Main timer loop thread */
    struct asl_thread mainThr;
    /* Wait for the latest timer */
    struct asl_thread onTimeThr;
    
    /* mutex for timer asd_list */
    struct asl_thread_mutex tlmutex;
    /* Current timer queue */
    struct asd_list *timerList;
    
    /* mutex for pend asd_list */
    struct asl_thread_mutex plmutex;
    /* Wait for entering to the timer queue */
    struct asd_list *pendList;
    
    /* mutex for delete asd_list */
    struct asl_thread_mutex dlmutex;
    /* Wait for deleting from the timer queue */
    struct asd_list *delList;
};


struct asf_timer_item
{
    struct asf_timer* at;
    s_int32_t id;
    asl_time_val start;
    asl_time_val interval;
    asl_time_val repeat;
    asf_time_func func;
    void* arg;
};



struct asf_timer* asf_timer_create();
s_int32_t asf_timer_start(struct asf_timer *at, asl_time_val interval, asl_time_val repeat, void* func, void *arg);
result_t asf_timer_cancel(struct asf_timer *at, s_int32_t tid);
result_t asf_timer_destroy(struct asf_timer *at);


#endif /* ASF_TIMER_H */
