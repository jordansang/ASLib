#ifndef ASD_STREAM_H
#define ASD_STREAM_H

enum ASD_STREAM_ERRNO
{
    ASD_STREAM_STREAM_SIZE_NOT_INVALID = 1,
    ASD_STREAM_DATA_SIZE_NOT_INVALID,
    ASD_STREAM_NO_ROOM,
    ASD_STREAM_OFFSET_INVALID,
    ASD_STREAM_ERRNO_MAX
};

#define ASD_STREAM_MAX_SIZE (0 - ASD_STREAM_ERRNO_MAX)

struct asd_stream
{
    u_int32_t size;
    char* data;
    char* end;
    char* rptr;
    char* wptr;
    u_int32_t dsize;
};

struct asd_stream* asd_stream_create(u_int32_t size);
u_int32_t asd_stream_put(struct asd_stream *s, void* val, u_int32_t size);

#define asd_stream_put_char(s, val) asd_stream_put(s, val, sizeof(u_int8_t))
#define asd_stream_put_short(s, val) asd_stream_put(s, val, sizeof(u_int16_t))
#define asd_stream_put_int(s, val) asd_stream_put(s, val, sizeof(u_int32_t))
#define asd_stream_put_longlong(s, val) asd_stream_put(s, val, sizeof(u_int64_t))

struct asd_stream* asd_stream_resize(struct asd_stream* s, u_int32_t size);
void asd_stream_clear(struct asd_stream *s);
void* asd_stream_take(struct asd_stream *s, void* target, u_int32_t size);

#define asd_stream_take_char(s,t) asd_stream_take(s,t, sizeof(u_int8_t))
#define asd_stream_take_short(s,t) asd_stream_take(s,t, sizeof(u_int16_t))
#define asd_stream_take_int(s,t) asd_stream_take(s,t, sizeof(u_int32_t))
#define asd_stream_take_longlong(s,t) asd_stream_take(s,t, sizeof(u_int64_t))

result_t asd_stream_set_takepos(struct asd_stream *s, u_int32_t pos);
#define asd_stream_reset_takepos(s) asd_stream_set_takepos(s, 0);

result_t asd_stream_set_putpos(struct asd_stream *s, u_int32_t pos);
#define asd_stream_reset_putpos(s) asd_stream_set_takepos(s, 0);

void asd_stream_destroy(struct asd_stream **s);
char* asd_stream_get_data(struct asd_stream *s);
#define ASD_STREAM_DATA(S) asd_stream_get_data(S)
u_int32_t asd_stream_get_dsize(struct asd_stream *s);
#define ASD_STREAM_DSIZE(S) asd_stream_get_dsize(S)

#endif /* ASD_STREAM_H */
