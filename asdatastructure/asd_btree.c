#include "asl_inc.h"
#include "asd_list.h"
#include "asd_btree.h"

static s_int32_t get_times_of_two(s_int32_t times)
{
    s_int32_t ret = 1;
    s_int32_t index;
    for(index = 0; index < times; index++)
    {
        ret *= 2;
    }
    return ret;
}
static void asd_btree_dump_test_get_node_recurse(struct asd_list** nodeList, struct asd_btree_item *bn, s_int32_t *h)
{
    if(bn == NULL)
        return;
    struct asd_list *nl = nodeList[bn->hc - 1];
    asd_list_add(nl, bn);
    //asl_print_dbg("asd_btree_dump_test_get_node_recurse hc:%d", bn->hc);
    
    if(bn->hc == *h)
    {
        return;
    }
    else if(bn->count == 0)
    {
        s_int32_t level;
        s_int32_t nodeIndex;
        for(level = 0; level < *h - bn->hc; level++)
        {
            for(nodeIndex = 0; nodeIndex < get_times_of_two(level + 1); nodeIndex++)
                asd_list_add(nodeList[bn->hc + level], NULL);
        }
        return;
    }
    asd_btree_dump_test_get_node_recurse(nodeList, bn->l, h);
    asd_btree_dump_test_get_node_recurse(nodeList, bn->r, h);
    return;
}
static struct asd_list** asd_btree_dump_test_get_node_list(struct asd_btree *t, s_int32_t *h)
{
    //asl_print_dbg("asd_btree_dump_test_get_node_list");
    if(t == NULL || t->count == 0)
        return NULL;
    struct asd_list** nodeList = (struct asd_list**)asl_malloc(sizeof(struct asd_list*)*(*h));
    if(nodeList == NULL)
        return NULL;
    s_int32_t index;
    for(index = 0; index < *h; index++)
    {
        nodeList[index] = asd_list_create();
    }
    asd_btree_dump_test_get_node_recurse(nodeList, t->root, h);
    return nodeList;
}
void asd_btree_dump_test_ospace(s_int32_t count)
{
    if(count < 1)
        return;
    char* buf = (char*)asl_malloc(sizeof(char)*count*2 + 1);
    if(buf == NULL)
        return;
    char* space = " ";
    asl_memset(buf, *space, sizeof(char)*count*2);
    asl_printf("%s", buf);
    asl_free(buf);
    return;
}
void asd_btree_dump_test_orecurse(struct asd_list** nodeList, s_int32_t hc, s_int32_t cIndex, s_int32_t *h)
{
    if(hc > *h)
        return;
    //asl_print_dbg("asd_btree_dump_test_orecurse:hc:%d cIndex:%d h:%d",hc, cIndex, *h);
    struct asd_list *nl = nodeList[hc - 1];
    struct asd_list_item *head = nl->head;
    //s_int32_t count = get_times_of_two(hc - 1);
    s_int32_t spaceStart = get_times_of_two(*h - hc) - 1;
    s_int32_t space = get_times_of_two(*h - hc + 1) - 1;
    s_int32_t index;
    asd_btree_dump_test_ospace(spaceStart);
    if(head->data != NULL)
    {
        asl_printf("%02d", cIndex);
        cIndex++;
    }
    else
    {
        asl_printf("  ");
    }
    //asl_print_dbg("nl->count:%d count:%d", nl->count, count);
    //for(index = 1; index < count; index++)
    for(index = 1; index < nl->count; index++)
    {
        head = head->next;
        asd_btree_dump_test_ospace(space);
        if(head->data != NULL)
        {
            asl_printf("%02d", cIndex);
            cIndex++;
        }
        else
        {
            asl_printf("  ");
        }
    }
    asl_printf("\n");
    asd_btree_dump_test_orecurse(nodeList, hc + 1, cIndex, h);
    return;
}
void asd_btree_dump(struct asd_btree *t)
{
    if(t == NULL || t->count == 0)
        return;
    s_int32_t h = asd_btree_get_height(t);
    struct asd_list** nodeList = asd_btree_dump_test_get_node_list(t, &h);
    
    if(h > 6)
        h = 6;
    asd_btree_dump_test_orecurse(nodeList, 1, 0, &h);
    s_int32_t index;
    for(index = 0; index < h; index++)
    {
        asd_list_destroy(nodeList[index]);
    }
    asl_free(nodeList);
    return;
}

static struct asd_btree_item* asd_btree_lookup(struct asd_btree *t, struct asd_btree_item *bn, void *data)
{
    if(bn == NULL)
    {
        return NULL;
    }
    if((t->cmpFunc)(bn->data, data) == 0)
    {
        return bn;
    }
    struct asd_btree_item *bnl = asd_btree_lookup(t, bn->l, data);
    struct asd_btree_item *bnr = asd_btree_lookup(t, bn->r, data);
    return (bnl == NULL ?  bnr : bnl);
}
struct asd_btree *asd_btree_create()
{
    struct asd_btree *t = (struct asd_btree*)asl_malloc(sizeof(struct asd_btree));
    if(t == NULL)
    {
        asl_print_err("Malloc for Binary Tree FAILED");
        return NULL;
    }
    t->root = NULL;
    return t;
}
result_t asd_btree_insert(struct asd_btree *t, void* pdata, void* data)
{
    if(t == NULL || t->cmpFunc == NULL)
    {
        return FAILURE;
    }
    if(t->root == NULL)
    {
        t->root = (struct asd_btree_item*)asl_malloc(sizeof(struct asd_btree_item));
        t->root->p = t->root->l = t->root->r = NULL;
        t->root->count = 0;
        t->root->data = data;
        t->root->hc = 1;
    }
    else
    {
        struct asd_btree_item *pnode = asd_btree_lookup(t, t->root, pdata);
        if(pnode == NULL || pnode->count == 2)
        {
            return FAILURE;
        }
        struct asd_btree_item *newNode = (struct asd_btree_item*)asl_malloc(sizeof(struct asd_btree_item));
        if(newNode == NULL)
        {
            return FAILURE;
        }
        newNode->p = pnode;
        newNode->l = newNode->r = NULL;
        newNode->count = 0;
        newNode->data = data;
        newNode->hc = pnode->hc + 1;
        if(pnode->l == NULL)
        {
            pnode->l = newNode;
        }
        else
        {
            pnode->r = newNode;
        }
        pnode->count++;
    }
    t->count++;
    return SUCCESS;
}
static result_t asd_btree_remove_node(struct asd_btree *t, struct asd_btree_item *bn)
{
    if(bn == NULL)
    {
        return SUCCESS;
    }
    if(bn->l != NULL && asd_btree_remove_node(t, bn->l) == SUCCESS)
    {
        bn->count--;
    }
    if(bn->r != NULL && asd_btree_remove_node(t, bn->r) == SUCCESS)
    {
        bn->count--;
    }
    if(t->delFunc != NULL)
    {
        (t->delFunc)(bn->data);
    }
    if(bn->p)
    {
        if(bn->p->l == bn)
        {
            bn->p->l = NULL;
        }
        else
        {
            bn->p->r = NULL;
        }
        bn->p = NULL;
    }
    else
    {
        t->root = NULL;
    }
    asl_free(bn);
    bn = NULL;
    t->count--;
    return SUCCESS;
}
result_t asd_btree_remove(struct asd_btree *t, void* data)
{
    if(t == NULL)
    {
        return FAILURE;
    }
    return asd_btree_remove_node(t, asd_btree_lookup(t, t->root, data));
}
result_t asd_btree_destroy(struct asd_btree* t)
{
    if(t == NULL)
    {
        return SUCCESS;
    }
    if(asd_btree_remove_node(t, t->root) == SUCCESS)
    {
        asl_free(t);
        return SUCCESS;
    }
    return FAILURE;
}
static s_int32_t asd_btree_get_max(s_int32_t h, s_int32_t hl, s_int32_t hr)
{
    s_int32_t ret = h;
    if(hl > ret)
        ret = hl;
    if(hr > ret)
        ret = hr;
    return ret;
}
static s_int32_t asd_btree_height(struct asd_btree_item* bn, s_int32_t h)
{
    if(bn == NULL)
    {
        return h;
    }
    h++;
    s_int32_t hl, hr;
    hl = asd_btree_height(bn->l, h);
    hr = asd_btree_height(bn->r, h);
    return asd_btree_get_max(h, hl, hr);
}
s_int32_t asd_btree_get_height(struct asd_btree *t)
{
    if(t == NULL)
    {
        return 0;
    }
    return asd_btree_height(t->root, 0);
}
