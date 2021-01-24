#include "asl_type.h"
#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asl_file.h"
#include "asl_syscmd.h"

bool asl_path_is_exist(char* path)
{
    if(access(path, F_OK) == 0)
    {
        return TRUE;
    }
    return FALSE;
}

result_t asl_mkdir(char* path, u_int32_t mode)
{
    char dirName[256] = {0};
    char* p = path;
    char* pNext = NULL;
    
    while(pNext != path)
    {
        pNext = strchr(p + 1, '/');
        if(pNext != NULL)
        {
            strncpy(dirName, p, pNext - path);
            p = pNext + 1;
            pNext = dirName;
        }
        else
        {
            pNext = path;
        }
        //asl_print_dbg("dir:%s", pNext);
        if(asl_path_is_exist(pNext) != SUCCESS)
        {
            mkdir(pNext, mode);
        }
    }
    return SUCCESS;
}
char* asl_get_filelist(char* path, char* regexp)
{
    struct asl_syscmd syscmd;
    char cmd[256] = {0};
    char* filelist = NULL;
    if(regexp != NULL)
    {
        sprintf(cmd, "ls %s | grep -E \"%s\"", path, regexp);
    }
    else
    {
        sprintf(cmd, "ls %s | grep -E \"*\"", path);
    }
    syscmd.cmd = cmd;
    syscmd.retSize = 1024;
    syscmd.retBuf = (char*)asl_malloc(syscmd.retSize);
    
    if(access(path, F_OK) != 0)
    {
        asl_print_err("\"%s\" is NOT Exist", path);
        return NULL;
    }
    if(asl_syscmd_exec(&syscmd) != SUCCESS)
    {
        asl_print_err("Execute %s Command FAILED", cmd);
        return NULL;
    }
    filelist = (char*)asl_malloc(asl_strtlen(syscmd.retBuf));
    asl_memcpy(filelist, syscmd.retBuf, asl_strtlen(syscmd.retBuf));
    
    return filelist;
}
result_t asl_set_blockstate(struct asl_fd* fd)
{
    if(fd != NULL && fd->fd > 0)
    {
        int cur;

        if ((cur = fcntl(fd->fd, F_GETFL)) > 0)
        {
            cur = cur | (fd->blockstate == DISABLE ? O_NONBLOCK : (~O_NONBLOCK));
            if(fcntl(fd->fd, F_SETFL, cur) < 0)
            {
                asl_print_err("fcntl failed setting fd %d block state %s: %s", 
                    fd->fd, asl_get_state_str(fd->blockstate),strerror(errno));
                return FAILURE;
            }
        }
        return SUCCESS;
    }
    return FAILURE;
}
