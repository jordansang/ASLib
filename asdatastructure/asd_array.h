#ifndef ASD_ARRAY_H
#define ASD_ARRAY_H

#include "asl_stdio.h"

struct asd_array
{
    u_int32_t max;
    u_int32_t len;
    u_int32_t count;
    void* start;

    cmp_func cfunc;
};

struct asd_array* asd_array_create(s_int32_t itemLen);
struct asd_array* asd_array_create_size(s_int32_t itemLen, u_int32_t size);
result_t asd_array_put(struct asd_array *arr, void *item, u_int32_t index);
result_t asd_array_get(struct asd_array *arr, u_int32_t index, void *target);
u_int32_t asd_array_get_value_index(struct asd_array *arr, void* target);
result_t asd_array_append(struct asd_array *arr, void *items, u_int32_t count);
result_t asd_array_truncate(struct asd_array *arr, u_int32_t count);
result_t asd_array_last(struct asd_array *arr, u_int32_t count, void *target);
result_t asd_array_clear(struct asd_array *arr);
result_t asd_array_destroy(struct asd_array **arrp);

#define ASD_ARRAY_COUNT(arr) (arr->count)
#define ASD_ARRAY_DATA(arr) (arr->start)
#endif /*ASD_ARRAY_H */
