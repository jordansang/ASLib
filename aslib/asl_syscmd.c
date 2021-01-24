#include "asl_type.h"
#include "asl_stdio.h"
#include "asl_syscmd.h"

result_t asl_syscmd_sync()
{
    sync();
    return SUCCESS;
}
result_t asl_syscmd_exec(struct asl_syscmd *aslSyscmd)
{
    result_t ret = SUCCESS;
    int pipefd[2];
    int count;
    if(aslSyscmd == NULL || aslSyscmd->cmd == NULL)
    {
        asl_print_err("Invalid Command");
        return FAILURE;
    }
    /* If the command not need return, use system() to execute the command */
    if(aslSyscmd->retBuf == NULL || aslSyscmd->retSize <= 0)
    {
        //asl_printf("@@@@@@@Execute \"%s\" Command\n", aslSyscmd->cmd);
        ret = system(aslSyscmd->cmd);
        if(ret != -1 && WIFEXITED(ret) && 0 == WEXITSTATUS(ret))
        {
            //asl_print_dbg("Execute:\"%s\" SUCCESS", aslSyscmd->cmd);
            ret = SUCCESS;
        }
        else
        {
            asl_print_err("Execute:\"%s\" FAILED %s", aslSyscmd->cmd, strerror(WEXITSTATUS(ret)));
            ret = FAILURE;
        }
        //asl_printf("@@@@@@@Finish \"%s\" Command\n\n", aslSyscmd->cmd);
        return ret;
    }

    /* Create the pipe for communicate */
    if(pipe(pipefd) == -1)
    {
        asl_print_err("Create Pipe fd failed");
        return FAILURE;
    }

    pid_t pid;
    pid = fork();

    if(pid == -1)
    {
        asl_print_err("Create child process failed");
        return FAILURE;
    }

    if(pid == 0)
    {
        //asl_print_dbg("Execute command \"%s\" in execl()    --- child process", aslSyscmd->cmd);
        /* Child process, execute command here */
        /* Redesignate the output pipefd to stdout */
        dup2(pipefd[1], STDOUT_FILENO);
        /* Child process won't operate the pipefd */
        close(pipefd[1]);
        close(pipefd[0]);
        /* Use execl() to execute the system command */
        execl("/bin/sh","sh","-c", aslSyscmd->cmd, NULL);
        /* Handle the process to execl, we shuold not get it again */
        asl_print_err("error execute ls\n");
        return FAILURE;
    }
    //be_print_dbg("Read command \"%s\" return buffer  --- parent process", cmd);
    /* Parent process, get the return buffer here */
    close(pipefd[1]);
    /* Parent process will read from the other side of pipefd */
    count = read(pipefd[0], aslSyscmd->retBuf, aslSyscmd->retSize);
    if(count < 0)
    {
        asl_print_err("Read error");
        return FAILURE;
    }
    //asl_print_dbg("Get return buffer \"%s\" --- parent process", aslSyscmd->retBuf);
    close(pipefd[0]);
    return SUCCESS;
}
