#ifndef ASD_FIFO_H
#define ASD_FIFO_H


struct asd_fifo_item
{
    struct asd_fifo_item *prev;
    struct asd_fifo_item *next;
    void *data;
};

struct asd_fifo
{
    struct asd_fifo_item *head;
    struct asd_fifo_item *tail;
    s_int32_t count;
    del_func delFunc;
    dump_func dumpFunc;
};


struct asd_fifo* asd_fifo_create();
result_t asd_fifo_enque(struct asd_fifo *f, void *data);
void* asd_fifo_deque(struct asd_fifo *f);
s_int32_t asd_fifo_count(struct asd_fifo *f);
bool asd_fifo_is_empty(struct asd_fifo *f);
result_t asd_fifo_destroy(struct asd_fifo *f);
void asd_fifo_dump(struct asd_fifo *f);


#endif /* ASD_FIFO_H */
