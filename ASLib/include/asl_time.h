#ifndef ASL_TIME_H
#define ASL_TIME_H

#include <sys/time.h>
#include <time.h>
typedef time_t asl_time_t;

/*
struct timeval {
   time_t      tv_sec;     //seconds
   suseconds_t tv_usec;    //microseconds
};
*/
typedef struct 
{
    asl_time_t sec;
    u_int32_t usec;
}asl_time_val;
/*
struct timespec {
   time_t tv_sec;        //seconds
   long   tv_nsec;       //nanoseconds
};
*/
typedef struct
{
    asl_time_t sec;
    u_int32_t nsec;
}asl_time_spec;

asl_time_val* asl_time_get_val(asl_time_val *val);
char* asl_time_get_ctime();
s_int8_t asl_time_cmp(const asl_time_val *a, const asl_time_val *b);
asl_time_val* asl_time_add(const asl_time_val *a, const asl_time_val *b, asl_time_val *sum);
asl_time_val* asl_time_sub(const asl_time_val *a, const asl_time_val *b, asl_time_val *sub);

#define asl_time(T) time(T)
u_int64_t asl_time_get_stamp();
void asl_time_dump(asl_time_val *val, const char* name);


#endif /* ASL_TIME_H */
