#include "asl_inc.h"
#include "asf_mem_manager.h"

enum TEST_MTYPE
{
    TEST_MTYPE_MAIN,
    TEST_MTYPE_TEST,
    TEST_MTYPE_RUNNING,
    TEST_MTYPE_TMP,
    TEST_MTYPE_ERR,
    TEST_MTYPE_SYSTEM
};

struct asf_memg_minfo_item mItems[] = 
{
    {TEST_MTYPE_MAIN, "Main"},
    {TEST_MTYPE_TEST, "Test"},
    {TEST_MTYPE_RUNNING, "Running"},
    {TEST_MTYPE_TMP, "Temp"},
    {TEST_MTYPE_ERR, "Error"},
    {TEST_MTYPE_SYSTEM, "System"}
};

struct test_memg_container
{
    u_int32_t type;
    u_int32_t count;
    char** ptrs;
};

#define TEST_MIN_BLOCK_COUNT 10

#define TEST_MSIZE_COUNT 10
#define TEST_MSIZE_POWER 6
void test_memg()
{
    /* Memory Type Index */
    u_int32_t mIndex;
    /* Memory Size Index */
    u_int32_t sIndex;
    /* Pointer Index */
    u_int32_t pIndex;
    result_t ret = FAILURE;
    struct test_memg_container *ptrContainer = NULL;
    char** tmpPtr;
    u_int32_t *mSize;
    struct asf_memg_minfo mInfo;

    /* Initialize Memory Size Types */
    mSize = (u_int32_t*)asl_malloc(TEST_MSIZE_COUNT * (sizeof(u_int32_t)));
    for(mIndex = 0; mIndex < TEST_MSIZE_COUNT; mIndex++)
    {
        mSize[mIndex] = (1 << (mIndex + TEST_MSIZE_POWER));
    }

    /* Initialize Pointer Container for every Memory Type */
    ptrContainer = (struct test_memg_container*)asl_malloc(ASL_COUNTOF(mItems) * (sizeof(struct test_memg_container)));
    for(mIndex = 0; mIndex < ASL_COUNTOF(mItems); mIndex++)
    {
        ptrContainer[mIndex].type = mItems[mIndex].type;
        ptrContainer[mIndex].count = (mIndex + TEST_MIN_BLOCK_COUNT);
        ptrContainer[mIndex].ptrs = (char**)asl_malloc(TEST_MSIZE_COUNT * (mIndex + TEST_MIN_BLOCK_COUNT) * (sizeof(char*)));
    }

    /* Initialize Memory Manager */
    mInfo.count = ASL_COUNTOF(mItems);
    mInfo.items = mItems;

    asl_print_dbg("Test of ASF Memory Manager START");

    ret = asf_memg_create(&mInfo);
    asl_print_dbg("Initialize Memory Manager %s", asl_get_ret_str(ret));

    asl_print_dbg("Test of ASF Memory Manager Allocate Half Count");

    /* Allocate Half Count in every Memory type */
    for(mIndex = 0; mIndex < ASL_COUNTOF(mItems); mIndex++)
    {
        tmpPtr = (ptrContainer[mIndex].ptrs);
        for(sIndex = 0; sIndex < TEST_MSIZE_COUNT; sIndex++)
        {
            for(pIndex = 0; pIndex < ptrContainer[mIndex].count/2; pIndex++)
            {
                *(tmpPtr) = (char*)asf_memg_alloc(mItems[mIndex].type, mSize[sIndex]);
                tmpPtr++;
            }
        }
    }

    //asf_memg_dump(ASF_MEMG_OPE_TYPE);
    //asf_memg_dump(ASF_MEMG_OPE_SIZE);
    sleep(3);

    asl_print_dbg("Test of ASF Memory Manager Free Half Count");

    /* Free the Memory Blocks Allocated */
    for(mIndex = 0; mIndex < ASL_COUNTOF(mItems); mIndex++)
    {
        tmpPtr = ptrContainer[mIndex].ptrs;
        for(sIndex = 0; sIndex < TEST_MSIZE_COUNT; sIndex++)
        {
            for(pIndex = 0; pIndex < ptrContainer[mIndex].count/2; pIndex++)
            {
                asf_memg_free(*(tmpPtr));
                tmpPtr++;
            }
        }
    }

    //asf_memg_dump(ASF_MEMG_OPE_TYPE);
    //asf_memg_dump(ASF_MEMG_OPE_SIZE);
    sleep(3);

    asl_print_dbg("Test of ASF Memory Manager Allocate All");

    /* Allocate All in every Memory type */
    for(mIndex = 0; mIndex < ASL_COUNTOF(mItems); mIndex++)
    {
        tmpPtr = (ptrContainer[mIndex].ptrs);
        for(sIndex = 0; sIndex < TEST_MSIZE_COUNT; sIndex++)
        {
            for(pIndex = 0; pIndex < ptrContainer[mIndex].count; pIndex++)
            {
                *(tmpPtr) = (char*)asf_memg_alloc(mItems[mIndex].type, mSize[sIndex]);
                tmpPtr++;
            }
        }
    }

    //asf_memg_dump(ASF_MEMG_OPE_TYPE);
    //asf_memg_dump(ASF_MEMG_OPE_SIZE);
    sleep(3);

    asl_print_dbg("Test of ASF Memory Manager Free All");

    /* Free the Memory Blocks Allocated */
    for(mIndex = 0; mIndex < ASL_COUNTOF(mItems); mIndex++)
    {
        tmpPtr = ptrContainer[mIndex].ptrs;
        for(sIndex = 0; sIndex < TEST_MSIZE_COUNT; sIndex++)
        {
            for(pIndex = 0; pIndex < ptrContainer[mIndex].count; pIndex++)
            {
                asf_memg_free(*(tmpPtr));
                tmpPtr++;
            }
        }
    }

    asf_memg_dump(ASF_MEMG_OPE_TYPE);
    asf_memg_dump(ASF_MEMG_OPE_SIZE);

    asl_print_dbg("Test of ASF Memory Manager FINISH");

    return;
}
