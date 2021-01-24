#include "asl_inc.h"
#include "asd_inc.h"
//#include "asf_log.h"

/*
** 1. Multiple ways to get log messages like shell, net, file .etc;
** 2. Take place of common output message like printf, send, write, fwrite .etc;
** 3. Minimum the consumption of system resources
** 4. Seperate with working threads
*/

/* 
** shell: NULL, NULL
** file: PATH, NAME
** net: IP, PORT
*/

/*
s_int32_t asf_log_file_create(const char* path);
s_int32_t asf_log_net_create(struct asl_net_info *info);
void asf_log(s_int32_t id, const char* fmt, ...);
*/

static void working_thr()
{
    string *time = str_create();
    str_puts(time, asl_time_get_ctime());
    asl_printf("[%s]", ASD_STR_CONTENT(time));
    str_destroy(&time);
}

void test_log()
{
    working_thr();
}
