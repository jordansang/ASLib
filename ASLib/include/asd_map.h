#ifndef ASD_MAP_H
#define ASD_MAP_H

#include "asd_list.h"

struct asd_map_item
{
    void* key;
    void* data;
    cmp_func keyCmpFunc;
    del_func keyDelFunc;
    del_func dataDelFunc;
};

struct asd_map
{
    u_int32_t size;
    struct asd_list* itemList;
    cmp_func keyCmpFunc;
    del_func keyDelFunc;
    del_func dataDelFunc;
};

struct asd_map *asd_map_create();
result_t asd_map_insert(struct asd_map *map, void *key, void* data);
result_t asd_map_remove(struct asd_map *map, void *key);
result_t asd_map_destroy(struct asd_map *map);
void* asd_map_get_data(struct asd_map *map, void *key);

#endif /* ASD_MAP_H */
