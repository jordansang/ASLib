#include "asl_type.h"
#include "asl_stdio.h"
#include "asl_memory.h"
#include "asl_string.h"

size_t asl_strlen(const void* val)
{
    const char* str = (const char*)val;
    if(str == NULL)
    {
        asl_print_warn("Target String is NULL");
        return 0;
    }
    return (strlen(str));
}
char* asl_strchr(const void* val, int c)
{
    const char* src = (const char*)val;
    if(src == NULL)
    {
        asl_print_warn("Target String is NULL");
        return NULL;
    }
    return (strchr(src, c));
}
char* asl_strrchr(const char* src, int c)
{
    if(src == NULL)
    {
        asl_print_warn("Target String is NULL");
        return NULL;
    }
    return (strrchr(src, c));
}
result_t asl_stoh(u_int8_t *dst, char* src)
{
    int index;
    u_int8_t current[2];
    for(index = 0; index < 2; index++)
    {
        current[index] = tolower(src[index]);
        if(current[index] >= '0' && current[index] <= '9')
        {
            current[index] = current[index] - '0';
        }
        else
        {
            current[index] = current[index] - 'W';
        }
    }
    *dst = (current[0] << 4) | (current[1]);
    return SUCCESS;
}
char* asl_strnrchr(const char* src, int c, u_int32_t n)
{
    int index;
    const char* p;
    if(src == NULL)
    {
        asl_print_err("Target string is NULL");
        return NULL;
    }
    p = src + n;
    /* skip itself */
    if(*p == c)
    {
        p--;
    }
    for(index = 0; index < n; index++)
    {
        if(*p == c)
        {
            return (char*)p;
        }
        p--;
    }
    return NULL;
}
char* asl_strstr(const char* src, const char* t)
{
    if(src == NULL || t == NULL)
    {
        asl_print_err("Source String or Target String is NULL");
        return NULL;
    }
    return (strstr(src, t));
}
char** asl_str_split(const char *src, const char ch, s_int32_t *vCount)
{
    if(src == NULL || vCount == NULL || *src == '\0')
        return NULL;
    char** vec = NULL;
    const char* p = src;
    *vCount = 0;
    while((p = asl_strchr(p, ch)) != NULL && p++)
    {
        vec = (char**)asl_realloc(vec, (*vCount + 1)*(sizeof(char*)));
        vec[*vCount] = (char*)asl_malloc(p - src);
        asl_memcpy(vec[*vCount], src, p - src - 1);
        src = p;
        (*vCount)++;
    }
    /* Last word */
    vec = (char**)asl_realloc(vec, (*vCount + 1)*(sizeof(char*)));
    vec[*vCount] = (char*)asl_malloc(asl_strtlen(src));
    asl_memcpy(vec[*vCount], src, asl_strlen(src));
    (*vCount)++;
    //asl_print_dbg("count:%d str:%s", *vCount, vec[0]);
    return vec;
}

