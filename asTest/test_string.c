#include "asl_inc.h"
#include "asd_inc.h"


void test_string()
{
    string *str = str_create();
    char* src1 = "Test";
    char* src2 = "For";
    char* src3 = "Test String";

    str_puts(str, src1);
    asl_print_dbg("%s", ASD_STR_CONTENT(str));

    str_puts(str, src2);
    asl_print_dbg("%s", ASD_STR_CONTENT(str));

    str_puts(str, src3);
    asl_print_dbg("%s", ASD_STR_CONTENT(str));

    str_putf(str, "%s %s %s!", src1, src2, src3);
    asl_print_dbg("%s", ASD_STR_CONTENT(str));

    str_destroy(&str);
}
