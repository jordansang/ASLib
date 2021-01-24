/* Created By JS. Contact with <sanghe@163.com> */

/*************************************************************************
Filename: asd_map.c

File Description: 
    Datastruct map. It uses only one Prime Key to find all node in map.
    ASD Map supports both key-value and key-multi_values.

Created by: JS

Create Date: 2018/04/02

Modified by:

Modified Date:

Modification Description:

*************************************************************************/
#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asd_map.h"
static s_int32_t asd_map_default_key_cmp_func(void *a, void *b)
{
    return (a == b);
}
static s_int32_t asd_map_cmp_func(void *a, void *b)
{
    if(!a || !b)
        return (-1);
    struct asd_map_item *aitem = (struct asd_map_item*)a;
    struct asd_map_item *bitem = (struct asd_map_item*)b;

    cmp_func cmpFunc = aitem->keyCmpFunc ? aitem->keyCmpFunc : asd_map_default_key_cmp_func;
    return cmpFunc(aitem->key, bitem->key);
}
static void asd_map_del_func(void *val)
{
    struct asd_map_item *item = (struct asd_map_item*)val;
    if(!val)
        return;
    del_func keyDelFunc = item->keyDelFunc;
    del_func dataDelFunc = item->dataDelFunc;
    if(keyDelFunc)
        keyDelFunc(item->key);
    if(dataDelFunc)
        dataDelFunc(item->data);
    asl_free(item);
}
/************************************************************************* 
Function name: asd_map_create

Function Description:
    Create a new Map Container.

Parameters:
    cmp_func        -- Prime Key Comparation function
    
Return:
    map
    NULL

*************************************************************************/
struct asd_map *asd_map_create()
{
    struct asd_map *map = (struct asd_map*)asl_malloc(sizeof(struct asd_map));
    if(map == NULL)
    {
        return NULL;
    }
    map->size = 0;
    map->itemList= asd_list_create();
    map->itemList->cmpFunc = asd_map_cmp_func;
    map->itemList->delFunc = asd_map_del_func;
    map->keyCmpFunc = NULL;
    map->keyDelFunc = NULL;
    map->dataDelFunc = NULL;

    return map;
}
/************************************************************************* 
Function name: asd_map_insert

Function Description:
    Insert a new Map Pair to Target Map. The Key is unique, so this function
    will check if the same prime key is existed.

Parameters:
    map         -- target map
    key         -- prime key
    data        -- key pair data
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
result_t asd_map_insert(struct asd_map *map, void *key, void* data)
{
    if(map == NULL || key == NULL)
    {
        asl_print_err("Map or Prime Key is NULL");
        return FAILURE;
    }
    struct asd_map_item *item = (struct asd_map_item*)asl_malloc(sizeof(struct asd_map_item));
    if(!item)
        return FAILURE;
    item->key = key;
    item->data = data;
    if(asd_list_item_is_exist(map->itemList, item) == TRUE)
    {
        asl_print_err("Same Prime Key has existed in map");
        asl_free(item);
        return FAILURE;
    }
    item->keyCmpFunc = map->keyCmpFunc;
    item->keyDelFunc = map->keyDelFunc;
    item->dataDelFunc = map->dataDelFunc;
    asd_list_add(map->itemList, item);
    map->size++;
    return SUCCESS;
}
/************************************************************************* 
Function name: asd_map_remove

Function Description:
    Delete the map pair in target map with designated prime key.

Parameters:
    map         -- target map
    key         -- prime key
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
result_t asd_map_remove(struct asd_map *map, void *key)
{
    if(map == NULL || key == NULL)
    {
        asl_print_err("Map or Prime Key is NULL");
        return FAILURE;
    }

    struct asd_map_item item;
    item.key = key;
    if(asd_list_item_is_exist(map->itemList, &item) == FALSE)
    {
        asl_print_err("Not find the prime key in Map");
        return FAILURE;
    }

    asd_list_del_item(map->itemList, &key);
    map->size--;
            
    return SUCCESS;
}
/************************************************************************* 
Function name: asd_map_destroy

Function Description:
    Destroy the target map. This function will free all pairs in target map.

Parameters:
    map         -- target map
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
result_t asd_map_destroy(struct asd_map *map)
{
    if(map == NULL)
    {
        return SUCCESS;
    }
    asd_list_destroy(map->itemList);
    asl_free(map);
    map = NULL;
    return SUCCESS;
}
/************************************************************************* 
Function name: asd_map_get_data

Function Description:
    Get map data by target prime key in the map.

Parameters:
    map         -- target map
    key         -- prime key
    
Return:
    data
    NULL

*************************************************************************/
void* asd_map_get_data(struct asd_map *map, void *key)
{
    if(map == NULL || key == NULL)
    {
        asl_print_err("Map or Key is NULL");
        return NULL;
    }
    struct asd_map_item tmp;
    tmp.key = key;
    struct asd_map_item *item = (struct asd_map_item*)asd_list_get_val(map->itemList, &tmp);
    if(item == NULL)
    {
        asl_print_err("Not find the prime key in Map");
        return NULL;
    }
    return item->data;
}
