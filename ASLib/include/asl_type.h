#ifndef ASL_TYPE_H
#define ASL_TYPE_H

typedef signed char s_int8_t;
typedef signed short s_int16_t;
typedef signed int s_int32_t;
typedef signed long long s_int64_t;

// #ifndef _SYS_TYPES_H
// typedef unsigned char u_int8_t;
// typedef unsigned short u_int16_t;
// typedef unsigned int u_int32_t;
// typedef unsigned long long u_int64_t;
// #endif

typedef s_int32_t result_t;
#define SUCCESS 1
#define FAILURE (-1)

typedef s_int32_t bool;
#define TRUE 1
#define FALSE 0

#define ENABLE 1
#define DISABLE 0

typedef s_int32_t(*cmp_func)(void *a, void *b);
typedef void(*del_func)(void *val);
typedef void(*ope_func)(void *val);
typedef void(*dump_func)(void *val);
typedef void(*swap_func)(void *a, void *b);

#endif /* ASL_TYPE_H */
