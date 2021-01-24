#include "asl_inc.h"
s_int64_t asl_get_nsquare(s_int32_t base, s_int32_t times)
{
    return 0;
}
s_int32_t *asl_rand_in_range_no_dup(s_int32_t *target, s_int32_t count, s_int32_t min, s_int32_t max)
{
    if(target == NULL || max - min < 0 || count < 1 || max - min + 1 < count)
    {
        return NULL;
    }
    s_int32_t index;
    s_int32_t curCount = max - min;
    s_int32_t choice;
    s_int32_t tmp;
    if(max - min + 1 == count)
    {
        for(index = min; index < max + 1; index++)
        {
            target[index - min] = index;
        }
        for(index = 0; index < count; index++)
        {
            choice = rand()%(curCount + 1);
            //asl_print_dbg("No.%d Get choice %d from %d items", index, choice, curCount);
            tmp = target[choice];
            target[choice] = target[curCount];
            target[curCount] = tmp;
            
            curCount--;
        }
    }
    else
    {
        s_int32_t *src = (s_int32_t*)asl_malloc((max - min + 1) * sizeof(s_int32_t));
        for(index = min; index < max + 1; index++)
        {
            src[index - min] = index;
        }
        
        for(index = 0; index < count; index++)
        {
            choice = rand()%(curCount + 1);
            //asl_print_dbg("No.%d Get choice %d from %d items", index, choice, curCount);
            tmp = src[choice];
            target[index] = tmp;
            src[choice] = src[curCount];
            curCount--;
        }
        asl_free(src);
    }
    return target;
}
s_int32_t *asl_rand_in_range(s_int32_t *target, s_int32_t count, s_int32_t min, s_int32_t max)
{
    if(target == NULL || max - min < 0 || count < 1)
    {
        return NULL;
    }
    s_int32_t index;
    for(index = 0; index < count; index++)
    {
        target[index] = min + rand()%(max - min - 1);
    }
    return target;
}

