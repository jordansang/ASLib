#include "asl_stdio.h"
#include "asl_memory.h"
#include "asd_list.h"


static struct asd_list_item* list_lookup_item(struct asd_list *l, void* data)
{
    if(l->cmpFunc != NULL)
    {
        struct asd_list_item *li;
        for(li = l->head; li; li = li->next)
        {
            if((l->cmpFunc)(li->data, data) == 0)
            {
                return li;
            }
        }
    }
    return NULL;
}
void asd_list_dump(struct asd_list *l)
{
    if(l == NULL)
    {
        asl_print_err("List is NOT Initialized");
        return;
    }
    asl_printf("List %p has %d items:\n", l, l->count);
    s_int32_t index = 0;
    void *data;
    struct asd_list_item *li = NULL;
    ASD_LIST_LOOP(l, data, li)
    {
        asl_printf("\tList item %d:\n", index++);
        if(l->dumpFunc != NULL)
        {
            (l->dumpFunc)(data);
        }
        else
        {
            asl_printf("\tdata:%p\n", data);
        }
    }
}
struct asd_list* asd_list_create()
{
    struct asd_list *l = (struct asd_list*)asl_malloc(sizeof(struct asd_list));
    if(l == NULL)
    {
        asl_print_err("Malloc for List FAILED");
        return NULL;
    }
    
    l->head = l->tail = NULL;
    l->count = 0;
    l->cmpFunc = NULL;
    l->delFunc = NULL;
    l->dumpFunc = NULL;
    return l;
}
result_t asd_list_add_no_dup(struct asd_list *l, void *data)
{
    if (l == NULL)
    {
        asl_print_err("List is NOT Initialized");
        return FAILURE;
    }
    struct asd_list_item *tmp = list_lookup_item(l, data);
    if(tmp != NULL)
    {
        return FAILURE;
    }
    return asd_list_add(l, data);
}

result_t asd_list_add(struct asd_list *l, void *data)
{
    if (l == NULL)
    {
        asl_print_err("List is NOT Initialized");
        return FAILURE;
    }
    struct asd_list_item *li = (struct asd_list_item*)asl_malloc(sizeof(struct asd_list_item));
    if(li == NULL)
    {
        asl_print_err("Malloc for List Item FAILED");
        return FAILURE;
    }
    li->prev = l->tail;
    li->data = data;

    if (l->head == NULL)
    {
        l->head = li;
    }
    else
    {
        l->tail->next = li;
    }
    l->tail = li;
    l->count++;
    return SUCCESS;
}
result_t asd_list_add_before(struct asd_list *l, void *cur, void *data)
{
    if (l == NULL)
    {
        asl_print_err("List is NOT Initialized");
        return FAILURE;
    }
    struct asd_list_item *tli = list_lookup_item(l, cur);
    if(tli == NULL)
    {
        return FAILURE;
    }
    struct asd_list_item *li = (struct asd_list_item*)asl_malloc(sizeof(struct asd_list_item));
    if(li == NULL)
    {
        asl_print_err("Malloc for List Item FAILED");
        return FAILURE;
    }
    li->data = data;
    li->next = tli;
    tli->prev->next = li;
    li->prev = tli->prev;
    tli->prev = li;
    if(l->head == tli)
    {
        l->head = li;
    }
    l->count++;
    asd_list_dump(l);
    return SUCCESS;    
}
result_t asd_list_add_after(struct asd_list *l, void *cur, void* data)
{
    if (l == NULL)
    {
        asl_print_err("List is NOT Initialized");
        return FAILURE;
    }
    struct asd_list_item *tli = list_lookup_item(l, cur);
    if(tli == NULL)
    {
        return FAILURE;
    }
    struct asd_list_item *li = (struct asd_list_item*)asl_malloc(sizeof(struct asd_list_item));
    if(li == NULL)
    {
        asl_print_err("Malloc for List Item FAILED");
        return FAILURE;
    }
    li->data = data;
    li->next = tli->next;
    tli->next->prev = li;
    li->prev = tli;
    tli->next = li;
    if(l->tail == tli)
    {
        l->tail = li;
    }
    l->count++;
    return SUCCESS; 
}
result_t asd_list_add_sort(struct asd_list *l, void *data)
{
    if (l->cmpFunc)
    {
        struct asd_list_item *tli;
        if (l == NULL)
        {
            asl_print_err("List is NOT Initialized");
            return FAILURE;
        }

        struct asd_list_item *li = (struct asd_list_item*)asl_malloc(sizeof(struct asd_list_item));
        if(li == NULL)
        {
            asl_print_err("Malloc for List Item FAILED");
            return FAILURE;
        }

        li->data = data;

    
        for (tli = l->head; tli; tli = tli->next)
        {
            if ((l->cmpFunc)(data, tli->data) <= 0)
            {
                li->next = tli;
                li->prev = tli->prev;

                if (tli->prev)
                {
                    tli->prev->next = li;
                }
                else
                {
                    l->head = li;
                }
                tli->prev = li;
                l->count++;
                break;
            }
        }
        if(tli == NULL)
        {
            li->prev = l->tail;

            if (l->tail)
            {
                l->tail->next = li;
            }
            else
            {
                l->head = li;
            }

            l->tail = li;
            l->count++;
        }

        return SUCCESS;
    }

    
    return FAILURE;
}
void* asd_list_head(struct asd_list *l)
{
    if (l == NULL)
    {
        asl_print_err("List is NOT Initialized");
        return NULL;
    }
    if(l->head == NULL)
    {
        //asl_print_warn("No Data stored in List");
        return NULL;
    }
    return l->head->data;
}

void* asd_list_tail(struct asd_list *l)
{
    if (l == NULL)
    {
        asl_print_err("List is NOT Initialized");
        return NULL;
    }
    if(l->tail == NULL)
    {
        //asl_print_warn("No Data stored in List");
        return NULL;
    }
    return l->tail->data;
}

result_t asd_list_del_tail(struct asd_list *l)
{
    struct asd_list_item *tli;
    if (l != NULL && l->count != 0)
    {
        tli = l->tail;
        if(tli != NULL)
        {
            if (tli->prev)
            {
                tli->prev->next = tli->next;
            }
            else
            {
                l->head = tli->next;
            }

            if (tli->next)
            {
                tli->next->prev = tli->prev;
            }
            else
            {
                l->tail = tli->prev;
            }

            l->count--;
            
            if(l->delFunc)
            {
                (l->delFunc)(tli->data);
            }
            asl_free(tli);
        }
    }
    
    return SUCCESS;
}
result_t asd_list_del_head(struct asd_list *l)
{
    struct asd_list_item *tli;
    if (l != NULL && l->count != 0)
    {
        tli = l->head;
        if(tli != NULL)
        {
            if (tli->prev)
            {
                tli->prev->next = tli->next;
            }
            else
            {
                l->head = tli->next;
            }

            if (tli->next)
            {
                tli->next->prev = tli->prev;
            }
            else
            {
                l->tail = tli->prev;
            }

            l->count--;
            
            if(l->delFunc)
            {
                (l->delFunc)(tli->data);
            }
            asl_free(tli);
        }
    }
    
    return SUCCESS;
}

result_t asd_list_del_all(struct asd_list *l)
{
    if (l == NULL)
    {
        asl_print_err("List is NOT Initialized");
        return FAILURE;
    }
    struct asd_list_item *tli;
    for (tli = l->head; tli; tli = l->head)
    {
        if (tli->prev)
        {
            tli->prev->next = tli->next;
        }
        else
        {
            l->head = tli->next;
        }

        if (tli->next)
        {
            tli->next->prev = tli->prev;
        }
        else
        {
            l->tail = tli->prev;
        }

        l->count--;
        if(l->delFunc)
        {
            (l->delFunc)(tli->data);
        }
        asl_free(tli);
    }
    l->head = l->tail = NULL;
    l->count = 0;
    return SUCCESS;
}
result_t asd_list_destroy(struct asd_list *l)
{
    if (l != NULL && (asd_list_del_all(l) != SUCCESS || asl_free(l) != SUCCESS))
    {
        return FAILURE;
    }
    return SUCCESS;
}
result_t asd_list_add_list(struct asd_list *target, struct asd_list *src)
{
    if(target == NULL)
    {
        asl_print_err("List is NOT Initialized");
        return FAILURE;
    }
    if(src == NULL)
    {
        return SUCCESS;
    }
    void *data;
    struct asd_list_item *li = NULL;
    ASD_LIST_LOOP(src, data, li)
    {
        if(asd_list_add(target, data) != SUCCESS)
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}
struct asd_list* asd_list_get_list(struct asd_list* l, void *val)
{
    if(l == NULL || l->cmpFunc == NULL)
    {
        asl_print_err("List is NOT Initialized");
        return NULL;
    }
    struct asd_list *nlist = NULL;
    struct asd_list_item *li;

    for(li = l->head; li; li = li->next)
    {
        if(l->cmpFunc(li->data, val) == 0)
        {
            if(nlist == NULL)
            {
                nlist = asd_list_create();
            }
            
            asd_list_add(nlist, li->data);
        }
    }
    
    return nlist;
}
static s_int32_t list_default_cmp_func(void *a, void *b)
{
    if(a == b)
        return 0;
    else
        return (a < b ? -1 : 1);
}
result_t asd_list_item_is_exist(struct asd_list *l, void *data)
{
    struct asd_list_item *tmp;
    if(l != NULL && l->cmpFunc != NULL)
    {
        for(tmp = l->head; tmp; tmp = tmp->next)
        {
            if((l->cmpFunc)(tmp->data, data) == 0)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
void* asd_list_get_val(struct asd_list *l, void *data)
{
    struct asd_list_item *tmp;
    if(l != NULL && l->cmpFunc != NULL)
    {
        for(tmp = l->head; tmp; tmp = tmp->next)
        {
            if((l->cmpFunc)(tmp->data, data) == 0)
            {
                return tmp->data;
            }
        }
    }
    
    return NULL;
}
static struct asd_list_item* list_get_item(struct asd_list *l, void *data)
{
    struct asd_list_item *tmp;
    cmp_func cfunc = (l->cmpFunc == NULL ? list_default_cmp_func : l->cmpFunc);
    for(tmp = l->head; tmp; tmp = tmp->next)
    {
        if((cfunc)(tmp->data, data) == 0)
        {
            return tmp;
        }
    }
    return NULL;
}
result_t asd_list_del_item(struct asd_list *l, void *data)
{
    struct asd_list_item *tli;
    if (l != NULL && l->count != 0)
    {
        tli = list_get_item(l, data);
        if(tli != NULL)
        {
            if (tli->prev)
            {
                tli->prev->next = tli->next;
            }
            else
            {
                l->head = tli->next;
            }

            if (tli->next)
            {
                tli->next->prev = tli->prev;
            }
            else
            {
                l->tail = tli->prev;
            }

            l->count--;
            
            if(l->delFunc)
            {
                (l->delFunc)(tli->data);
            }
            asl_free(tli);
        }
    }
    
    return SUCCESS;
}
