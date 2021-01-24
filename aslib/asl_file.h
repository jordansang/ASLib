#ifndef ASL_FILE_H
#define ASL_FILE_H

#include <sys/stat.h>
#include <fcntl.h>

result_t asl_mkdir(char* path, u_int32_t mode);
char* asl_get_filelist(char* path, char* regexp);

struct asl_fd
{
    s_int32_t fd;
    u_int8_t blockstate;
};
result_t asl_set_blockstate(struct asl_fd* fd);


#endif /* ASL_FILE_H */
