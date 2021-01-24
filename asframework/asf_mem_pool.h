#ifndef ASF_MEM_POOL_H
#define ASF_MEM_POOL_H

#include "asl_memory.h"

#define ASF_MEMP_LIST_NODE_STAMP_FOREVER (-1)

typedef struct asf_memp_list_node_s
{
    struct asf_memp_list_node_s *pre;
    struct asf_memp_list_node_s *next;

    /* Memory Pointer */
    void *ptr;
    /* Original Size */
    u_int32_t oSize;
    /* Resized Size */
    u_int32_t rSize;
    /* Allocate time stamp */
    u_int64_t stamp;
}asf_memp_list_node;

typedef struct asf_memp_list_s
{
    struct asf_memp_list_node_s *head;
    struct asf_memp_list_node_s *tail;

    u_int32_t count;
}asf_memp_list;


struct asf_memp_tbl
{
    u_int32_t blkSize;
    asf_memp_list *freePtr;
    asf_memp_list *usedPtr;
    u_int32_t free;
    u_int32_t used;
};

struct asf_memp_tbl_container
{
    void* initPtr;
    u_int32_t initSize;
    struct asf_memp_tbl *tbls;
    u_int32_t count;
};

void* asf_memp_alloc(u_int32_t size);
result_t asf_memp_free(void *ptr);
void* asf_memp_realloc(void *ptr, u_int32_t size);
void asf_memp_dump();
void asf_memp_destroy();


#endif /* ASF_MEM_POOL_H */
