#include "asl_inc.h"
#include "asd_map.h"

/*
struct asd_map *asd_map_create();
result_t asd_map_insert(struct asd_map *map, void *key, void* data);
result_t asd_map_remove(struct asd_map *map, void *key);
result_t asd_map_destroy(struct asd_map *map);
void* asd_map_get_data(struct asd_map *map, void *key);
*/

s_int32_t test_map_key_cmp_func(void *a, void *b)
{
    s_int32_t *aval = (s_int32_t*)a;
    s_int32_t *bval = (s_int32_t*)b;

    if(*aval == *bval)
        return 0;
    else
        return (*aval < *bval ? -1 : 1);
}

void test_map_key_del_func(void *a)
{
    return;
}

void test_map_data_del_func(void *a)
{
    return;
}

#define TEST_MAP_KEY_COUNT 4000
#define TEST_MAP_TEST_COUNT 4000

void test_map()
{
    struct asd_map *map = asd_map_create();
    if(!map)
    {
        asl_print_err("Create Map FAILED");
    }
    map->keyCmpFunc = test_map_key_cmp_func;
    map->keyDelFunc = test_map_key_del_func;
    map->dataDelFunc = test_map_data_del_func;
    //s_int32_t keys[TEST_MAP_KEY_COUNT];
    //s_int32_t datas[TEST_MAP_KEY_COUNT];
    s_int32_t *keys = (s_int32_t*)asl_malloc(TEST_MAP_KEY_COUNT*sizeof(s_int32_t));
    s_int32_t *datas = (s_int32_t*)asl_malloc(TEST_MAP_KEY_COUNT*sizeof(s_int32_t));
    s_int32_t index;
    s_int32_t choice;
    u_int64_t start, end;
    
    for(index = 0; index < TEST_MAP_KEY_COUNT; index++)
    {
        keys[index] = index;
        datas[index] = rand();
        asd_map_insert(map, &keys[index], &datas[index]);
    }
    
    start = asl_time_get_stamp();
    for(index = 0; index < TEST_MAP_TEST_COUNT; index++)
    {
        //choice = rand()%TEST_MAP_KEY_COUNT;
        choice = -1;
        asd_map_get_data(map, &keys[choice]);
        //asl_print_dbg("No.%02d: [%d:%d] vs [%d:%d]", index, 
        //            keys[choice], datas[choice], keys[choice], 
        //            *(s_int32_t*)asd_map_get_data(map, &keys[choice]));
    }
    end = asl_time_get_stamp();
    asl_print_dbg("Find %d Items to Map consum: %lu", TEST_MAP_TEST_COUNT, end - start);
    asd_map_destroy(map);
    return;
}
