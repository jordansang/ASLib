#include "asl_inc.h"
#include "asd_btree.h"
/*
1. create a binary tree
2. insert 15 nodes, dump tree
3. remove 5 nodes, dump tree
4. delete root, dump tree
5. insert 15 nodes to create a only left tree, dump tree, calculate height
6. remove root
7. insert 15 nodes to create a balance tree, dump tree, calculate height
8. remove root
9. insert 100 nodes randomly, calculate height, balance tree, calculate height
10. loop step 8 and step 9 for 10 times
11. destroy binary tree
*/
/*
struct asd_btree *asd_btree_create();
result_t asd_btree_insert(struct asd_btree *t, void* pdata, void* data);
result_t asd_btree_remove(struct asd_btree *t, void* data);
result_t asd_btree_destroy(struct asd_btree* t);
s_int32_t asd_btree_height(struct asd_btree_item* bn, s_int32_t h);
result_t asd_btree_balance(struct asd_btree *t);
void asd_btree_dump(struct asd_btree *t);
*/
s_int32_t test_btree_cmpfunc(void *val1, void *val2)
{
    s_int32_t *src = (s_int32_t*)val1;
    s_int32_t *dst = (s_int32_t*)val2;
    if(*src == *dst)
    {
        return 0;
    }
    return (*src > *dst ? 1 : -1);
}

void test_btree_delfunc(void *val)
{
    return;
}

void test_btree_dumpfunc(void *val)
{
    return;
}

void test_btree()
{
    struct asd_btree *t = asd_btree_create();
    if(t == NULL)
    {
        asl_print_err("Create Binary Tree FAILED");
        return;
    }
    t->cmpFunc = test_btree_cmpfunc;
    t->delFunc = test_btree_delfunc;
    t->dumpFunc = test_btree_dumpfunc;
    s_int32_t vals[15] = {0};
    s_int32_t index;
    s_int32_t count = 0;
    s_int32_t *tmp = &vals[0];
    asd_btree_insert(t, NULL, &vals[0]);
    for(index = 1; index < 15; index++)
    {
        vals[index] = index;
        if(count == 2)
        {
            tmp++;
            count = 0;
        }
        asd_btree_insert(t, tmp, &vals[index]);
        count++;
    }
    asl_print_dbg("Balance Binary Tree Height:%d", asd_btree_get_height(t));
    asd_btree_dump(t);
    for(index = 14; index > 6; index--)
    {
        asd_btree_remove(t, &vals[index]);
    }
    asl_print_dbg("After Remove Balance Binary Tree Height:%d", asd_btree_get_height(t));
    asd_btree_dump(t);
    asd_btree_remove(t, &vals[0]);
    asd_btree_dump(t);
    asd_btree_insert(t, NULL, &vals[0]);
    for(index = 1; index < 15; index++)
    {
        asd_btree_insert(t, &vals[index - 1], &vals[index]);
    }
    asl_print_dbg("Only Left Binary Tree Height:%d", asd_btree_get_height(t));
    asd_btree_dump(t);
    asd_btree_remove(t, &vals[0]);
    s_int32_t rvals[100] = {0};
    for(index = 0; index < 100; index++)
    {
        rvals[index] = index;
    }

    asd_btree_destroy(t);
    return;
}
