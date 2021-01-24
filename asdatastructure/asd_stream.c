#include "asl_stdio.h"
#include "asl_memory.h"
#include "asd_stream.h"

struct asd_stream* asd_stream_create(u_int32_t size)
{
    if(size == 0)
        return NULL;
    if(size >= ASD_STREAM_MAX_SIZE)
    {
        asl_print_err("Stream Size is too Large");
        return NULL;
    }
    struct asd_stream *s = (struct asd_stream*)asl_malloc(sizeof(struct asd_stream) + size);
    if(s == NULL)
    {
        asl_print_err("Malloc for stream FAILED");
        return NULL;
    }
    s->data = (char*)s + sizeof(struct asd_stream);
    s->end = s->data + size;
    s->rptr = s->wptr = s->data;
    s->size = size;
    s->dsize = 0;
    return s;
}
u_int32_t asd_stream_put(struct asd_stream *s, void* val, u_int32_t size)
{
    if(s == NULL)
        return 0;
    if(val == NULL || size <= 0)
        return 0;
    if(s->dsize + size > s->size)
    {
        asl_print_err("Stream is FULL");
        return ASD_STREAM_NO_ROOM;
    }
    asl_memcpy(s->wptr, (char*)val, size);
    s->wptr += size;
    s->dsize += size;
    return size;
}
struct asd_stream* asd_stream_resize(struct asd_stream* s, u_int32_t size)
{
    if(size <= s->size)
    {
        s->size = size;
        s->end = s->data + size;
        s->dsize = (s->dsize > size ? size : s->dsize);
        s->rptr = (s->rptr > s->end ? s->end : s->rptr);
        s->wptr = (s->wptr > s->end ? s->end : s->wptr);
    }
    else
    {
        u_int32_t roffset, woffset;
        roffset = s->rptr - s->data;
        woffset = s->wptr - s->data;
        s = (struct asd_stream*)asl_realloc(s, sizeof(struct asd_stream) + size);
        s->data = (char*)s + sizeof(struct asd_stream);
        s->end = s->data + size;
        s->rptr = s->data + roffset;
        s->wptr = s->data + woffset;
        s->size = size;
    }
    return s;
}
void asd_stream_clear(struct asd_stream *s)
{
    if(s == NULL)
        return;
    s->dsize = 0;
    s->rptr = s->wptr = s->data;
}
void* asd_stream_take(struct asd_stream *s, void* target, u_int32_t size)
{
    if(s == NULL || target == NULL || size == 0)
        return NULL;
    if(s->dsize < size || s->end - s->rptr < size)
    {
        asl_print_err("No Valid Data in stream");
        return NULL;
    }
    asl_memcpy((char*)target, s->rptr, size);
    s->rptr += size;
    return target;
}
result_t asd_stream_set_takepos(struct asd_stream *s, u_int32_t pos)
{
    if(s == NULL || pos > s->size)
        return FAILURE;
    s->rptr = s->data + pos;
    return SUCCESS;
}
result_t asd_stream_set_putpos(struct asd_stream *s, u_int32_t pos)
{
    if(s == NULL || pos > s->size)
        return FAILURE;
    s->wptr = s->data + pos;
    return SUCCESS;
}

void asd_stream_destroy(struct asd_stream **s)
{
    if(s == NULL || *s == NULL)
        return;
    asl_free(*s);
    *s = NULL;
}

char* asd_stream_get_data(struct asd_stream *s)
{
    if(s == NULL)
        return NULL;
    return s->data;
}
u_int32_t asd_stream_get_dsize(struct asd_stream *s)
{
    if(s == NULL)
        return 0;
    return s->dsize;
}
