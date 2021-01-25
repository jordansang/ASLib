#include "asl_inc.h"

/*
asl_time_val* asl_time_get_val(asl_time_val *val);
s_int8_t asl_time_cmp(const asl_time_val *a, const asl_time_val *b);
asl_time_val* asl_time_add(const asl_time_val *a, const asl_time_val *b, asl_time_val *sum);
asl_time_val* asl_time_sub(const asl_time_val *a, const asl_time_val *b, asl_time_val *sub);

#define asl_time(T) time(T)
u_int64_t asl_time_get_stamp();
void asl_time_dump(asl_time_val *val, const char* name);
*/
void test_time()
{
    asl_time_val a,b,sum,sub,tmp,cur;
    if(asl_time_get_val(&cur) != NULL)
    {
        asl_time_dump(&cur, "Current");
    }
    else
    {
        asl_print_err("Get Current time FAILED");
    }
    a.sec = 1;
    a.usec = 999991;
    b.sec = 1;
    b.usec = 1000;
    if(asl_time_add(&a, &b, &sum) != NULL)
    {
        asl_time_dump(&sum, "Sum a and b");
    }
    else
    {
        asl_print_err("Get summary of a and b FAILED");
    }
    if(asl_time_sub(&sum, &a, &sub) != NULL)
    {
        asl_time_dump(&sub, "Sub sum and a");
    }
    else
    {
        asl_print_err("Get sub of a and b FAILED");
    }
    asl_print_dbg("ASL Time:%lu", asl_time(NULL));
    asl_print_dbg("ASL Time stamp:%lu", asl_time_get_stamp());
    asl_print_dbg("a cmp b:%d", asl_time_cmp(&a, &b));
    asl_print_dbg("b cmp a:%d", asl_time_cmp(&b, &a));
    a.sec = 1;
    a.usec = 1000;
    asl_print_dbg("a cmp b:%d", asl_time_cmp(&a, &b));
    asl_print_dbg("b cmp a:%d", asl_time_cmp(&b, &a));
    return;
}
