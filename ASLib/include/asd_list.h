/* Created By JS <sanghe@163.com> */

/*************************************************************************
Create Date: 2017/04/20

Filename: 

File Description:

Modified by:

Modified Date:

Modification Description:

*************************************************************************/
#ifndef ASD_LIST_H
#define ASD_LIST_H

struct asd_list_item
{
    struct asd_list_item *prev;
    struct asd_list_item *next;
    void *data;
};

struct asd_list
{
    struct asd_list_item *head;
    struct asd_list_item *tail;
    s_int32_t count;
    cmp_func cmpFunc;
    del_func delFunc;
    dump_func dumpFunc;
};

struct asd_list* asd_list_create();
result_t asd_list_add(struct asd_list *l, void *data);
result_t asd_list_add_no_dup(struct asd_list *l, void *data);
result_t asd_list_add_before(struct asd_list *l, void *cur, void *data);
result_t asd_list_add_after(struct asd_list *l, void *cur, void* data);
result_t asd_list_add_sort(struct asd_list *l, void *data);
void* asd_list_head(struct asd_list *l);
void* asd_list_tail(struct asd_list *l);
result_t asd_list_del_tail(struct asd_list *l);
result_t asd_list_del_head(struct asd_list *l);
result_t asd_list_del_item(struct asd_list *l, void *data);
result_t asd_list_del_all(struct asd_list *l);
result_t asd_list_destroy(struct asd_list *l);
result_t asd_list_add_list(struct asd_list *src, struct asd_list *dst);
struct asd_list* asd_list_get_list(struct asd_list* l, void *val);
void asd_list_dump(struct asd_list *l);
result_t asd_list_item_is_exist(struct asd_list *l, void *data);
void* asd_list_get_val(struct asd_list *l, void *data);


/* List iteration macro. */
#define ASD_LIST_LOOP(L,V, N) \
  for (N = (L)->head; (N); (N) = (N)->next) \
    if (((V) = (N)->data) != NULL)

#endif /* ASD_LIST_H */
