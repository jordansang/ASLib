#ifndef ASL_STDIO_H
#define ASL_STDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <assert.h>

#include "asl_type.h"

#define ASL_SEPARATOR  '/'
        
#define asl_get_file_name(FULLPATH) \
            (strrchr(FULLPATH, ASL_SEPARATOR) ? (strrchr(FULLPATH, ASL_SEPARATOR) + 1) : FULLPATH)

#define asl_print_err(FMT,...) \
            fprintf(stderr,"[ERR]  %s(%d)[%s]: "FMT"\n", \
                        asl_get_file_name(__FILE__), __LINE__,__func__,##__VA_ARGS__)

#define asl_print_warn(FMT,...) \
            fprintf(stderr,"[ALM]  %s(%d)[%s]: "FMT"\n", \
                        asl_get_file_name(__FILE__), __LINE__,__func__,##__VA_ARGS__)

#define asl_print_dbg(FMT,...) \
            fprintf(stdout,"[DBG]  %s(%d)[%s]: "FMT"\n", \
                        asl_get_file_name(__FILE__), __LINE__,__func__,##__VA_ARGS__)
            
#define asl_printf(...) fprintf(stdout,__VA_ARGS__)
#define asl_println(...) fprintf(stdout, __VA_ARGS__"\n") 

void asl_print_hex(const void* data, int size, const char* desc);

#define ASL_COUNTOF(ARY) (sizeof(ARY)/sizeof((ARY)[0]))

char* asl_get_state_str(result_t state);
char* asl_get_ret_str(result_t ret);



#endif /* ASL_STDIO_H */
