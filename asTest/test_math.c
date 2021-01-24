#include "asl_inc.h"

#define TEST_RAND_MAX 100000000

void test_math()
{
    s_int32_t i;
    s_int32_t *ra = (s_int32_t*)asl_malloc(TEST_RAND_MAX*(sizeof(s_int32_t)));
    if(ra == NULL)
    {
        asl_print_err("Can't Malloc for ra");
        return;
    }
    u_int64_t start = asl_time_get_stamp();
    asl_rand_in_range_no_dup(ra, TEST_RAND_MAX, 1, TEST_RAND_MAX);
    u_int64_t end = asl_time_get_stamp();
    asl_print_dbg("Generate %d no duplicated item consume %llu seconds", TEST_RAND_MAX, end - start);
    s_int32_t itemCount = TEST_RAND_MAX > 20 ? 20 : TEST_RAND_MAX;
    asl_printf("No Duplicated Random Array (top %d):\n", itemCount);
    for(i = 0; i < itemCount; i++)
    {
        asl_printf(" %d", ra[i]);
    }
    asl_println();

    start = asl_time_get_stamp();
    asl_rand_in_range(ra, TEST_RAND_MAX, 1, TEST_RAND_MAX);
    end = asl_time_get_stamp();
    asl_print_dbg("Generate %d item consume %llu seconds", TEST_RAND_MAX, end - start);
    asl_printf("Random Array (top %d):\n", itemCount);
    for(i = 0; i < itemCount; i++)
    {
        asl_printf(" %d", ra[i]);
    }
    asl_println();

    asl_free(ra);
}
