/* Created By JS. Contact with <sanghe@163.com> */

/*************************************************************************
Filename: asl_time.c

File Description: 
    Basal time functions.

Created by: JS

Create Date: 2018/04/03

Modified by:

Modified Date:

Modification Description:

*************************************************************************/
#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asl_time.h"

/************************************************************************* 
Function name: asl_time_get_stamp

Function Description:
    Generate the time stamp.

Parameters:
    
Return:
    time stamp

*************************************************************************/
u_int64_t asl_time_get_stamp()
{
    asl_time_t now;
    asl_time(&now);
    return ((u_int64_t)now);
}
char* asl_time_get_ctime()
{
    asl_time_t now;
    asl_time(&now);
    return ctime(&now);
}
asl_time_val* asl_time_get_val(asl_time_val *val)
{
    if(val == NULL)
    {
        return NULL;
    }
    struct timeval *tv = (struct timeval*)val;
    if(gettimeofday(tv, NULL) != 0)
    {
        asl_memset(val, 0, sizeof(asl_time_val));
        return NULL;
    }
    return val;
}
s_int8_t asl_time_cmp(const asl_time_val *a, const asl_time_val *b)
{
    if(!a || !b)
    {
        return -1;
    }
    const struct timeval *tva = (const struct timeval*)a;
    const struct timeval *tvb = (const struct timeval*)b;
    s_int8_t ret = 0;
    if(timercmp(tva, tvb, >) != 0)
    {
        ret = 1;
    }
    else if(timercmp(tva, tvb, <) != 0)
    {
        ret = -1;
    }
    return ret;
}
asl_time_val* asl_time_add(const asl_time_val *a, const asl_time_val *b, asl_time_val *sum)
{
    if(!a || !b || !sum)
    {
        return NULL;
    }
    const struct timeval *tva = (const struct timeval*)a;
    const struct timeval *tvb = (const struct timeval*)b;
    struct timeval *res = (struct timeval*)sum;
    timeradd(tva, tvb, res);
    return sum;
}
asl_time_val* asl_time_sub(const asl_time_val *a, const asl_time_val *b, asl_time_val *sub)
{
    if(!a || !b || !sub)
    {
        return NULL;
    }
    const struct timeval *tva = (const struct timeval*)a;
    const struct timeval *tvb = (const struct timeval*)b;
    struct timeval *res = (struct timeval*)sub;
    timersub(tva, tvb, res);
    return sub;
}
void asl_time_dump(asl_time_val *val, const char* name)
{
    if(val == NULL)
    {
        asl_print_dbg("No data in Time Value");
        return;
    }
    asl_printf("%s Time Value: Sec:%lu uSec:%u\n", (name == NULL ? "ASF" : name), val->sec, val->usec);
}
