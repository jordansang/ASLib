#ifndef ASD_BTREE_H
#define ASD_BTREE_H

struct asd_btree_item
{
    struct asd_btree_item *p;
    struct asd_btree_item *l;
    struct asd_btree_item *r;

    /* node height */
    s_int32_t hc;
    /* Child node count */
    s_int8_t count;
    void *data;
};

struct asd_btree
{
    struct asd_btree_item *root;

    s_int32_t count;
    cmp_func cmpFunc;
    del_func delFunc;
    dump_func dumpFunc;
};

struct asd_btree *asd_btree_create();
result_t asd_btree_insert(struct asd_btree *t, void* pdata, void* data);
result_t asd_btree_remove(struct asd_btree *t, void* data);
result_t asd_btree_destroy(struct asd_btree* t);
s_int32_t asd_btree_get_height(struct asd_btree *t);
void asd_btree_dump(struct asd_btree *t);
void asd_btree_dump_test(s_int32_t h);

#endif /* ASD_BTREE_H */