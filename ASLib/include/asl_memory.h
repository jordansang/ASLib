#ifndef ASL_MEMORY_H
#define ASL_MEMORY_H

#include <stdlib.h>

void* asl_malloc(int size);
result_t asl_free(void* p);
void* asl_realloc(void* p, int size);
result_t asl_memcpy(void* dst, const void* src, u_int32_t size);
result_t asl_memset(void* p, u_int32_t val, u_int32_t size);

#define ASL_MALLOC_T(TYPE) (TYPE*)(asl_malloc(sizeof(TYPE)))

#endif /* ASL_MEMORY_H */
