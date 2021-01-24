#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asl_time.h"
#include "asf_shell.h"


#define asl_write(fd, buf, len) fwrite(buf, len, 1, fd)
#define asl_sh_write(buf, len) asl_write(stdout, buf, len)
#define asl_sh_newline() asl_write(stdout, "\r\n", 3)
#define asl_sh_puts(str) asl_write(stdout, str, asl_strtlen(str))

#define asl_read(fd, buf, len) fread(buf, len, 1, fd)
#define asl_sh_read(buf, len) asl_read(stdin, buf, len)

void asl_sh_ttyset(int Reset)
{
    static struct termio old;
    struct termio        arg;

    if (Reset == 0) 
    {
        (void)ioctl(0, TCGETA, &old);

        arg = old;
        arg.c_lflag &= ~(ECHO | ICANON);
        arg.c_iflag &= ~(ISTRIP | INPCK);
        arg.c_cc[VMIN] = 1;
        arg.c_cc[VTIME] = 0;
        (void)ioctl(0, TCSETAW, &arg);
    } 
    else 
    {
        (void)ioctl(0, TCSETAW, &old);
    }
}


static void asl_sh_put_prompt(struct asl_sh *as)
{
    u_int8_t prompt[128] = {0};
    sprintf((char*)prompt, "%s> ", as->name);
    asl_sh_puts(prompt);
}
static void asl_sh_ops_close(struct asl_sh *as, u_int8_t* buf, s_int32_t len)
{
    asl_sh_newline();
    as->newClient = TRUE;
}
void asl_sh_close_client(struct asl_sh *as)
{
    asl_sh_ops_close(as, NULL, 0);
}

static void asl_sh_ops_backspace(struct asl_sh *as, u_int8_t* buf, s_int32_t len)
{
    if(as->inSize != 0)
    {
        as->inSize--;
        as->in[as->inSize] = '\0';
    }
}
        
static void asl_sh_ops_complete(struct asl_sh *as, u_int8_t* buf, s_int32_t len)
{
    as->in[as->inSize++] = buf[0];
    as->inSize = asl_strtlen(as->in);
    as->outSize = as->cFunc(as);
    if(as->outSize == 2 && (*(as->out))[0] == ASL_SH_COMLEMENTED)
    {
        /* Complete current command */
        asl_free(*(as->out));
        *(as->out) = NULL;
        asl_sh_newline();
        asl_sh_put_prompt(as);
        asl_sh_write(as->in, asl_strtlen(as->in));
        as->inSize = asl_strlen(as->in);
    }
    else if(as->outSize != 0)
    {
        /* String matches more than one command */
        asl_sh_newline();
        asl_sh_write(*(as->out), as->outSize);
        asl_free(*(as->out));
        *(as->out) = NULL;
        asl_sh_put_prompt(as);
        as->in[as->inSize - 2] = '\0';
        as->inSize -= 2;
        asl_sh_write(as->in, asl_strtlen(as->in));
    }
    else
    {
        /* No match command */
        as->in[as->inSize - 2] = '\0';
        as->inSize -= 2;
    }
}
static void asl_sh_ops_exec(struct asl_sh *as, u_int8_t* buf, s_int32_t len)
{
    as->inSize = asl_strtlen(as->in);
    as->outSize = as->opFunc(as);
    if(!as->newClient)
    {
        if(as->outSize != 0)
        {
            asl_sh_newline();
            asl_sh_write(*(as->out), as->outSize);
            asl_free(*(as->out));
            *(as->out) = NULL;
        }
        asl_sh_newline();
        asl_sh_put_prompt(as);
    }
    asl_memset(as->in, 0, as->inSize);
    as->inSize = 0;
}
static void asl_sh_ops_help(struct asl_sh *as, u_int8_t* buf, s_int32_t len)
{
    as->in[as->inSize++] = buf[0];
    asl_sh_write(buf, len);
    asl_sh_newline();
    as->outSize = as->hFunc(as);
    asl_sh_write(*(as->out), as->outSize);
    if(as->outSize >= 2 && (*(as->out))[as->outSize - 2] != '\n')
        asl_sh_newline();
    asl_free(*(as->out));
    *(as->out) = NULL;
    
    /* delete '?' and '\0' */
    as->inSize = asl_strtlen(as->in);
    as->inSize -= 2;
    as->in[as->inSize] = '\0';
    //asl_sh_newline();
    asl_sh_put_prompt(as);
    asl_sh_write(as->in, as->inSize);
}

void asl_sh_default_ops_func(struct asl_sh *as, u_int8_t* buf, s_int32_t len)
{
    as->in[as->inSize++] = buf[0];
    asl_sh_write(buf, len);
}

static ASL_SH_OPS_FUNC asl_sh_get_ops_func(u_int8_t ch)
{
    struct
    {
        u_int8_t ch;
        void(*func)(struct asl_sh*, u_int8_t*, s_int32_t);
    }asl_sh_ops[] = 
    {
        {0x7f,                    asl_sh_ops_backspace},
        {'\t',                    asl_sh_ops_complete},
        {'\n',                    asl_sh_ops_exec},
        {'\r',                    asl_sh_ops_exec},
        {'?',                     asl_sh_ops_help}
    };
    u_int32_t index;
    for(index = 0; index < ASL_COUNTOF(asl_sh_ops); index++)
    {
        if(ch == asl_sh_ops[index].ch)
            return asl_sh_ops[index].func;
    }
    return asl_sh_default_ops_func;
}

static void asl_sh_loop(void *arg)
{
    struct asl_sh *as = (struct asl_sh*)arg;
    s_int32_t rlen = 0;
    as->inSize = 0;
    u_int8_t buf[1] = {0};
    asl_print_dbg("ASL Shell Start!!!");
    asl_sh_newline();
    asl_sh_put_prompt(as);
    while(TRUE)
    {
        if(as->newClient)
        {
            asl_sh_ttyset(1);
            system("/bin/bash");
            asl_sh_ttyset(0);
            as->newClient = FALSE;
            asl_sh_newline();
            asl_sh_put_prompt(as);
        }
        asl_memset(buf, 0, 1);
        rlen = asl_sh_read(buf, 1);
        if(rlen <= 0)
            break;
        //asl_print_hex(buf, rlen, "Shell Read");
        (asl_sh_get_ops_func(buf[0]))(as, buf, rlen);
    }
}

struct asl_sh* asl_sh_create(void *opFunc, void *hFunc, void *cFunc, char* name)
{
	assert(opFunc && hFunc && cFunc);
    struct asl_sh *as = ASL_MALLOC_T(struct asl_sh);
    const char* nstr = (name == NULL ? ASL_SH_DEFAULT_NAME : name);
    as->name = (char*)asl_malloc(asl_strtlen(nstr));
    asl_memcpy(as->name, nstr, asl_strlen(nstr));
    as->opFunc = (ASL_SH_FUNC)opFunc;
    as->hFunc = (ASL_SH_FUNC)hFunc;
    as->cFunc = (ASL_SH_FUNC)cFunc;
    as->newClient = FALSE;
    as->in = (u_int8_t*)asl_malloc(8192);
    as->out = (u_int8_t**)asl_malloc(sizeof(u_int8_t*));
    asl_sh_ttyset(0);
    asl_thread_create(&as->thr, asl_sh_loop, as);
    
    return as;
}

result_t asl_sh_destroy(struct asl_sh **asp)
{
    if(asp != NULL && *asp != NULL)
    {
        struct asl_sh *as = *asp;
        asl_thread_cancel(&as->thr);
        asl_thread_join(&as->thr);
        if(*(as->out) != NULL)
            asl_free(*(as->out));
        asl_free(as->out);
        asl_free(as->in);
        asl_free(as->name);
        asl_free(as);
        *asp = NULL;
        asl_sh_ttyset(1);
    }
    
    return SUCCESS;
}
