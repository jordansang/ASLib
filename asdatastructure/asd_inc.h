#ifndef ASD_INC_H
#define ASD_INC_H

#include "asd_array.h"
#include "asd_btree.h"
#include "asd_fifo.h"
#include "asd_list.h"
#include "asd_map.h"
#include "asd_stream.h"
#include "asd_string.h"
#include "asd_tree.h"

/* asd_array Simplification */
typedef struct asd_array array;
#define array_create(len) asd_array_create(len)
#define array_put(arr, item, index) asd_array_put(arr, item, index)
#define array_get(arr, index, target) asd_array_get(arr, index, target)
#define array_append(arr, items, count) asd_array_append(arr, items, count)
#define array_last(arr, count, target) asd_array_last(arr, count, target)
#define array_clear(arr) asd_array_clear(arr)
#define array_destroy(arrp) asd_array_destroy(arrp)

/* asd_btree Simplification */
typedef struct asd_btree_item btree_item;
typedef struct asd_btree btree;
#define btree_create() asd_btree_create()
#define btree_insert(t, pdata, data) asd_btree_insert(t, pdata, data)
#define btree_remove(t, pdata, data) asd_btree_remove(t, pdata, data)
#define btree_destroy(t) asd_btree_destroy(t)
#define btree_get_height(t) asd_btree_get_height(t)
#define btree_dump(t) asd_btree_dump(t)
#define btree_dump_test(h) asd_btree_dump_test(h)

/* asd_fifo Simplification */
typedef struct asd_fifo_item fifo_item;
typedef struct asd_fifo fifo;
#define fifo_create() asd_fifo_create()
#define fifo_enque(f, data) asd_fifo_enque(f, data)
#define fifo_deque(f) asd_fifo_deque(f)
#define fifo_count(f) asd_fifo_count(f)
#define fifo_is_empty(f) asd_fifo_is_empty(f)
#define fifo_destroy(f) asd_fifo_destroy(f)
#define fifo_dump(f) asd_fifo_dump(f)

/* asd_list Simplification */
typedef struct asd_list_item list_item;
typedef struct asd_list list;
#define list_create() asd_list_create()
#define list_add(l, data) asd_list_add(l, data)
#define list_add_no_dup(l, data) asd_list_add_no_dup(l, data)
#define list_add_before(l, cur, data) asd_list_add_before(l, cur, data)
#define list_add_after(l, cur, data) asd_list_add_after(l, cur, data)
#define list_add_sort(l, data) asd_list_add_sort(l, data)
#define list_head(l) asd_list_head(l)
#define list_tail(l) asd_list_tail(l)
#define list_del_tail(l) asd_list_del_tail(l)
#define list_del_head(l) asd_list_del_head(l)
#define list_del_item(l, data) asd_list_del_item(l, data)
#define list_del_all(l) asd_list_del_all(l)
#define list_destroy(l) asd_list_destroy(l)
#define list_add_list(src, dst) asd_list_add_list(src, dst)
#define list_get_list(l, val) asd_list_get_list(l, val)
#define list_dump(l) asd_list_dump(l)
#define list_item_is_exist(l, data) asd_list_item_is_exist(l, data)
#define list_get_val(l, data) asd_list_get_val(l, data)
#define LIST_LOOP(L, V, N) ASD_LIST_LOOP(L, V, N)

/* asd_map Simplification */
typedef struct asd_map_item map_item;
typedef struct asd_map map;
#define map_create() asd_map_create()
#define map_insert(map, key, data) asd_map_insert(map, key, data)
#define map_remove(map, key) asd_map_remove(map, key)
#define map_destroy(map) asd_map_destroy(map)
#define map_get_data(map, key) asd_map_get_data(map, key)

/* asd_stream Simplification */
typedef struct asd_stream stream;
#define stream_create(size) asd_stream_create(size)
#define stream_put(s, val, size) asd_stream_put(s, val, size)
#define stream_put8(s, val) asd_stream_put_char(s, val)
#define stream_put16(s, val) asd_stream_put_short(s, val)
#define stream_put32(s, val) asd_stream_put_int(s, val)
#define stream_put64(s, val) asd_stream_put_longlong(s, val)
#define stream_resize(s, size) asd_stream_resize(s, size)
#define stream_clear(s) asd_stream_clear(s)
#define stream_take(s, target, size) asd_stream_take(s, target, size)
#define stream_take8(s, t) asd_stream_take_char(s, t)
#define stream_take16(s, t) asd_stream_take_short(s, t)
#define stream_take32(s, t) asd_stream_take_int(s, t)
#define stream_take64(s, t) asd_stream_take_longlong(s, t)
#define stream_set_takepos(s, pos) asd_stream_set_takepos(s, pos)
#define stream_reset_takepos(s) asd_stream_reset_takepos(s)
#define stream_set_putpos(s, pos) asd_stream_set_putpos(s, pos)
#define stream_reset_putpos(s) asd_stream_reset_putpos(s)
#define stream_destroy(s) asd_stream_destroy(s)
#define stream_get_data(s) asd_stream_get_data(s)
#define STREAM_DATA(s) ASD_STREAM_DATA(s)
#define STREAM_DSIZE(s) ASD_STREAM_DSIZE(s)

/* asd_tree Simplification */
typedef struct asd_tree_item tree_item;
typedef struct asd_tree tree;
#define tree_get_item(tree, data) asd_tree_get_item(tree, data)
#define tree_add(tree, parent, data) asd_tree_add(tree, parent, data)
#define tree_del(tree, parent, data) asd_tree_del(tree, parent, data)
#define tree_create() asd_tree_create()
#define tree_destroy(tree) asd_tree_destroy(tree)

/* asd_string Simplification */
typedef struct asd_str string;
#define str_create() asd_str_create()
#define str_destroy(str) asd_str_destroy(str)
#define str_putf(str, fmt, ...) asd_str_putf(str, fmt, ##__VA_ARGS__)
#define str_puts(str, src) asd_str_putf(str, "%s", src)
#define str_clear(str) asd_str_clear(str)

#endif /* ASD_INC_H */
