#include "asl_inc.h"
#include "asf_timer.h"

/*
** A. Basic Test
**      1. Create Timer
**      2. Add 1 second Timer repeat 3 second, wait them running, delete it
**      3. Add 5,3,1 second Timer, wait them running
**      4. Add 7,5,3,1 second Timer, delete 3 second timer, wait 1 running ,delete 5, wait 7
**      5. Add 7,5,3,1 second Timer, delete timer
** 
** B. Random Test
**      1. Create Timer
**      2. Random choose add/delete timer in random time;
**      3. Destroy timer
*/
#define TEST_TIMER_MODE_A 0
#define TEST_TIMER_MODE_B 1

#define TEST_TIMER_COUNT 1000

struct test_timer_arg
{
    char desc[32];
    asl_time_val start;
    asl_time_val interval;
    asl_time_val repeat;
    asl_time_val end;
};

void* test_timer_func(void *arg)
{
    struct test_timer_arg *tta = (struct test_timer_arg*)arg;
    asl_time_get_val(&tta->end);
    asl_print_dbg("Timer \"%s\" Called: Start:%lu:%u Interval:%lu:%u Repeat:%lu:%u End:%lu:%u", 
                    tta->desc, tta->start.sec, tta->start.usec, 
                    tta->interval.sec, tta->interval.usec, 
                    tta->repeat.sec, tta->repeat.usec,
                    tta->end.sec, tta->end.usec);
    return NULL;
}
void test_timer_set(struct test_timer_arg *arg, s_int32_t isec, s_int32_t rsec)
{
    asl_time_get_val(&arg->start);
    sprintf(arg->desc, "i%dr%d", isec, rsec);
    arg->interval.sec = (asl_time_t)isec;
    arg->interval.usec = 0;
    arg->repeat.sec = (asl_time_t)rsec;
    arg->repeat.usec = 0;
}
s_int32_t test_timer_start(struct asf_timer *at, struct test_timer_arg *arg)
{
    s_int32_t tid = asf_timer_start(at, arg->interval, arg->repeat, test_timer_func, arg);
    asl_print_dbg("Start Timer \"%s\" with Tid:%d", arg->desc, tid);
    return tid;
}
void test_timer_stop(struct asf_timer *at, struct test_timer_arg *arg, s_int32_t tid)
{
    asf_timer_cancel(at, tid);
    asl_print_dbg("Cancel Timer \"%s\" with Tid:%d", arg->desc,  tid);
}
void test_timer()
{
    s_int32_t index;
    struct asf_timer *at = NULL;
    struct test_timer_arg tta[10] = {0};
    s_int32_t tid[10]= {0};
    
    if(TEST_TIMER_MODE_A)
    {
        asl_print_dbg("Timer Test Mode A START");
        at = asf_timer_create();
        if(at == NULL)
        {
            asl_print_err("Create Timer FAILED");
            return;
        }
        test_timer_set(&tta[0], 1, 3);
        tid[0] = test_timer_start(at, &tta[0]);
        
        sleep(5);
        test_timer_stop(at, &tta[0], tid[0]);
        sleep(5);

        test_timer_set(&tta[0], 5, 0);
        tid[0] = test_timer_start(at, &tta[0]);

        test_timer_set(&tta[1], 3, 0);
        tid[1] = test_timer_start(at, &tta[1]);

        test_timer_set(&tta[2], 1, 0);
        tid[2] = test_timer_start(at, &tta[2]);

        sleep(6);

        test_timer_set(&tta[0], 7, 0);
        tid[0] = test_timer_start(at, &tta[0]);

        test_timer_set(&tta[1], 5, 0);
        tid[1] = test_timer_start(at, &tta[1]);

        test_timer_set(&tta[2], 3, 0);
        tid[2] = test_timer_start(at, &tta[2]);

        test_timer_set(&tta[3], 1, 0);
        tid[3] = test_timer_start(at, &tta[3]);

        test_timer_stop(at, &tta[2], tid[2]);
        sleep(2);

        test_timer_stop(at, &tta[1], tid[1]);
        sleep(6);

        test_timer_set(&tta[0], 7, 0);
        tid[0] = test_timer_start(at, &tta[0]);

        test_timer_set(&tta[1], 5, 0);
        tid[1] = test_timer_start(at, &tta[2]);

        test_timer_set(&tta[2], 3, 0);
        tid[2] = test_timer_start(at, &tta[2]);

        test_timer_set(&tta[3], 1, 0);
        tid[3] = test_timer_start(at, &tta[3]);

        asf_timer_destroy(at);
        asl_print_dbg("Timer Test Mode A END");
    }

    if(TEST_TIMER_MODE_B)
    {
        asl_time_val cur;
        asl_print_dbg("Timer Test Mode B START");
        s_int32_t opeCount = TEST_TIMER_COUNT;
        s_int32_t tid[TEST_TIMER_COUNT] = {0};
        struct test_timer_arg tta[TEST_TIMER_COUNT] = {0};
        s_int32_t tidCount = 0;
        s_int32_t i;
        asl_time_t isec, rsec;
        at = asf_timer_create();
        if(at == NULL)
        {
            asl_print_err("Create Timer FAILED");
            return;
        }
        while(opeCount--)
        {
            //sleep(1+ rand()%3);
            if((1+rand()%2) == 1)
            {
                /* Add a new Timer */
                i = TEST_TIMER_COUNT - opeCount + 1;
                isec = 1 + rand()%9;
                rsec = rand()%2;
                test_timer_set(&tta[i], isec, rsec);
                tid[i] = test_timer_start(at, &tta[i]);
            }
            else
            {
                for(index = TEST_TIMER_COUNT; index > 0; index--)
                {
                    asl_time_get_val(&cur);
                    if(tid[index - 1] != 0 
                        && ((cur.sec - tta[index - 1].start.sec < tta[index - 1].interval.sec) 
                            || tta[index - 1].repeat.sec != 0))
                    {
                        test_timer_stop(at, &tta[index - 1], tid[index - 1]);
                        tid[index - 1] = 0;
                        break;
                    }
                }
            }
        }
        sleep(100);
        asf_timer_destroy(at);
        asl_print_dbg("Timer Test Mode B END");
    }
    
}
