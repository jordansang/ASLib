#include "asl_inc.h"
#include "asf_mem_pool.h"


#define USE_ASF 1

#ifdef USE_ASF
#define TEST_MALLOC(SIZE) asf_memp_alloc(SIZE)
#define TEST_FREE(PTR) asf_memp_free(PTR)
#define TEST_DUMP()   asf_memp_dump()
//#define TEST_DUMP()
#else
#define TEST_MALLOC(SIZE) asl_malloc(SIZE)
#define TEST_FREE(PTR) asl_free(PTR)
#define TEST_DUMP()
#endif

#define TEST_COUNT 1
#define PRE_ALLOC_COUNT 100
#define NEED_COUNT 100
#define TBL_COUNT 10
#define POWER_ON_BLOCK 6

u_int64_t test_get_time_interval(struct timespec *before, struct timespec* after)
{
    u_int64_t interval = 0;
    #if 1
    u_int64_t secInterval = after->tv_sec - before->tv_sec;
    u_int64_t nsecInterval = secInterval*1000000000 + after->tv_nsec - before->tv_nsec;

    interval = nsecInterval;

    //asl_print_dbg("secInter:%lu %lu inter:%lu", secInterval, secInterval*1000000000, interval);
    #else
    u_int64_t power = 1000000000;
    if(after->tv_sec != before->tv_sec)
    {
        asl_print_dbg("start:%lu:%lu end:%lu:%lu", 
                before->tv_sec, before->tv_nsec, after->tv_sec, after->tv_nsec);
        interval = (after->tv_sec - before->tv_sec)*power + after->tv_nsec - before->tv_nsec;
        asl_print_dbg("sec:%lu %lu nsec:%lu ret:%lu", 
                (after->tv_sec - before->tv_sec),
                (after->tv_sec - before->tv_sec)*1000000000, 
                (after->tv_sec - before->tv_sec)*1000000000 + after->tv_nsec,
                interval);
        
    }
    else
    {
        interval = (after->tv_nsec - before->tv_nsec);
    }
    #endif
    return interval;
}

void test_memp()
{
    result_t ret = FAILURE;
    u_int32_t index;
    u_int32_t ptrIndex;
    u_int32_t size[3];
    char* ptr[TBL_COUNT][NEED_COUNT] = {0};
    struct timespec beforeTs;
    struct timespec afterTs;
    struct timespec tmpTs;
    u_int64_t interval = 0;
    int count = TEST_COUNT;
#ifdef USE_ASF
    struct asf_memp_tbl_container amtc;
    amtc.count = TBL_COUNT;
    struct asf_memp_tbl tbls[amtc.count];
    
    for(index = 0; index < TBL_COUNT; index++)
    {
        tbls[index].blkSize = (1 << (index + POWER_ON_BLOCK));
        tbls[index].free = PRE_ALLOC_COUNT;
    }
    amtc.tbls = tbls;
    //ret = asf_memp_create(&amtc);
    //asl_print_dbg("Create Memory Pool Items %s", asl_get_ret_str(ret));
#endif
    
    clock_gettime(CLOCK_REALTIME, &beforeTs);
    while(count--)
    {
        for(index = 0; index < TBL_COUNT; index++)
        {
            for(ptrIndex = 0; ptrIndex < NEED_COUNT; ptrIndex++)
            {
                size[2] = (1 << (index + POWER_ON_BLOCK));
                size[0] = (1 << (index + POWER_ON_BLOCK - 1)) + 1;
                size[1] = ((size[0] + size[2]) / 2);
                ptr[index][ptrIndex] = TEST_MALLOC(size[1]);
            }
        }
        TEST_DUMP();
        
        for(index = 0; index < TBL_COUNT; index++)
        {
            for(ptrIndex = 0; ptrIndex < NEED_COUNT; ptrIndex++)
            {
                ret = TEST_FREE(ptr[index][ptrIndex]);
                if(ret != SUCCESS)
                {
                    asl_print_dbg("free ptr[%d][%d] FAILED", index, ptrIndex);
                }
            }
        }
        TEST_DUMP();
        for(index = 0; index < TBL_COUNT; index++)
        {
            for(ptrIndex = 0; ptrIndex < NEED_COUNT; ptrIndex++)
            {
                size[2] = (1 << (index + POWER_ON_BLOCK));
                size[0] = (1 << (index + POWER_ON_BLOCK - 1)) + 1;
                size[1] = ((size[0] + size[2]) / 2);
                ptr[index][ptrIndex] = TEST_MALLOC(size[1]);
            }
        }
        TEST_DUMP();
        
        for(index = 0; index < TBL_COUNT; index++)
        {
            for(ptrIndex = 0; ptrIndex < NEED_COUNT; ptrIndex++)
            {
                ret = TEST_FREE(ptr[index][ptrIndex]);
                if(ret != SUCCESS)
                {
                    asl_print_dbg("free ptr[%d][%d] FAILED", index, ptrIndex);
                }
            }
        }
        TEST_DUMP();
        
        for(index = 0; index < TBL_COUNT; index++)
        {
            for(ptrIndex = 0; ptrIndex < NEED_COUNT; ptrIndex++)
            {
                size[2] = (1 << (index + POWER_ON_BLOCK));
                size[0] = (1 << (index + POWER_ON_BLOCK - 1)) + 1;
                size[1] = ((size[0] + size[2]) / 2);
                ptr[index][ptrIndex] = TEST_MALLOC(size[1]);
            }
        }
        TEST_DUMP();
        
#ifdef USE_ASF
        asf_memp_destroy();
        //asf_memp_dump();
#else
        for(index = 0; index < TBL_COUNT; index++)
        {
            for(ptrIndex = 0; ptrIndex < NEED_COUNT; ptrIndex++)
            {
                ret = TEST_FREE(ptr[index][ptrIndex]);
                if(ret != SUCCESS)
                {
                    asl_print_dbg("free ptr[%d][%d] FAILED", index, ptrIndex);
                }
            }
        }
#endif
    }
    clock_gettime(CLOCK_REALTIME, &afterTs);
    //asl_print_dbg("start:%lu:%lu end:%lu:%lu", beforeTs.tv_sec, beforeTs.tv_nsec, afterTs.tv_sec, afterTs.tv_nsec);
    asl_printf("\n\n%d times average:%lu ns\n", TEST_COUNT, test_get_time_interval(&beforeTs, &afterTs)/TEST_COUNT);
    return;
}
