#ifndef ASL_SYSCMD_H
#define ASL_SYSCMD_H

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>


struct asl_syscmd
{
    char* cmd;
    char* retBuf;
    u_int32_t retSize;
};
result_t asl_syscmd_sync();
result_t asl_syscmd_exec(struct asl_syscmd *aslSyscmd);

#endif /* ASL_SYSCMD_H */
