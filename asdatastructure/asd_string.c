#include "asl_stdio.h"
#include "asl_string.h"
#include "asd_string.h"
#include "asl_memory.h"

struct asd_str* asd_str_create()
{
    struct asd_str *str = ASL_MALLOC_T(struct asd_str);
    str->buf = NULL;
    str->size = 0;
    str->len = 0;
    str->freeCount = 0;
    return str;
}
result_t asd_str_destroy(struct asd_str **str)
{
    if(str != NULL && *str != NULL)
    {
        if((*str)->buf != NULL)
            asl_free((*str)->buf);
        asl_free(*str);
        *str = NULL;
    }

    return SUCCESS;
}


s_int32_t asd_str_appendf(struct asd_str *str, char* fmt, ...)
{
    if(str == NULL)
        return -1;
    char c;
    va_list ap;
    va_start(ap, fmt);
    s_int32_t oldLen = str->len;
    str->len += vsnprintf(&c, 1, fmt, ap);
    
    if(str->size < str->len + 1)
    {
        str->buf = (char*)asl_realloc(str->buf, str->len + 1);
        str->size = str->len + 1;
    }

    if(str->size > 2 *(str->len + 1))
    {
        if(++str->freeCount == 3)
        {
            str->size = (str->size)/2;
            str->buf = (char*)asl_realloc(str->buf, str->size);
            str->freeCount = 0;
        }
        
    }
    vsnprintf(str->buf + oldLen, str->len - oldLen + 1, fmt, ap);
    //asl_print_dbg("ret:%d", vsprintf(str->buf, fmt, ap));
    
    va_end(ap);
    return str->len;
}
s_int32_t asd_str_putf(struct asd_str *str, char* fmt, ...)
{
    if(str == NULL)
        return -1;
    if(str->len != 0)
        asd_str_clear(str);
    
    char c;
    
    va_list ap;
    va_start(ap, fmt);
    str->len = vsnprintf(&c, 1, fmt, ap);
    
    if(str->size < str->len + 1)
    {
        str->size = str->len + 1;
        str->buf = (char*)asl_realloc(str->buf, str->size);
    }

    if(str->size > 2 *(str->len + 1))
    {
        if(++str->freeCount == 3)
        {
            str->size = (str->size)/2;
            str->buf = (char*)asl_realloc(str->buf, str->size);
            str->freeCount = 0;
        }
        
    }
    vsnprintf(str->buf, str->len + 1, fmt, ap);
    //asl_print_dbg("ret:%d", vsprintf(str->buf, fmt, ap));
    
    va_end(ap);
    return str->len;
}
#define ASD_STR_MIN(A, B) (A > B ? B : A)
s_int32_t asd_str_putns(struct asd_str *str, const char* src, s_int32_t n)
{
    if(str != NULL && n > 0)
    {
        s_int32_t len = ASD_STR_MIN(asl_strtlen(src), n + 1);
        char* buf = (char*)asl_malloc(len);
        asl_memcpy(buf, src, len - 1);

        len = asd_str_puts(str, buf);

        asl_free(buf);

        return len;
    }

    return -1;
}

result_t asd_str_clear(struct asd_str *str)
{
    if(str != NULL && str->len > 0)
    {
        asl_memset(str->buf, 0, str->len);
        str->len = 0;
    }
    return SUCCESS;
}
result_t asd_strcpy(struct asd_str *dst, struct asd_str *src)
{
    if(dst != NULL && src != NULL)
    {
        asd_str_putns(dst, ASD_STR_CONTENT(src), ASD_STR_LEN(src));
        return SUCCESS;
    }
    return FAILURE;
}
