#include "asl_inc.h"
#include "asd_inc.h"

/*
typedef struct asd_array array;
//#define array_create(len) asd_array_create(len)
//#define array_put(arr, item, index) asd_array_put(arr, item, index)
//#define array_get(arr, index, target) asd_array_get(arr, index, target)
//#define array_append(arr, items, count) asd_array_append(arr, items, count)
//#define array_last(arr, count, target) asd_array_last(arr, count, target)
//#define array_clear(arr) asd_array_clear(arr)
#define array_destroy(arrp) asd_array_destroy(arrp)
*/

void test_array()
{
    array *carr = array_create(sizeof(char));
    char banner[] = "Test Array Banner !!!";
    char newTitle[] = "Carr";
    char append[] = "Append Information";
    char buf[128] = {0};
    char ch = 0;
    array_append(carr, banner, asl_strtlen(banner));
    array_last(carr, asl_strtlen(banner) - 5, buf);
    array_get(carr, 3, &ch);
    asl_print_dbg("1 Get Buf:%s. ch:%c", buf, ch);
    asl_memset(buf, 0, 128);
    ch = 0;
    
    int index;
    for(index = 0; index < 4; index++)
    {
        array_put(carr, &newTitle[index], index);
    }

    array_last(carr, asl_strtlen(banner), buf);
    array_get(carr, 3, &ch);
    asl_print_dbg("2 Get Buf:%s. ch:%c", buf, ch);
    asl_memset(buf, 0, 128);
    ch = 0;

    array_put(carr, &banner[4], asl_strtlen(banner) - 1);
    array_append(carr, append, asl_strtlen(append));
    array_last(carr, asl_strtlen(banner) + asl_strtlen(append), buf);
    array_get(carr, asl_strtlen(banner) + 3, &ch);
    asl_print_dbg("3 Get Buf:%s. ch:%c", buf, ch);
    asl_memset(buf, 0, 128);
    ch = 0;

    array_clear(carr);
    array_append(carr, append, asl_strtlen(append));
    array_last(carr, asl_strtlen(append), buf);
    array_get(carr, 3, &ch);
    asl_print_dbg("4 Get Buf:%s. ch:%c", buf, ch);
    asl_memset(buf, 0, 128);
    ch = 0;

    array_destroy(&carr);
}
