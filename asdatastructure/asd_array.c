#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asd_array.h"

#define ASD_ARRAY_REDUNDANCY 10
struct asd_array* asd_array_create(s_int32_t itemLen)
{
    struct asd_array *arr = ASL_MALLOC_T(struct asd_array);
    arr->max = 0;
    arr->len = itemLen;
    arr->count = 0;
    arr->start = NULL;

    return arr;
}
struct asd_array* asd_array_create_size(s_int32_t itemLen, u_int32_t size)
{
    struct asd_array *arr = ASL_MALLOC_T(struct asd_array);
    arr->max = size;
    arr->len = itemLen;
    arr->count = 0;
    arr->start = asl_malloc(size*itemLen);

    return arr;
}
result_t asd_array_put(struct asd_array *arr, void *item, u_int32_t index)
{
    if(arr == NULL || (index + 1) > arr->count)
        return FAILURE;
    asl_memcpy((char*)(arr->start) + index*(arr->len), item, arr->len);
    return SUCCESS;
}
result_t asd_array_get(struct asd_array *arr, u_int32_t index, void *target)
{
    if(arr == NULL || index >= arr->count || target == NULL)
        return FAILURE;
    asl_memcpy(target, (char*)(arr->start) + index*(arr->len), arr->len);
    return SUCCESS;
}
u_int32_t asd_array_get_value_index(struct asd_array *arr, void* target)
{
    if(arr->cfunc == NULL && arr->count == 0)
        return -1;
    u_int32_t index = 0;
    void *tmp = asl_malloc(arr->len);
    //while(asd_array_get(arr, index++, tmp) == SUCCESS && arr->cfunc(tmp, target) != 0);
    while(asd_array_get(arr, index, tmp) == SUCCESS)
    {
        //asl_print_dbg("0x%02x vs 0x%02x", *((char*)tmp), *((char*)target));
        if(arr->cfunc(tmp, target) == 0)
        {
            //asl_print_dbg("Get target");
            break;
        }
        index++;
    }
    asl_free(tmp);
    //asl_print_dbg("index:%d, arr->count:%d", index, arr->count);
    return (index == arr->count ? -1 : index);
}
result_t asd_array_append(struct asd_array *arr, void *items, u_int32_t count)
{
    if(arr == NULL)
        return FAILURE;
    if(arr->count + count >= arr->max)
    {
        arr->max = arr->count + count +  ASD_ARRAY_REDUNDANCY;
        arr->start = asl_realloc(arr->start, (arr->max)*(arr->len));
        asl_memset(arr->start + (arr->count)*(arr->len), 0, (arr->max - arr->count)*(arr->len));
    }
    asl_memcpy((char*)(arr->start) + (arr->count)*(arr->len), items, count*(arr->len));
    arr->count += count;
    return SUCCESS;
}
result_t asd_array_truncate(struct asd_array *arr, u_int32_t count)
{
    if(arr == NULL || count > arr->count)
        return FAILURE;
    asl_memset((char*)(arr->start) + (count)*(arr->len), 0, (arr->count - count)*(arr->len));
    arr->count = count;

    return SUCCESS;
}

result_t asd_array_last(struct asd_array *arr, u_int32_t count, void *target)
{
    if(arr == NULL || count > arr->count || target == NULL)
        return FAILURE;
    u_int32_t index = arr->count - count;
    asl_memcpy(target, (char*)(arr->start) + index*(arr->len), count*(arr->len));
    return SUCCESS;
}
result_t asd_array_clear(struct asd_array *arr)
{
    if(arr != NULL && arr->count > 0)
    {
        asl_memset(arr->start, 0, (arr->max)*(arr->len));
        arr->count = 0;
        return SUCCESS;
    }
    return FAILURE;
}
result_t asd_array_destroy(struct asd_array **arrp)
{
    if(arrp != NULL && *arrp != NULL)
    {
        struct asd_array *arr = *arrp;
        asl_free(arr->start);
        asl_free(arr);
        *arrp = NULL;
    }
    return SUCCESS;
}
