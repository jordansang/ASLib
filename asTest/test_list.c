#include "asl_inc.h"
#include "asd_list.h"

s_int32_t test_list_cmpfunc(void *val1, void *val2)
{
    s_int32_t *src = (s_int32_t*)val1;
    s_int32_t *dst = (s_int32_t*)val2;
    if(*src == *dst)
    {
        return 0;
    }
    return (*src > *dst ? 1 : -1);
}

void test_list_delfunc(void *val)
{
    return;
}

void test_list_dumpfunc(void *val)
{
    s_int32_t *src = (s_int32_t*)val;

    asl_printf("\tval:%d\n", *src);
}
/*
O struct asd_list* asd_list_create();
O result_t asd_list_add(struct asd_list *l, void *data);
O result_t asd_list_add_before(struct asd_list *l, void *cur, void *data);
O result_t asd_list_add_after(struct asd_list *l, void *cur, void* data);
O result_t asd_list_add_sort(struct asd_list *l, void *data);
O void* asd_list_head(struct asd_list *l);
O result_t asd_list_del_item(struct asd_list *l, void *data);
O result_t asd_list_del_all(struct asd_list *l);
O result_t asd_list_destroy(struct asd_list *l);
O result_t asd_list_add_list(struct asd_list *src, struct asd_list *dst);
O struct asd_list* asd_list_get_list(struct asd_list* l, void *val);
*/
void test_list()
{
    s_int32_t vals[10] = {0};
    s_int32_t pad[10] = {0};
    s_int32_t index;
    struct asd_list *l = asd_list_create();
    struct asd_list *pl = asd_list_create();
    struct asd_list *gl = NULL;
    if(l == NULL)
    {
        asl_print_err("Create asd_list FAILED");
        return;
    }
    if(pl == NULL)
    {
        asl_print_err("Create asd_list FAILED");
        return;
    }
    l->cmpFunc = test_list_cmpfunc;
    l->delFunc = test_list_delfunc;
    l->dumpFunc = test_list_dumpfunc;
    for(index = 0; index < 10; index++)
    {
        asd_list_add(pl, &pad[index]);
    }
    for(index = 0; index < 10; index++)
    {
        vals[index] = index;
        asd_list_add(l, &vals[index]);
    }
    asl_print_dbg("After Initialized");
    asd_list_dump(l);
    for(index = 0; index < 10; index += 2)
    {
        asd_list_del_item(l, &vals[index]);
    }
    asl_print_dbg("After Delete some items");
    asd_list_dump(l);
    asd_list_del_all(l);
    asl_print_dbg("After Delete all items");
    asd_list_dump(l);
    for(index = 0; index < 10; index += 2)
    {
        asd_list_add(l, &vals[index]);
    }
    asl_print_dbg("After Add some items");
    asd_list_dump(l);
    asd_list_add_before(l, &vals[4], &vals[5]);
    asl_print_dbg("After Add before certain item %d", vals[4]);
    asd_list_dump(l);
    asd_list_add_after(l, &vals[4], &vals[3]);
    asl_print_dbg("After Add after certain item %d", vals[4]);
    asd_list_dump(l);
    asd_list_add_sort(l, &vals[7]);
    asl_print_dbg("After Add sort certain item");
    asd_list_dump(l);
    asl_print_dbg("List Head Val:%d", *(s_int32_t*)asd_list_head(l));
    s_int32_t *tmp;
    asl_print_dbg("ASD_LIST_LOOP");
    struct asd_list_item *li = NULL;
    ASD_LIST_LOOP(l, tmp, li)
    {
        asl_print_dbg("ASD_LIST_LOOP:%d", *tmp);
    }
    asd_list_add_list(l, pl);
    asl_print_dbg("After Add asd_list");
    asd_list_dump(l);
    gl = asd_list_get_list(l, &pad[0]);
    gl->dumpFunc = test_list_dumpfunc;
    asl_print_dbg("After Get asd_list");
    asd_list_dump(gl);
    asd_list_destroy(l);
    asd_list_destroy(pl);
    asd_list_destroy(gl);
}
