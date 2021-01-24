#ifndef ASL_MATH_H
#define ASL_MATH_H

#define asl_get_arithmetic_progression(start, end, count) (((start + end)*count)/2)


s_int64_t asl_get_nsquare(s_int32_t base, s_int32_t times);
s_int32_t *asl_rand_in_range_no_dup(s_int32_t *target, s_int32_t count, s_int32_t min, s_int32_t max);
s_int32_t *asl_rand_in_range(s_int32_t *target, s_int32_t count, s_int32_t min, s_int32_t max);

#endif /* ASL_MATH_H */
