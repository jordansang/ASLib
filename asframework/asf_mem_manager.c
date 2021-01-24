/* Created By JS. Contact with <sanghe@163.com> */

/*************************************************************************
Filename: asf_mem_manager.c

File Description: 
    Supply Memory Manager Functions. It will display all memory using 
    informations.

Created by: JS

Create Date: 2018/04/02

Modified by:

Modified Date:

Modification Description:

*************************************************************************/
#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_time.h"
#include "asl_memory.h"
#include "asf_mem_pool.h"
#include "asf_mem_manager.h"
/* Global Memory types */
struct asf_memg *memgGlobal = NULL;
u_int32_t asf_memg_get_upper_size(u_int32_t size)
{
    u_int32_t index;
    for(index = 0; index < 32; index++)
    {
        if(size <= (1 << index))
        {
            break;
        }
    }
    if(index != 32)
    {
        return (1 << index);
    }
    return (-1);
}

/************************************************************************* 
Function name: asf_memg_check_mtype

Function Description:
    Check if Memory Type has already been add to Memory Type Container.

Parameters:
    
Return:
    TRUE
    FALSE

*************************************************************************/
char* asf_memg_get_mtype_desc(u_int32_t type)
{
    u_int32_t index;
    
    for(index = 0; index < memgGlobal->mInfo->count; index++)
    {
        if(type == memgGlobal->mInfo->items[index].type)
        {
            return memgGlobal->mInfo->items[index].desc;
        }
    }
    return NULL;
}
/************************************************************************* 
Function name: asf_memg_check_mtype

Function Description:
    Check if Memory Type has already been add to Memory Type Container.

Parameters:
    
Return:
    TRUE
    FALSE

*************************************************************************/
result_t asf_memg_check_mtype(u_int32_t type)
{
    u_int32_t index;
    
    for(index = 0; index < memgGlobal->mInfo->count; index++)
    {
        if(type == memgGlobal->mInfo->items[index].type)
        {
            return TRUE;
        }
    }
    return FALSE;
}
/************************************************************************* 
Function name: asf_memg_alloc

Function Description:
    Allocate a Type Memory Block with target size.

Parameters:
    type            -- memory block type
    size            -- memory block size
    
Return:
    pointer to the memory block
    NULL

*************************************************************************/
void* asf_memg_alloc(u_int32_t type, u_int32_t size)
{
    void *ptr = NULL;
    struct asf_memg_item *item;
    if(asf_memg_check_mtype(type) != TRUE)
    {
        asl_print_warn("Not Enrolled Memory Type %d", type);
        type = ASF_MEMG_MTYPE;
    }
    
    ptr = asf_memp_alloc(size);
    if(ptr != NULL)
    {
        item = (struct asf_memg_item*)asl_malloc(sizeof(struct asf_memg_item));
        item->size = asf_memg_get_upper_size(size);
        item->ptr = ptr;
        item->ope = ASF_MEMG_OPE_SAVE;
        item->type = type;
        item->freeT = 0;
        item->allocT = asl_time_get_stamp();

        //asl_print_dbg("Alloc Mtype:%s Ptr:%p size:%u Stamp:%lu", asf_memg_get_mtype_desc(type), ptr, item->size, item->allocT);
        
        asd_list_add(memgGlobal->list, item);
    }

    return (ptr);
}
/************************************************************************* 
Function name: asf_memg_free

Function Description:
    Free Target Memory Block and modify the free time.

Parameters:
    ptr         -- memory block pointer
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
result_t asf_memg_free(void *ptr)
{
    struct asf_memg_item *item;
    struct asd_list_item *li = NULL;
    ASD_LIST_LOOP(memgGlobal->list, item, li)
    {
        if(item->ptr == ptr)
        {
            //asl_print_dbg("Free Mtype:%s Ptr:%p size:%u Stamp:%lu", asf_memg_get_mtype_desc(item->type), ptr, item->size, item->allocT);
            item->ptr = 0;
            item->freeT = asl_time_get_stamp();
            
            return asf_memp_free(ptr);
        }
    }

    asl_print_warn("Memory Block %p is not created by ASF Memory Manager", ptr);
    return FAILURE;
}
/************************************************************************* 
Function name: asf_memg_realloc

Function Description:

Parameters:
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
void* asf_memg_realloc(void* ptr, u_int32_t size)
{
    void *nptr = NULL;
    struct asf_memg_item *item;
    struct asf_memg_item *nItem;
    //asl_print_dbg("Memory Manager Realloc for pointer %p to size %u", ptr, size);
    struct asd_list_item *li = NULL;
    ASD_LIST_LOOP(memgGlobal->list, item, li)
    {
        if(item->ptr == ptr)
        {
            //asl_print_dbg("Get Pointer %p to Realloc", item->ptr);
            item->ptr = 0;
            item->freeT = asl_time_get_stamp();
            
            nptr = asf_memp_realloc(ptr, size);
            
            if(nptr != NULL)
            {
                //asl_print_dbg("Realloc Pointer %p to %p", ptr, nptr);
                nItem = (struct asf_memg_item*)asl_malloc(sizeof(struct asf_memg_item));
                nItem->ptr = nptr;
                nItem->ope = ASF_MEMG_OPE_SAVE;
                nItem->type = item->type;
                nItem->freeT = 0;
                nItem->allocT = asl_time_get_stamp();

                asd_list_add(memgGlobal->list, nItem);
            }
            return (nptr);
        }
    }

    asl_print_warn("Memory Block %p is not created by ASF Memory Manager", ptr);
    return NULL;
}
/************************************************************************* 
Function name: asf_memg_list_cmp

Function Description:
    ASF Memory Manager List node compare funcion. This function will return
    different values, but, now asd_list can't support parts of these values.

Parameters:
    src         -- asd_list node, struct asf_memg_item
    dst         -- target info, struct asf_memg_item
    
Return:
    ASF_MEMG_CMP_NE     -- unequal
    ASF_MEMG_CMP_E      -- equal
    ASF_MEMG_CMP_S      -- smaller
    ASF_MEMG_CMP_L      -- larger

*************************************************************************/
result_t asf_memg_list_cmp(void *src, void* dst)
{
    struct asf_memg_item *srcItem = (struct asf_memg_item*)src;
    struct asf_memg_item *dstItem = (struct asf_memg_item*)dst;

    switch(dstItem->ope)
    {
        case ASF_MEMG_OPE_TYPE:
            //asl_print_dbg("Type:%u vs %u", srcItem->type, dstItem->type);
            return (srcItem->type == dstItem->type);
        case ASF_MEMG_OPE_SIZE:
            return (srcItem->size == dstItem->size);
        case ASF_MEMG_OPE_ALLOCT:
            if(srcItem->allocT == dstItem->allocT)
            {
                return ASF_MEMG_CMP_E;
            }
            else
                return (srcItem->allocT > dstItem->allocT ? ASF_MEMG_CMP_L : ASF_MEMG_CMP_S);
        case ASF_MEMG_OPE_FREET:
            if(srcItem->allocT == dstItem->allocT)
            {
                return ASF_MEMG_CMP_E;
            }
            else
                return (srcItem->allocT > dstItem->allocT ? ASF_MEMG_CMP_L : ASF_MEMG_CMP_S);
        default:
            asl_print_err("Illigal type of Memory Manager Operation");
            break;
    }

    return ASF_MEMG_CMP_NE;
}
/************************************************************************* 
Function name: asf_memg_list_del

Function Description:
    ASF Memory Manager Delete List node function.

Parameters:
    val             -- asd_list node, struct asf_memg_item
    
Return:


*************************************************************************/
void asf_memg_list_del(void *val)
{
    asl_free(val);
}
/************************************************************************* 
Function name: 

Function Description:

Parameters:
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
void asf_memg_dump(u_int8_t ope)
{
    u_int32_t index;
    struct asd_list* typeList;
    struct asf_memg_item target;
    struct asf_memg_item *typeItem = NULL;
    struct asd_list* dumpList = NULL;
    struct asf_memg_dump_item *dumpItem = NULL;
    if(ope == ASF_MEMG_OPE_SIZE)
    {
        return asf_memp_dump();
    }
    else
    {
        asl_println("ASF Memory Manager. Created By JS <sanghe@163.com>");
        target.ope = ASF_MEMG_OPE_TYPE;
        for(index = 0; index < memgGlobal->mInfo->count; index++)
        {
            target.type = memgGlobal->mInfo->items[index].type;
            //asl_print_dbg("target.type:%u", target.type);
            typeList = asd_list_get_list(memgGlobal->list, &target);
            if(typeList == NULL)
            {
                asl_printf("\tMemory Type:%s has not alloc memory yet\n", memgGlobal->mInfo->items[index].desc);
                continue;
            }
            //asl_print_dbg("typeList:%p", (char*)typeList);
            typeList->cmpFunc = NULL;
            typeList->delFunc = NULL;
            asl_printf("\tMemory Type:%s\n", memgGlobal->mInfo->items[index].desc);
            //asl_printf("\t%10s%10s%10s%10s\n", "Size", "Count", "Using", "Freed");
            dumpList = asd_list_create();
            dumpList->delFunc = NULL;
            struct asd_list_item *li = NULL;
            struct asd_list_item *li1 = NULL;
            ASD_LIST_LOOP(typeList, typeItem, li)
            {
                //asl_print_dbg("TypeItem: %s %p %u %lu %lu", 
                //    asf_memg_get_mtype_desc(typeItem->type), typeItem->ptr, 
                //    typeItem->size, typeItem->allocT, typeItem->freeT);
                ASD_LIST_LOOP(dumpList, dumpItem, li1)
                {
                    if(typeItem->size == dumpItem->size)
                    {
                        break;
                    }
                }
                
                if(dumpItem == NULL)
                {
                    dumpItem = (struct asf_memg_dump_item*)asl_malloc(sizeof(struct asf_memg_dump_item));
                    dumpItem->size = typeItem->size;
                    dumpItem->aCount = dumpItem->fCount = dumpItem->count = 0;
                    asd_list_add(dumpList, dumpItem);
                }
                //asl_print_dbg("dumpItem: %u %u %u %u", 
                //    dumpItem->size, dumpItem->count, dumpItem->aCount, dumpItem->fCount);
                dumpItem->count++;
                if(typeItem->freeT == 0)
                {
                    dumpItem->aCount++;
                }
                else
                {
                    dumpItem->fCount++;
                }
            }
            asl_printf("\t%5s", "Size");
            ASD_LIST_LOOP(dumpList, dumpItem, li)
            {
                asl_printf("%10u", dumpItem->size);
            }
            asl_printf("\n");
            asl_printf("\t%5s", "Count");
            ASD_LIST_LOOP(dumpList, dumpItem, li)
            {
                asl_printf("%10u", dumpItem->count);
            }
            asl_printf("\n");
            asl_printf("\t%5s", "Using");
            ASD_LIST_LOOP(dumpList, dumpItem, li)
            {
                asl_printf("%10u", dumpItem->aCount);
            }
            asl_printf("\n");
            asl_printf("\t%5s", "Freed");
            ASD_LIST_LOOP(dumpList, dumpItem, li)
            {
                asl_printf("%10u", dumpItem->fCount);
            }
            asl_printf("\n");
            //ASD_LIST_LOOP(dumpList, dumpItem, li)
            //{
            //    asl_printf("\t%10u%10u%10u%10u\n", dumpItem->size, dumpItem->count, dumpItem->aCount, dumpItem->fCount);
            //}
            asl_printf("\n");
            asd_list_destroy(typeList);
            asd_list_destroy(dumpList);
        }
    }
}
/************************************************************************* 
Function name: asf_memg_create

Function Description:
    Initialize Memory Manager with Memory Types.

Parameters:
    info            -- memory types informations
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
result_t asf_memg_create(struct asf_memg_minfo *info)
{
    if(info == NULL)
    {
        asl_print_err("Memory Manager Information must be given");
        return FAILURE;
    }

    memgGlobal = (struct asf_memg*)asl_malloc(sizeof(struct asf_memg));
    
    memgGlobal->mInfo = (struct asf_memg_minfo*)asl_malloc(sizeof(struct asf_memg_minfo));
    memgGlobal->mInfo->count = info->count;
    memgGlobal->mInfo->items = (struct asf_memg_minfo_item*)asl_malloc((info->count)*(sizeof(struct asf_memg_minfo_item)));
    asl_memcpy(memgGlobal->mInfo->items, info->items, (info->count)*(sizeof(struct asf_memg_minfo_item)));

    memgGlobal->binfo = (struct asf_memg_binfo*)asl_malloc(sizeof(struct asf_memg_binfo));
    memgGlobal->binfo->count = 0;
    memgGlobal->binfo->size = NULL;
    
    memgGlobal->list = asd_list_create();
    memgGlobal->list->cmpFunc = asf_memg_list_cmp;
    memgGlobal->list->delFunc = asf_memg_list_del;

    return SUCCESS;
}
/************************************************************************* 
Function name: asf_memg_destroy

Function Description:
    Release all memory allocated by Memory Manager.

Parameters:
    
Return:
    SUCCESS

*************************************************************************/
result_t asf_memg_destroy()
{
    asd_list_destroy(memgGlobal->list);
    asl_free(memgGlobal->mInfo->items);
    asl_free(memgGlobal->mInfo);
    asl_free(memgGlobal->binfo->size);
    asl_free(memgGlobal->binfo);
    asl_free(memgGlobal);
    
    return SUCCESS;
}
