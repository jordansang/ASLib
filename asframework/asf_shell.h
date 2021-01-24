#ifndef ASF_SHELL_H
#define ASF_SHELL_H

#include <termio.h>
#include "asl_thread.h"

#define ASL_SH_DEFAULT_NAME   "Default"
#define ASL_SH_DEFAULT_REPLAY "NO OPFUNC"
#define ASL_SH_DEFAULT_HELP   "NO HELP in DEFAULT MODE"

/* Complemented inBuf */
#define ASL_SH_COMLEMENTED 0xfe

struct asl_sh;

typedef u_int32_t(*ASL_SH_FUNC)(struct asl_sh *as);

struct asl_sh
{
    char* name;
    struct asl_thread thr;
    bool newClient;
    ASL_SH_FUNC opFunc;
    void* opArg;
    ASL_SH_FUNC hFunc;
    void* hArg;
    ASL_SH_FUNC cFunc;
    void* cArg;
    
    u_int8_t* in;
    s_int32_t inSize;
    u_int8_t** out;
    s_int32_t outSize;
};

typedef void(*ASL_SH_OPS_FUNC)(struct asl_sh*, u_int8_t*, s_int32_t);

struct asl_sh* asl_sh_create(void *opFunc, void *hFunc, void *cFunc, char* name);
result_t asl_sh_destroy(struct asl_sh **asp);
#define asl_sh_is_new_client(as) (as->newClient)
void asl_sh_close_client(struct asl_sh *as);

#endif /* ASF_SHELL_H */
