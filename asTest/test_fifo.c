#include "asl_inc.h"
#include "asd_fifo.h"

void test_fifo_delfunc(void *val)
{
    return;
}
void test_fifo_dumpfunc(void *val)
{
    s_int32_t *src = (s_int32_t*)val;
    asl_printf("\tval:%d\n", *src);
}
/*
struct asd_fifo* asd_fifo_create();
result_t asd_fifo_enque(struct asd_fifo *f, void *data);
void* asd_fifo_deque(struct asd_fifo *f);
s_int32_t asd_fifo_count(struct asd_fifo *f);
bool asd_fifo_is_empty(struct asd_fifo *f);
result_t asd_fifo_destroy(struct asd_fifo *f);
*/

void test_fifo()
{
    struct asd_fifo *f = asd_fifo_create();
    s_int32_t vals[10] = {0};
    s_int32_t *tmp = NULL;
    if(f == NULL)
    {
        asl_print_err("Create FIFO FAILED");
        return;
    }
    f->delFunc = test_fifo_delfunc;
    f->dumpFunc = test_fifo_dumpfunc;
    s_int32_t index;
    for(index = 0; index < 10; index++)
    {
        vals[index] = index;
        asd_fifo_enque(f, &vals[index]);
    }
    asl_print_dbg("After Initialized");
    asd_fifo_dump(f);
    asl_print_dbg("Dequeue");
    for(index = 0; index < 10; index++)
    {
        tmp = (s_int32_t*)asd_fifo_deque(f);
        asl_print_dbg("tmp:%d", *tmp);
    }
    if(asd_fifo_is_empty(f))
    {
        asl_print_dbg("1FIFO count:%d", asd_fifo_count(f));
        asd_fifo_enque(f, &vals[0]);
    }
    asl_print_dbg("2FIFO count:%d", asd_fifo_count(f));
    asl_print_dbg("FIFO Destroy:%s", asl_get_ret_str(asd_fifo_destroy(f)));
}
