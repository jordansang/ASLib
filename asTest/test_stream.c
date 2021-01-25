#include "asl_inc.h"
#include "asd_stream.h"

#define TEST_STREAM_LEN 1500

struct test_stream_head
{
    u_int32_t proto;
    u_int8_t  id;
    u_int16_t seq;
    u_int64_t stamp;
    u_int8_t  flag;
};
#define TSH_LEN 16
#define TSH_T_LEN sizeof(struct test_stream_head)

struct test_stream_body_head
{
    u_int32_t type;
    u_int8_t  check;
    u_int16_t author;
    u_int64_t len;
};
#define TSBH_LEN 15
#define TSBH_T_LEN sizeof(struct test_stream_body_head)

void test_stream_head_encode(struct asd_stream *s, struct test_stream_head *h)
{
    asd_stream_clear(s);
    asd_stream_put_int(s, &h->proto);
    asd_stream_put_char(s, &h->id);
    asd_stream_put_short(s, &h->seq);
    asd_stream_put_longlong(s, &h->stamp);
    asd_stream_put_char(s, &h->flag);
}

void test_stream_body_head_encode(struct asd_stream *s, struct test_stream_body_head *h)
{
    asd_stream_put_int(s, &h->type);
    asd_stream_put_char(s, &h->check);
    asd_stream_put_short(s, &h->author);
    asd_stream_put_longlong(s, &h->len);
}

void test_stream_head_decode(struct asd_stream *s, struct test_stream_head *h)
{
    asd_stream_reset_takepos(s);
    asd_stream_take_int(s, &h->proto);
    asd_stream_take_char(s, &h->id);
    asd_stream_take_short(s, &h->seq);
    asd_stream_take_longlong(s, &h->stamp);
    asd_stream_take_char(s, &h->flag);
}

void test_stream_body_head_decode(struct asd_stream *s, struct test_stream_body_head *h)
{
    asd_stream_take_int(s, &h->type);
    asd_stream_take_char(s, &h->check);
    asd_stream_take_short(s, &h->author);
    asd_stream_take_longlong(s, &h->len);
}

void test_stream_exchange(struct asd_stream *src, struct asd_stream *dst)
{
    asd_stream_clear(dst);
    asd_stream_put(dst, ASD_STREAM_DATA(src), ASD_STREAM_DSIZE(src));
}

void test_stream()
{
    struct asd_stream *src, *dst;
    src = asd_stream_create(TEST_STREAM_LEN);
    dst = asd_stream_create(TEST_STREAM_LEN);
    struct test_stream_head tsh;
    asl_memset(&tsh, 0, TSH_T_LEN);
    struct test_stream_body_head tsbh;
    asl_memset(&tsbh, 0, TSBH_T_LEN);

    char body[10];
    asl_memset(body, 1, 10);

    tsh.proto = 0x11;
    tsh.id = 0x01;
    tsh.seq = 0x100;
    tsh.stamp = asl_time_get_stamp();
    tsh.flag = 0xee;
    asl_print_dbg("TSH len:  %d vs %ld", TSH_LEN, TSH_T_LEN);
    asl_print_hex(&tsh, TSH_T_LEN, "TSH");
    
    tsbh.type = 0x02;
    tsbh.check = 0xaa;
    tsbh.author = 0x66;
    tsbh.len = 0x0a;
    asl_print_dbg("TSBH len: %d vs %ld", TSBH_LEN, TSBH_T_LEN);
    asl_print_hex(&tsbh, TSBH_T_LEN, "TSBH");

    test_stream_head_encode(src, &tsh);
    test_stream_body_head_encode(src, &tsbh);
    asd_stream_put(src, body, 10);

    asl_print_hex(ASD_STREAM_DATA(src), ASD_STREAM_DSIZE(src), "Output Stream");

    test_stream_exchange(src, dst);

    asl_print_hex(ASD_STREAM_DATA(dst), ASD_STREAM_DSIZE(dst), "Input Stream");
    asl_memset(&tsh, 0, TSH_T_LEN);
    asl_memset(&tsbh, 0, TSBH_T_LEN);
    asl_memset(body, 0, 10);
    test_stream_head_decode(dst, &tsh);
    asl_print_hex(&tsh, TSH_T_LEN, "TSH after");
    test_stream_body_head_decode(dst, &tsbh);
    asl_print_hex(&tsbh, TSBH_T_LEN, "TSBH after");
    asd_stream_take(dst, body, 10);
    asl_print_hex(body, 10, "body after");

    asd_stream_destroy(&src);
    asd_stream_destroy(&dst);
}
