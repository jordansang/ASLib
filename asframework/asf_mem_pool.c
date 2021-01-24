/* Created By JS. Contact with <sanghe@163.com> */

/*************************************************************************
Filename: asf_mem_pool.c

File Description: 
    ASF Memory Pool. Used as common memory allocate or free function. 
    This framework will shift running speed of programs.

Created by: JS

Create Date: 2018/03/27

Modified by:

Modified Date:

Modification Description:

*************************************************************************/
#include "asl_type.h"
#include "asl_stdio.h"
#include "asl_struct.h"
#include "asl_time.h"
#include "asl_memory.h"
#include "asf_mem_pool.h"
/* All available Memory tables */
struct asf_memp_tbl_container *mempGloble = NULL;
/************************************************************************* 
Function name: asf_memp_list_create

Function Description:
    Memory Pool List Create. It has the same operations as usual asd_list 
    datastruct.

Parameters:
    
Return:
    asd_list created

*************************************************************************/
asf_memp_list* asf_memp_list_create()
{
    asf_memp_list *ptrList = (asf_memp_list*)asl_malloc(sizeof(struct asf_memp_list_s));
    ptrList->head = ptrList->tail = NULL;
    ptrList->count = 0;
    return ptrList;
}
/************************************************************************* 
Function name: asf_memp_list_add

Function Description:
    Add new Memory pointer to target memory asd_list.

Parameters:
    asd_list        -- target memory asd_list
    ptr         -- new memory pointer
    
Return:
    new asd_list node
    NULL

*************************************************************************/
asf_memp_list_node* asf_memp_list_add(asf_memp_list *list, void *ptr)
{
    asf_memp_list_node *node;
    if(list == NULL)
    {
        asl_print_err("Memory Pointer List is NULL");
        return NULL;
    }

    node = (asf_memp_list_node*)asl_malloc(sizeof(struct asf_memp_list_node_s));
    node->ptr = ptr;
    if(list->head == NULL)
    {
        list->head = list->tail = node;
        node->pre = node->next = NULL;
    }
    else
    {
        node->pre = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
    list->count++;

    return node;
}
/************************************************************************* 
Function name: asf_memp_list_get_node

Function Description:
    Get target memory asd_list node with ptr in designated memory asd_list.

Parameters:
    asd_list        -- memory asd_list
    ptr         -- target memory pointer
    
Return:
    asd_list node with target ptr
    NULL

*************************************************************************/
asf_memp_list_node* asf_memp_list_get_node(asf_memp_list *list, void  *ptr)
{
    asf_memp_list_node *node = NULL;
    if(list == NULL)
    {
        asl_print_err("Memory Pointer List is NULL");
        return NULL;
    }
    for(node = list->head; node != NULL; node = node->next)
    {
        //asl_print_dbg("ptr: %p vs %p", ptr, node->ptr);
        if(ptr == node->ptr)
        {
            break;
        }
    }
    return node;
}
/************************************************************************* 
Function name: asf_memp_list_del

Function Description:
    Remove memory asd_list node with target ptr in memory asd_list.

Parameters:
    asd_list        -- memory asd_list
    ptr         -- memory pointer
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
result_t asf_memp_list_del(asf_memp_list *list, void *ptr)
{
    asf_memp_list_node *node = asf_memp_list_get_node(list, ptr);
    if(node != NULL)
    {
        if (node->pre)
            node->pre->next = node->next;
        else
            list->head = node->next;

        if (node->next)
            node->next->pre = node->pre;
        else
            list->tail = node->pre;

        list->count--;

        asl_free(node);
    }
    return SUCCESS;
}
/************************************************************************* 
Function name: asf_memp_list_get_last_node

Function Description:
    Get the tail node value of memory asd_list

Parameters:
    asd_list        -- memory asd_list
    
Return:
    memory pointer in tail node
    NULL

*************************************************************************/
void* asf_memp_list_get_last_node(asf_memp_list *list)
{
    if(list != NULL && list->tail != NULL)
    {
        return (list->tail->ptr);
    }
    return NULL;
}
/************************************************************************* 
Function name: asf_memp_list_destroy

Function Description:
    Release whole asd_list include all nodes in the asd_list.

Parameters:
    asd_list        -- memory asd_list
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
result_t asf_memp_list_destroy(asf_memp_list *list)
{
    void *ptr;
    while(list->count != 0)
    {
        ptr = asf_memp_list_get_last_node(list);
        asf_memp_list_del(list, ptr);
        asl_free(ptr);
    }
    return asl_free(list);
}
/************************************************************************* 
Function name: asf_memp_get_upper_size

Function Description:
    Memory Pool will allocate 2expn block. This function will determin which
    size of block shuold be allocate.

Parameters:
    size        -- size to be determinated
    
Return:
    final size or -1

*************************************************************************/
u_int32_t asf_memp_get_upper_size(u_int32_t size)
{
    u_int32_t index = 0;
    
    while(index < 32 && size > (1 << index++));
    return (index != 32 ? 1 << index : -1);
}
/************************************************************************* 
Function name: asf_memp_alloc

Function Description:
    Allocate Memory block. 
    1. It will create a new memory table for target blocksize, when alloc
       memory block with target size first time.
    2. It will create a new memory block if target memory asd_list doesn't have
       free memory pointer.
    3. It will return a free pointer it possible, and let it into used asd_list

Parameters:
    size        -- required memory size
    
Return:
    memory pointer

*************************************************************************/
void* asf_memp_alloc(u_int32_t size)
{
    u_int32_t index;
    void* ptr = NULL;
    u_int32_t upperSize = asf_memp_get_upper_size(size);
    struct asf_memp_tbl *tbl = NULL;
    asf_memp_list_node *node = NULL;
    asf_memp_list_node *tmpNode = NULL;
    
    /* 第一次使用内存池申请内存 */
    if(mempGloble == NULL)
    {
        mempGloble = (struct asf_memp_tbl_container*)asl_malloc(sizeof(struct asf_memp_tbl_container));
        mempGloble->count = 0;
        /* Not use create funtion to create memory pool */
        mempGloble->initPtr = NULL;
        mempGloble->initSize = 0;
    }
    
    for(index = 0; index < mempGloble->count; index++)
    {
        if(upperSize == mempGloble->tbls[index].blkSize)
        {
            tbl = &(mempGloble->tbls[index]);
            break;
        }
    }
    if(tbl == NULL)
    {
        mempGloble->tbls = asl_realloc(mempGloble->tbls, sizeof(struct asf_memp_tbl)*(mempGloble->count + 1));
        if(mempGloble->tbls != NULL)
        {
            mempGloble->tbls[mempGloble->count].blkSize = upperSize;
            
            mempGloble->tbls[mempGloble->count].free = 0;
            mempGloble->tbls[mempGloble->count].used = 1;
            mempGloble->tbls[mempGloble->count].freePtr = asf_memp_list_create();
            mempGloble->tbls[mempGloble->count].usedPtr = asf_memp_list_create();
            ptr = asl_malloc(mempGloble->tbls[mempGloble->count].blkSize);
            node = asf_memp_list_add(mempGloble->tbls[mempGloble->count].usedPtr, ptr);
            node->oSize = upperSize;
            node->rSize = size;
            node->stamp = asl_time_get_stamp();
            mempGloble->count++;
        }
    }
    else if(tbl->free == 0)
    {
        ptr = asl_malloc(tbl->blkSize);
        node = asf_memp_list_add(tbl->usedPtr, ptr);
        node->oSize = upperSize;
        node->rSize = size;
        node->stamp = asl_time_get_stamp();
        tbl->used++;
        //asl_print_dbg("Free asd_list is empty, new ptr %p with block size %u", ptr, node->oSize);
    }
    else
    {
        ptr = asf_memp_list_get_last_node(tbl->freePtr);
        node = asf_memp_list_get_node(tbl->freePtr, ptr);
        //asl_print_dbg("Free asd_list has available ptr %p with block size %u", ptr, node->oSize);
        tmpNode = asf_memp_list_add(tbl->usedPtr, ptr);
        tmpNode->oSize = node->oSize;
        tmpNode->rSize = size;
        tmpNode->stamp = node->stamp;
        tbl->used++;
        asf_memp_list_del(tbl->freePtr, ptr);
        tbl->free--;
    }
    
    return (ptr);
}
/************************************************************************* 
Function name: asf_memp_free

Function Description:
    Free memory pointer. It will make the pointer from used asd_list to free 
    asd_list.

Parameters:
    ptr     -- memory pointer
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
result_t asf_memp_free(void *ptr)
{
    result_t ret = FAILURE;
    u_int32_t index;
    struct asf_memp_tbl *tbl;
    asf_memp_list_node *node;
    asf_memp_list_node *tmpNode;
    if(ptr == NULL)
    {
        return SUCCESS;
    }
    if(mempGloble == NULL || mempGloble->tbls == NULL)
    {
        asl_print_err("Pointer %p is not allocated by ASF Memory Pool", ptr);
        return FAILURE;
    }
    for(index = 0; index < mempGloble->count; index++)
    {
        tbl = &(mempGloble->tbls[index]);
        node = asf_memp_list_get_node(tbl->usedPtr, ptr);
        if(node != NULL)
        {
            tmpNode = asf_memp_list_add(tbl->freePtr, ptr);
            tmpNode->oSize = node->oSize;
            tmpNode->stamp = node->stamp;
            tbl->free++;
            //asl_print_dbg("Add freed ptr %p to Free List, oSize:%u Stamp:%lu", ptr, tmpNode->oSize, tmpNode->stamp);
            asf_memp_list_del(tbl->usedPtr, ptr);
            tbl->used--;
            
            ret = SUCCESS;
            //asl_print_err("Find target item %p in memory tbl<%d>", ptr, tbl->blkSize);
            break;
        }
        else
        {
            //asl_print_err("Can't find target item %p in memory tbl<%d>", ptr, tbl->blkSize);
        }
    }
    if(index == mempGloble->count)
    {
        asl_print_err("Can't find target item %p in memory tbl", ptr);
    }
    return ret;
}
/************************************************************************* 
Function name: asf_memp_realloc

Function Description:
    Reallocate a memory pointer to a new size.
    1. It will allocate a new memory block, if the ptr is NULL.
    2. If the new size is not larger than the current blocksize, it will
       just return the pointer.
    3. If the new size is smaller or larger than the current blocksize, 
       it will allocate a new memory block and copy the content in old
       memory block to new memory block, then free the old memory pointer.

Parameters:
    ptr         -- old memory pointer
    size        -- new require size
    
Return:
    new memory pointer with require size

*************************************************************************/
void* asf_memp_realloc(void *ptr, u_int32_t size)
{
    u_int32_t index;
    struct asf_memp_tbl *tbl;
    asf_memp_list_node *node;
    void *nPtr = NULL;
    if(ptr == NULL)
    {
        ptr = asf_memp_alloc(size);
        return (ptr);
    }
    else
    {
        if(mempGloble == NULL || mempGloble->tbls == NULL)
        {
            asl_print_err("Pointer %p is not allocated by ASF Memory Pool", ptr);
            return NULL;
        }
        for(index = 0; index < mempGloble->count; index++)
        {
            tbl = &(mempGloble->tbls[index]);
            node = asf_memp_list_get_node(tbl->usedPtr, ptr);
            if(node != NULL)
            {
                if(size > tbl->blkSize || size < (tbl->blkSize / 2))
                {
                    nPtr = asf_memp_alloc(size);
                    asl_memcpy(nPtr, ptr, (size > tbl->blkSize ? tbl->blkSize : size));
                    asf_memp_free(ptr);
                }
                else
                {
                    nPtr = ptr;
                }
                break;
            }
        }
        if(index == mempGloble->count)
        {
            asl_print_err("Pointer %p is not allocated by ASF Memory Pool", ptr);
        }
    }
    
    return nPtr;
}
/************************************************************************* 
Function name: asf_memp_create

Function Description:
    Create a suit of memory lists with different block size before use the
    memory blocks. It is not prerequisite to use this function, but, it
    can surely improve the preference of memory pool.

Parameters:
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
result_t asf_memp_create(struct asf_memp_tbl_container *container)
{
    if(container == NULL || container->count == 0 || container->tbls == NULL)
    {
        return FAILURE;
    }
    u_int32_t index, ptrIndex;
    void* ptr = NULL;
    void* tmpPtr = NULL;
    struct asf_memp_tbl *tbl = NULL;
    u_int32_t totalSize = 0;
    asf_memp_list_node *node = NULL;
    /* 第一次使用内存池申请内存 */
    if(mempGloble == NULL)
    {
        mempGloble = (struct asf_memp_tbl_container*)asl_malloc(sizeof(struct asf_memp_tbl_container));
        mempGloble->count = container->count;
        /* Use create function to use memory pool framework */
    }
    else
    {
        asl_print_err("ASF Memory Pool has been using now.");
        return FAILURE;
    }

    for(index = 0; index < container->count; index++)
    {
        tbl = &(container->tbls[index]);
        //asl_print_dbg("blkSize:%d count:%d total:%d", tbl->blkSize, tbl->free, ((tbl->blkSize) * (tbl->free)));
        totalSize += ((tbl->blkSize) * (tbl->free));
    }
    //asl_print_dbg("totalSize:%d", totalSize);
    if((ptr = asl_malloc(totalSize)) == NULL)
    {
        asl_print_err("Create memory blocks failed");
        return FAILURE;
    }
    mempGloble->initPtr = ptr;
    mempGloble->initSize = totalSize;
    mempGloble->tbls = (struct asf_memp_tbl*)asl_malloc(sizeof(struct asf_memp_tbl)*(container->count));
    if(mempGloble->tbls == NULL)
    {
        asl_print_err("Create memory tables failed");
        return FAILURE;
    }
    tmpPtr = ptr;
    for(index = 0; index < container->count; index++)
    {
        tbl = &(container->tbls[index]);
        mempGloble->tbls[index].blkSize = tbl->blkSize;
            
        mempGloble->tbls[index].free = tbl->free;
        mempGloble->tbls[index].used = 0;
        mempGloble->tbls[index].freePtr = asf_memp_list_create();
        mempGloble->tbls[index].usedPtr = asf_memp_list_create();

        for(ptrIndex = 0; ptrIndex < tbl->free; ptrIndex++)
        {
            node = asf_memp_list_add(mempGloble->tbls[index].freePtr, tmpPtr);
            node->oSize = mempGloble->initSize;
            node->rSize = tbl->blkSize;
            node->stamp = ASF_MEMP_LIST_NODE_STAMP_FOREVER;
            tmpPtr += tbl->blkSize;
        }
    }
    asf_memp_dump();
    return SUCCESS;
}
/************************************************************************* 
Function name: asf_memp_destroy

Function Description:
    Determinate ALL ASF Memory Pool functions. It will release all parts
    in memory pool.

Parameters:
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
void asf_memp_destroy()
{
    u_int32_t index;
    void* ptr;
    asf_memp_list *list;
    asf_memp_list_node *node = NULL;
    struct asf_memp_tbl *tbl;
    if(mempGloble == NULL)
    {
        return;
    }
    if(mempGloble->tbls == NULL)
    {
        asl_free(mempGloble);
        mempGloble = NULL;
        return;
    }
    
    for(index = 0; index < mempGloble->count; index++)
    {
        tbl = &(mempGloble->tbls[index]);
        list = tbl->freePtr;
        while(list->count != 0)
        {
            ptr = asf_memp_list_get_last_node(list);
            node = asf_memp_list_get_node(list, ptr);
            if(node->oSize == tbl->blkSize)
            {
                //asl_print_dbg("Free %p with block size %u in free list", ptr, node->oSize);
                asl_free(ptr);
            }
            else
            {
                //asl_print_dbg("Ommit %p with block size %u in free list, blksize:%u", ptr, node->oSize, tbl->blkSize);
            }
            asf_memp_list_del(list, ptr);
        }
        asl_free(list);
        list = tbl->usedPtr;
        while(list->count != 0)
        {
            ptr = asf_memp_list_get_last_node(list);
            node = asf_memp_list_get_node(list, ptr);
            if(node->oSize == tbl->blkSize)
            {
                //asl_print_dbg("Free %p with block size %u in used list", ptr, node->oSize);
                asl_free(ptr);
            }
            else
            {
                //asl_print_dbg("Ommit %p with block size %u in used list, blksize:%u", ptr, node->oSize, tbl->blkSize);
            }
            asf_memp_list_del(list, ptr);
        }
        asl_free(list);
    }
    if(mempGloble->initPtr != NULL)
    {
        //asl_print_dbg("Free %p for preallocate", mempGloble->initPtr);
        asl_free(mempGloble->initPtr);
        mempGloble->initPtr = NULL;
    }
    asl_free(mempGloble->tbls);
    mempGloble->tbls = NULL;
    asl_free(mempGloble);
    mempGloble = NULL;
}
/************************************************************************* 
Function name: asf_memp_dump

Function Description:
    Print all ASF Memory Pool detail informations to screen.

Parameters:
    
Return:

*************************************************************************/
void asf_memp_dump()
{
    u_int32_t index;
    //asf_memp_list_node *node;
    struct asf_memp_tbl *tbl;
    if(mempGloble == NULL)
    {
        asl_printf("ASF Memory Pool is EMPTY\n");
        return;
    }
    asl_printf("ASF Memory Pool:\n");
    asl_printf("\tBlock Size:");
    for(index = 0; index < mempGloble->count; index++)
    {
        tbl = &(mempGloble->tbls[index]);
        asl_printf("%8u", tbl->blkSize);
    }
    asl_printf("\n\tFree Count:");
    for(index = 0; index < mempGloble->count; index++)
    {
        tbl = &(mempGloble->tbls[index]);
        asl_printf("%8u", tbl->free);
    }
    asl_printf("\n\tUsed Count:");
    for(index = 0; index < mempGloble->count; index++)
    {
        tbl = &(mempGloble->tbls[index]);
        asl_printf("%8u", tbl->used);
    }
    asl_printf("\n\n");
    #if 0
    for(index = 0; index < mempGloble->count; index++)
    {
        tbl = &(mempGloble->tbls[index]);
        asl_printf("BlockSize %u", tbl->blkSize);
        asl_printf("\tFree:%u", tbl->free);
        asl_printf("\tUsed:%u\n", tbl->used);
        #if 0
        asl_printf("\tFree Pointers:");
        for(node = tbl->freePtr->head; node != NULL; node = node->next)
        {
            asl_printf(" %p", node->ptr);
        }
        asl_printf("\n\tUsed Pointers:");
        for(node = tbl->usedPtr->head; node != NULL; node = node->next)
        {
            asl_printf(" %p", node->ptr);
        }
        asl_printf("\n\n");
        #endif
    }
    #endif
}
