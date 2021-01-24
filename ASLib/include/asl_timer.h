#ifndef ASL_TIMER_H
#define ASL_TIMER_H

#include <sys/timerfd.h>


struct asl_timer
{
    /* timer fd */
    s_int32_t id;
    /* first end time */
    u_int32_t init;
    /* repeat time */
    u_int32_t repeat;
};

result_t asl_timer_create(struct asl_timer *timer);
result_t asl_timer_destroy(struct asl_timer *timer);
result_t asl_timer_set(struct asl_timer *timer);

#endif /* ASL_TIMER_H */
