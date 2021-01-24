#include "asl_type.h"
#include "asl_stdio.h"
#include "asl_memory.h"

void* asl_malloc(int size)
{
    void *buf = NULL;
    if(size > 0)
    {
        if((buf = (void*)malloc(size)) != NULL)
        {
            asl_memset(buf, 0, size);
        }
    }
    return buf;
}
result_t asl_free(void* p)
{
    if(p != NULL)
    {
        free(p);
    }
    return SUCCESS;
}
void* asl_realloc(void* p, int size)
{
    return (p = realloc(p, size));
}
result_t asl_memcpy(void* dst, const void* src, u_int32_t size)
{
    if(dst == NULL || src == NULL)
    {
        asl_print_err("Can't copy NULL ptr");
        return FAILURE;
    }
    if(size != 0 && memcpy(dst, src, size) != dst)
    {
        return FAILURE;
    }
    return SUCCESS;
}
result_t asl_memset(void* p, u_int32_t val, u_int32_t size)
{
    if(p == NULL || size == 0)
    {
        asl_print_err("Can't set NULL ptr");
        return FAILURE;
    }
    if(memset(p, val, size) != p)
    {
        return FAILURE;
    }
    return SUCCESS;
}
