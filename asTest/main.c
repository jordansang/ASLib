#include "main.h"
//#define NDEBUG 1
#include "assert.h"
#include "asd_list.h"

#include <sys/time.h>
#include <sys/resource.h>

static int tcount = 0;
void gen_bracket_recurse(char* last, struct asd_list *store, int cur, int n)
{
    //asl_print_dbg("gen_bracket_recurse cur:%d n:%d", cur, n);
    tcount++;
    int index;
    int avalibleCount = cur + 1;
    if(cur == n - 1)
    {
        char **acur = (char**)asl_malloc(avalibleCount*sizeof(char*));
        for(index = 0; index < avalibleCount; index++)
        {
            acur[index] = (char*)asl_malloc(strlen(last) + 3);
        }
        for(index = 0; index < avalibleCount; index++)
        {
            memcpy(acur[index], last, index*sizeof(char));
            memcpy(acur[index]+index, "()", 2*sizeof(char));
            memcpy(acur[index]+index+2, last+index, (strlen(last) - index)*sizeof(char));
            //asd_list_add_no_dup(store, acur[index]);
            asd_list_add(store, acur[index]);
        }
        return;
    }
    else
    {
        char acur[avalibleCount][strlen(last) + 3];
        memset(acur, 0, avalibleCount*(strlen(last) + 3)*sizeof(char));
        for(index = 0; index < avalibleCount; index++)
        {
            memcpy(acur[index], last, index*sizeof(char));
            memcpy(acur[index]+index, "()", 2*sizeof(char));
            memcpy(acur[index]+index+2, last+index, (strlen(last) - index)*sizeof(char));
        }
        cur++;
        for(index = 0; index < avalibleCount; index++)
        {
            gen_bracket_recurse(acur[index], store, cur, n);
        }
    }
}
void gen_bracket_del_func(void *val)
{
    char* v = (char*)val;
    asl_free(v);
}
s_int32_t gen_bracket_cmp_func(void *a, void *b)
{
    char* aval = (char*)a;
    char* bval = (char*)b;
    return strcmp(aval, bval);
}
void gen_bracket_driver(int n)
{
    char *first = "()";
    struct asd_list *store = asd_list_create();
    store->delFunc = gen_bracket_del_func;
    store->cmpFunc = gen_bracket_cmp_func;
    gen_bracket_recurse(first, store, 1, n);
    asl_printf("[\n");
    int index;
    struct asd_list_item *node;
    char *tmp;
    #if 0
    ASD_LIST_LOOP(store, tmp, node)
    {
        asl_printf("  \"%s\"\n", tmp);
    }
    asl_printf("]\n");
    #endif
    asd_list_destroy(store);
}
void gen_bracket_driver_s(void *val)
{
    gen_bracket_driver(*((s_int32_t*)val));
}
u_int64_t func_time_consume(void (*func)(void*), void* arg)
{
    u_int64_t start, end;
    start = asl_time_get_stamp();
    func(arg);
    end = asl_time_get_stamp();
    asl_print_dbg("start:%lu end:%lu consume:%lu", start, end, end - start);
    return end- start;
}

/* test for #define and typedef */
int isBadPtr(void *ptr)
{
    return 1;
}
void tdtest()
{
    int *t = NULL;
    assert(!isBadPtr(t));
}

u_int32_t* thash_init(u_int32_t count)
{
     u_int32_t *src = (u_int32_t*)asl_malloc(count * sizeof(u_int32_t));
     while(count--)
        src[count] = rand();
     return src;
}
s_int32_t thash_cmpfunc(void *val1, void *val2)
{
    s_int32_t *src = (s_int32_t*)val1;
    s_int32_t *dst = (s_int32_t*)val2;
    if(*src == *dst)
    {
        return 0;
    }
    return (*src > *dst ? 1 : -1);
}
#define THASH_ITEM_COUNT  100000
#define THASH_TABLE_COUNT 2048
void thash()
{
    int count = THASH_ITEM_COUNT;
    u_int32_t *src = (u_int32_t*)asl_malloc(count * sizeof(u_int32_t));
    u_int32_t *target = (u_int32_t*)asl_malloc(count * sizeof(u_int32_t));
    while(count--)
        src[count] = rand();

    count = THASH_ITEM_COUNT;
    while(count--)
    {
        target[count] = src[rand()%THASH_ITEM_COUNT];
    }
    
    u_int32_t h = 0;
    struct asd_list *bucket[THASH_TABLE_COUNT];
    struct asd_list *vlist = asd_list_create();
    vlist->cmpFunc = thash_cmpfunc;
    count = THASH_TABLE_COUNT;
    while(count--)
    {
        bucket[count] = asd_list_create();
        bucket[count]->cmpFunc = thash_cmpfunc;
    }
    count = THASH_ITEM_COUNT;
    while(count--)
    {
        h = src[count]%THASH_TABLE_COUNT;
        asd_list_add(bucket[h], &src[count]);
        asd_list_add(vlist, &src[count]);
    }

    asl_print_dbg("Already Save Data to List Hash Table");

    count = THASH_TABLE_COUNT;
    
    u_int64_t start, end;
    count = THASH_ITEM_COUNT;
    start = asl_time_get_stamp();
    while(count--)
    {
        h = target[count]%THASH_TABLE_COUNT;
        asd_list_get_val(bucket[h], &target[count]);
    }
    end = asl_time_get_stamp();
    asl_print_dbg("Hash List: %lu", end - start);

    count = THASH_ITEM_COUNT;
    start = asl_time_get_stamp();
    while(count--)
    {
        asd_list_get_val(vlist, &target[count]);
    }
    end = asl_time_get_stamp();
    asl_print_dbg("List: %lu", end - start);

    asl_free(src);
    asl_free(target);

    count = THASH_TABLE_COUNT;
    while(count--)
        asd_list_destroy(bucket[count]);

    asd_list_destroy(vlist);
}
inline s_int32_t tadd(const s_int32_t a, const s_int32_t b)
{
    return a + b;
}
void trlimit()
{
    //int getrlimit(int resource, struct rlimit *rlim);
    struct rlimit rlim;
    getrlimit(RLIMIT_CORE, &rlim);
    if(rlim.rlim_cur == RLIM_INFINITY)
        asl_print_dbg("RLIMIT_CORE cur is infinity");
    else
        asl_print_dbg("RLIMIT_CORE cur is %lu", rlim.rlim_cur);

    if(rlim.rlim_max == RLIM_INFINITY)
        asl_print_dbg("RLIMIT_CORE max is infinity");
    else
        asl_print_dbg("RLIMIT_CORE cur is %lu", rlim.rlim_max);
}
int main(int argc, char** argv)
{
    //gen_bracket_driver(10);
    //s_int32_t n = 10;
    //asl_print_dbg("Consume:%lu", func_time_consume(gen_bracket_driver_s, &n));
    //asl_print_dbg("Count:%d", tcount);
    //test_timer();
    //test_map();
    //thash();
    //s_int32_t c = tadd(1, 2);
    //asl_print_dbg("c:%d", c);

    //trlimit();
    //tcoredump();
    // test_log();
    test_ctrl();
    return 0;
}
