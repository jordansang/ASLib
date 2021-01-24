#ifndef ASD_STRING_H
#define ASD_STRING_H

#include "asl_stdio.h"


struct asd_str
{
    char* buf;
    s_int32_t len;
    s_int32_t size;
    u_int32_t freeCount;
};

struct asd_str* asd_str_create();
result_t asd_str_destroy(struct asd_str **str);

s_int32_t asd_str_appendf(struct asd_str *str, char* fmt, ...);
#define asd_str_appends(str, src) asd_str_appendf(str, "%s", src)
#define asd_str_appendc(str, ch)  asd_str_appendf(str, "%c", ch)

s_int32_t asd_str_putf(struct asd_str *str, char* fmt, ...);
#define asd_str_puts(str, src) asd_str_putf(str, "%s", src)
#define asd_str_putc(str, ch) asd_str_putf(str, "%c", ch)
s_int32_t asd_str_putns(struct asd_str *str, const char* src, s_int32_t n);

result_t asd_str_clear(struct asd_str *str);
#define ASD_STR_CONTENT(str) (str->buf)
#define ASD_STR_LEN(str) (str->len)
#define ASD_STR_TLEN(str) (str->len + 1)
result_t asd_strcpy(struct asd_str *dst, struct asd_str *src);

#endif /* ASD_STRING_H */
