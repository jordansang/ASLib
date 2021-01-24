#ifndef ASL_STRUCT_H
#define ASL_STRUCT_H

struct key_str
{
    u_int32_t key;
    char* str;
};

struct key_key
{
    u_int32_t sKey;
    u_int32_t dKey;
};

struct str_str
{
    char* sstr;
    char* dstr;
};

#endif /* ASL_STRUCT_H */