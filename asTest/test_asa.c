#include "asl_inc.h"
#include "asa_inc.h"

#define TEST_ASA_ITEM_COUNT 1000

typedef void (*sort_func)(s_int32_t *A, s_int32_t count);
enum TEST_ASA_METHOD
{
    TAM_BUBBLE,
    TAM_INSERTION,
    TAM_MERGE,
    TAM_QUICK
};

struct test_asa_container
{
    s_int32_t method;
    sort_func sortFunc;
    const char* desc;
}tac[] = 
{
    {TAM_BUBBLE,    bubble,     "Bubble"},
    {TAM_INSERTION, insertion,  "Insertion"},
    {TAM_MERGE,     merge,      "Merge"},
    {TAM_QUICK,     quick,      "Quick"}
};

static void test_asa_ret_output(const char* desc, s_int32_t *ret)
{
    s_int32_t i;
    s_int32_t itemCount = TEST_ASA_ITEM_COUNT > 20 ? 20 : TEST_ASA_ITEM_COUNT;
    asl_printf("%s (top %d):\n", desc, itemCount);
    for(i = 0; i < itemCount; i++)
    {
        asl_printf(" %d", ret[i]);
    }
    asl_println();
}

static void test_asa_driver(s_int32_t method, s_int32_t *randInput, s_int32_t *randInputNodup, s_int32_t *ret)
{
    struct test_asa_container* ctac = NULL;
    s_int32_t index;
    u_int64_t start, end;
    for(index = 0; index < ASL_COUNTOF(tac); index++)
    {
        if(method == tac[index].method)
        {
            ctac = &tac[index];
            break;
        }
    }
    if(ctac == NULL)
    {
        return;
    }
    asl_printf("%s Sort I:\n", ctac->desc);
    asl_memcpy(ret, randInput, TEST_ASA_ITEM_COUNT * sizeof(s_int32_t));
    test_asa_ret_output("Orignal List", ret);
    start = asl_time_get_stamp();
    (ctac->sortFunc)(ret, TEST_ASA_ITEM_COUNT);
    end = asl_time_get_stamp();
    test_asa_ret_output("Sorted List", ret);
    asl_printf("%s Sort %d item consume %lu seconds\n\n", ctac->desc, TEST_ASA_ITEM_COUNT, end - start);

    asl_printf("%s Sort II:\n", ctac->desc);
    asl_memcpy(ret, randInputNodup, TEST_ASA_ITEM_COUNT * sizeof(s_int32_t));
    test_asa_ret_output("Orignal List", ret);
    start = asl_time_get_stamp();
    (ctac->sortFunc)(ret, TEST_ASA_ITEM_COUNT);
    end = asl_time_get_stamp();
    test_asa_ret_output("Sorted List", ret);
    asl_printf("%s Sort %d no duplicated item consume %lu seconds\n\n", ctac->desc, TEST_ASA_ITEM_COUNT, end - start);
}

void test_asa()
{
    s_int32_t *randInput = (s_int32_t*)asl_malloc(TEST_ASA_ITEM_COUNT * sizeof(s_int32_t));
    s_int32_t *randInputNodup = (s_int32_t*)asl_malloc(TEST_ASA_ITEM_COUNT * sizeof(s_int32_t));
    s_int32_t *ret = (s_int32_t*)asl_malloc(TEST_ASA_ITEM_COUNT * sizeof(s_int32_t));;

    if(!randInput || !randInputNodup || !ret)
    {
        asl_print_err("Malloc For Test ASA FAILED");
        asl_free(randInput);
        asl_free(randInputNodup);
        asl_free(ret);
        return;
    }

    asl_rand_in_range(randInput, TEST_ASA_ITEM_COUNT, 1, TEST_ASA_ITEM_COUNT);
    asl_rand_in_range_no_dup(randInputNodup, TEST_ASA_ITEM_COUNT, 1, TEST_ASA_ITEM_COUNT);

    test_asa_driver(TAM_BUBBLE, randInput, randInputNodup, ret);
    test_asa_driver(TAM_INSERTION, randInput, randInputNodup, ret);
    test_asa_driver(TAM_MERGE, randInput, randInputNodup, ret);
    test_asa_driver(TAM_QUICK, randInput, randInputNodup, ret);

    return;
}
