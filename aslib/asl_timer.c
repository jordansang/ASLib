#include "asl_stdio.h"
#include "asl_timer.h"

result_t asl_timer_create(struct asl_timer *timer)
{
    if(timer != NULL && (timer->id = timerfd_create(CLOCK_REALTIME, 0)) > 0)
    {
        return SUCCESS;
    }
    asl_print_err("Create debug timer fd failed, errno:%d,%s", errno, strerror(errno));
    return FAILURE;
}
result_t asl_timer_destroy(struct asl_timer *timer)
{
    if(timer != NULL && timer->id > 0)
    {
        struct itimerspec new_value;
        struct timespec now;
        if(clock_gettime(CLOCK_REALTIME, &now) > 0)
        {
            new_value.it_value.tv_sec = 0;
            new_value.it_value.tv_nsec = 0;
            new_value.it_interval.tv_sec = 0;
            new_value.it_interval.tv_nsec = 0;
            if(timerfd_settime(timer->id, TFD_TIMER_ABSTIME, &new_value, NULL) < 0)
            {
                asl_print_err("Destroy timerfd(%d) start failed", timer->id);
                return FAILURE;
            }
            close(timer->id);
        }
    }
    return SUCCESS;
}
result_t asl_timer_set(struct asl_timer *timer)
{
    if(timer != NULL && timer->id > 0)
    {
        struct itimerspec new_value;
        struct timespec now;
        if(clock_gettime(CLOCK_REALTIME, &now) == 0)
        {
            new_value.it_value.tv_sec = now.tv_sec + timer->init;
            new_value.it_value.tv_nsec = now.tv_nsec;
            new_value.it_interval.tv_sec = timer->repeat;
            new_value.it_interval.tv_nsec = 0;
            if(timerfd_settime(timer->id, TFD_TIMER_ABSTIME, &new_value, NULL) < 0)
            {
                asl_print_err("Set timerfd(%d) start failed", timer->id);
                return FAILURE;
            }
            return SUCCESS;
        }
    }
    return FAILURE;
}