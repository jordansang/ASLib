#ifndef ASL_STRING_H
#define ASL_STRING_H

#include <string.h>

size_t asl_strlen(const void* val);
#define asl_formatstrtlen(format, ...) (snprintf(NULL, 0, format, ##__VA_ARGS__) + 1)
#define asl_strtlen(str) (asl_strlen(str) + 1)

/* string to hex, "01" to 0x01 */
result_t asl_stoh(u_int8_t *dst, char* src);

char* asl_strchr(const void* val, int c);
char* asl_strrchr(const char* src, int c);
/* find target character in n from source end */
char* asl_strnrchr(const char* src, int c, u_int32_t n);
char* asl_strstr(const char* src, const char* t);
char** asl_str_split(const char *src, const char ch, s_int32_t *vCount);

#define asl_strncpy(dest, src, n) strncpy(dest, src, n)
#define asl_strcmp(a, b) strcmp(a, b)
#define asl_strncmp(a, b, n) strncmp(a, b, n)
#endif /* ASL_STRING_H */
