#ifndef ASF_THREAD_POOL_H
#define ASF_THREAD_POOL_H

#include "asl_thread.h"
#include "asd_list.h"

#define ASF_TP_INIT_COUNT 5
/* If idle/total is bigger than ASF_TP_IDLE_PERCENT, it maybe need to release */
#define ASF_TP_IDLE_PERCENT 0.6
/* Idle is too much for ASF_TP_IDLE_NEED_CLEAN times */
#define ASF_TP_IDLE_NEED_CLEAN 5
/* Release ASF_TP_IDLE_CLEAN_PERCENT of total to System */
#define ASF_TP_IDLE_CLEAN_PERCENT 0.5
/* Check idle/total percent per 1 second */
#define ASF_TP_MAINTAIN_INTERVAL 1

typedef void(*ASF_TP_ROUTINE)(void* arg);

struct asf_tp
{
    u_int8_t collectState;
    struct asl_thread maintainThr;
    struct asl_sem atSem;
    struct asd_list *idleList;
    struct asd_list *runningList;
    u_int32_t total;
    u_int32_t running;
    u_int32_t idle;

    u_int32_t idleLast;
};

struct asf_tp* asf_tp_create();
void asf_tp_start_routine(struct asf_tp *at, ASF_TP_ROUTINE func, void* arg);
void asf_tp_destroy(struct asf_tp **atp);
result_t asf_tp_enable_collect(struct asf_tp *at);
result_t asf_tp_disable_collect(struct asf_tp *at);

#endif /* ASF_THREAD_POOL_H */
