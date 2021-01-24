#include "asl_stdio.h"
#include "asl_memory.h"
#include "asd_fifo.h"


void asd_fifo_dump(struct asd_fifo *f)
{
    if(f == NULL)
    {
        asl_print_err("FIFO is NOT Initialized");
        return;
    }
    struct asd_fifo_item *fi = f->head;
    s_int32_t count = f->count;
    asl_printf("FIFO %p has %d items:\n", f, f->count);
    while(count--)
    {
        asl_printf("\tFIFO %d:\n", f->count - count - 1);
        if(f->dumpFunc)
        {
            (f->dumpFunc)(fi->data);
        }
        else
        {
            asl_printf("\tdata:%p\n", fi->data);
        }
        fi = fi->next;
    }
}

struct asd_fifo* asd_fifo_create()
{
    struct asd_fifo *f = (struct asd_fifo*)asl_malloc(sizeof(struct asd_fifo));
    if(f == NULL)
    {
        asl_print_err("Malloc for FIFO FAILED");
        return NULL;
    }
    f->count = 0;
    f->head = f->tail = NULL;
    return f;
}
result_t asd_fifo_enque(struct asd_fifo *f, void *data)
{
    if(f == NULL)
    {
        asl_print_err("FIFO is NOT Initialized");
        return FAILURE;
    }
    struct asd_fifo_item *fi = (struct asd_fifo_item*)asl_malloc(sizeof(struct asd_fifo_item));
    if(fi == NULL)
    {
        asl_print_err("Malloc for FIFO Item FAILED");
        return FAILURE;
    }
    fi->data = data;
    fi->prev = f->tail;
    fi->next = NULL;
    if(f->tail)
    {
        f->tail->next = fi;
    }
    f->tail = fi;
    if(f->head == NULL)
    {
        f->head = fi;
    }
    f->count++;
    return SUCCESS;
}
void* asd_fifo_deque(struct asd_fifo *f)
{
    if(f == NULL)
    {
        asl_print_err("FIFO is NOT Initialized");
        return NULL;
    }
    if(f->head == NULL)
    {
        return NULL;
    }
    struct asd_fifo_item *fi = f->head;
    void* data = fi->data;
    f->head = f->head->next;
    if(f->head != NULL)
    {
        f->head->prev = NULL;
    }
    else
    {
        f->tail = NULL;
    }
    asl_free(fi);
    f->count--;
    return data;
}
s_int32_t asd_fifo_count(struct asd_fifo *f)
{
    return f->count;
}
bool asd_fifo_is_empty(struct asd_fifo *f)
{
    if(f != NULL)
    {
        return (f->count == 0);
    }
    else
    {
        asl_print_err("FIFO is NOT Initialized");
        return TRUE;
    }
}
result_t asd_fifo_destroy(struct asd_fifo *f)
{
    if(f == NULL)
    {
        return SUCCESS;
    }
    void *fi;
    while((fi = asd_fifo_deque(f)) != NULL)
    {
        if(f->delFunc)
        {
            (f->delFunc)(fi);
        }
    }
    
    return asl_free(f);
}