#ifndef ASF_MEM_MANAGER_H
#define ASF_MEM_MANAGER_H

#include "asd_list.h"

#define ASF_MEMG_MTYPE (-1)

struct asf_memg_minfo_item
{
    u_int32_t type;
    char* desc;
};
struct asf_memg_minfo
{
    u_int32_t count;
    struct asf_memg_minfo_item *items;
};

struct asf_memg_binfo
{
    u_int32_t count;
    u_int32_t *size;
};

enum ASF_MEMG_OPE
{
    ASF_MEMG_OPE_SAVE,
    ASF_MEMG_OPE_TYPE,
    ASF_MEMG_OPE_SIZE,
    ASF_MEMG_OPE_ALLOCT,
    ASF_MEMG_OPE_FREET
};

enum ASF_MEMG_CMP
{
    ASF_MEMG_CMP_NE,
    ASF_MEMG_CMP_E,
    ASF_MEMG_CMP_S,
    ASF_MEMG_CMP_L
};

struct asf_memg_item
{
    u_int8_t ope;
    void* ptr;
    u_int32_t type;
    u_int32_t size;
    u_int64_t allocT;
    u_int64_t freeT;
};


struct asf_memg
{
    /* Memory Types */
    struct asf_memg_minfo *mInfo;
    /* Block types */
    struct asf_memg_binfo *binfo;
    /* Memory Record */
    struct asd_list* list;
};


struct asf_memg_dump_item
{
    u_int32_t size;
    u_int32_t count;
    /* Allocated Count */
    u_int32_t aCount;
    /* Freed Count */
    u_int32_t fCount;
};
void* asf_memg_alloc(u_int32_t type, u_int32_t size);
result_t asf_memg_free(void *ptr);
void* asf_memg_realloc(void* ptr, u_int32_t size);
void asf_memg_dump(u_int8_t ope);
result_t asf_memg_create(struct asf_memg_minfo *info);
result_t asf_memg_destroy();


#endif /* ASF_MEM_MANAGER_H */
