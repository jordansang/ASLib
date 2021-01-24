#include "asl_inc.h"
#include "asf_net_core.h"

struct asf_nc_rdata
{
    u_int32_t prio;
    struct asd_stream *s;
};

struct asf_nc_tdata
{
    u_int32_t prio;
    struct asl_net_info info;
    struct asd_stream *s;
};

static result_t asf_nc_net_info_check(u_int32_t ip, u_int16_t port)
{
    return SUCCESS;
}

static s_int32_t asf_nc_rdata_cmpFunc(void *a, void *b)
{
    struct asf_nc_rdata *aval, *bval;
    aval = (struct asf_nc_rdata*)a;
    bval = (struct asf_nc_rdata*)b;

    if(aval->prio == bval->prio)
        return 0;
    return (aval->prio > bval->prio ? 1 : -1);
}

static void asf_nc_rdata_delFunc(void *val)
{
    if(val == NULL)
    {
        asl_print_dbg("val is NULL");
        return;
    }
    struct asf_nc_rdata *and = (struct asf_nc_rdata*)val;
    asd_stream_destroy(&and->s);
    asl_free(and);
}

static s_int32_t asf_nc_tdata_cmpFunc(void *a, void *b)
{
    struct asf_nc_tdata *aval, *bval;
    aval = (struct asf_nc_tdata*)a;
    bval = (struct asf_nc_tdata*)b;

    if(aval->prio == bval->prio)
        return 0;
    return (aval->prio > bval->prio ? 1 : -1);
}

static void asf_nc_tdata_delFunc(void *val)
{
    if(val == NULL)
    {
        asl_print_dbg("val is NULL");
        return;
    }
    struct asf_nc_tdata *and = (struct asf_nc_tdata*)val;
    asd_stream_destroy(&and->s);
    asl_free(and);
}

static u_int32_t asf_nc_default_prioCkFunc(struct asd_stream *s)
{
    return 0;
}

static void asf_nc_default_procFunc(struct asd_stream *s)
{
    asl_print_hex(ASD_STREAM_DATA(s), ASD_STREAM_DSIZE(s), "Default ProcFunc Data");
}

static void asf_nc_recv_cancel(void *arg)
{
    asf_nc_rdata_delFunc(arg);
}

static void asf_nc_recv(struct asf_nc *anc)
{
    struct asf_nc_rdata *data = NULL;
    
    while(TRUE)
    {
        data = (struct asf_nc_rdata*)asl_malloc(sizeof(struct asf_nc_rdata));
        data->s = asd_stream_create(ASF_NET_CORE_MTU);
        asl_thread_cleanup_begin(asf_nc_recv_cancel, data);
        asl_thread_set_cancel_enable();
        data->s->dsize = asl_recv(anc->rsock, ASD_STREAM_DATA(data->s), ASF_NET_CORE_MTU);
        //asl_print_dbg(" recv data:%p data->s:%p", data, data->s);
        asl_thread_set_cancel_disable();
        asd_stream_set_putpos(data->s, ASD_STREAM_DSIZE(data->s));
        data->prio = anc->prioCkFunc(data->s);
        asl_print_dbg("rcv data priority:%u", data->prio);
        asl_sem_wait(&anc->rOpeSem);
        asd_list_add_sort(anc->rlist, data);
        asl_sem_post(&anc->rSem);
        asl_sem_post(&anc->rOpeSem);
        asl_thread_cleanup_end(ASL_THREAD_NOT_NEED_CLEANUP);
    }
    
}

static void asf_nc_proc(struct asf_nc *anc)
{
    struct asf_nc_rdata *data;
    while(TRUE)
    {
        asl_thread_set_cancel_enable();
        asl_sem_wait(&anc->rSem);
        asl_sem_wait(&anc->rOpeSem);
        asl_thread_set_cancel_disable();
        data = (struct asf_nc_rdata*)asd_list_tail(anc->rlist);
        asd_list_del_tail(anc->rlist);
        asl_sem_post(&anc->rOpeSem);
        asl_print_dbg("pro data priority:%u", data->prio);
        anc->func(data->s);
        asf_nc_rdata_delFunc(data);
    }
}

static void asf_nc_send(struct asf_nc *anc)
{
    struct asf_nc_tdata *data;
    struct asl_net_info info;
    while(TRUE)
    {
        asl_thread_set_cancel_enable();
        asl_sem_wait(&anc->tSem);
        asl_sem_wait(&anc->tOpeSem);
        asl_thread_set_cancel_disable();
        data = (struct asf_nc_tdata*)asd_list_tail(anc->tlist);
        asd_list_del_tail(anc->tlist);
        asl_sem_post(&anc->tOpeSem);
        info.ip = data->info.ip;
        info.port = data->info.port;
        asl_sendto(anc->rsock, ASD_STREAM_DATA(data->s), ASD_STREAM_DSIZE(data->s), &info);
        asf_nc_tdata_delFunc(data);
    }
}

void asf_nc_tras_to(struct asf_nc* anc, struct asd_stream *s, u_int32_t ip, u_int16_t port)
{
    if(anc == NULL || s == NULL || asf_nc_net_info_check(ip, port) != SUCCESS)
        return;
    struct asf_nc_tdata *data = (struct asf_nc_tdata*)asl_malloc(sizeof(struct asf_nc_tdata));
    data->prio = anc->prioCkFunc(s);
    data->info.ip = ip;
    data->info.port = port;
    data->s = asd_stream_create(ASD_STREAM_DSIZE(s));
    //asl_print_dbg("tras data:%p data->s:%p", data, data->s);
    asd_stream_put(data->s, ASD_STREAM_DATA(s), ASD_STREAM_DSIZE(s));
    asl_sem_wait(&anc->tOpeSem);
    asd_list_add_sort(anc->tlist, data);
    asl_sem_post(&anc->tSem);
    asl_sem_post(&anc->tOpeSem);
}

void asf_nc_tras(struct asf_nc* anc, struct asd_stream *s)
{
    if(anc == NULL || s == NULL || asf_nc_net_info_check(anc->tsock->info.ip, anc->tsock->info.port) != SUCCESS)
        return;
    struct asf_nc_tdata *data = (struct asf_nc_tdata*)asl_malloc(sizeof(struct asf_nc_tdata));
    data->prio = anc->prioCkFunc(s);
    data->info.ip = anc->tsock->info.ip;
    data->info.port = anc->tsock->info.port;
    data->s = asd_stream_create(ASD_STREAM_DSIZE(s));
    //asl_print_dbg("tras data:%p data->s:%p", data, data->s);
    asd_stream_put(data->s, ASD_STREAM_DATA(s), ASD_STREAM_DSIZE(s));
    asl_sem_wait(&anc->tOpeSem);
    asd_list_add_sort(anc->tlist, data);
    asl_sem_post(&anc->tSem);
    asl_sem_post(&anc->tOpeSem);
}

struct asf_nc* asf_nc_start(struct asl_net_info *src, struct asl_net_info *dst, prio_check_func prioCkFunc, proc_func func)
{
    struct asf_nc *anc = (struct asf_nc*)asl_malloc(sizeof(struct asf_nc));
    
    anc->rsock = asl_net_create_sock(src, ASL_NET_UDP);
    anc->tsock = asl_net_create_sock(dst, ASL_NET_UDP);

    anc->prioCkFunc = (prioCkFunc == NULL ? asf_nc_default_prioCkFunc : prioCkFunc);
    anc->func = (func == NULL ? asf_nc_default_procFunc : func);
    anc->rlist = asd_list_create();
    anc->rlist->cmpFunc = asf_nc_rdata_cmpFunc;
    asl_sem_create(&anc->rOpeSem, "ASF NC rOpe", 1);
    asl_sem_create(&anc->rSem, "ASF NC r", 0);

    
    anc->tlist = asd_list_create();
    anc->tlist->cmpFunc = asf_nc_tdata_cmpFunc;
    asl_sem_create(&anc->tOpeSem, "ASF NC tOpe", 1);
    asl_sem_create(&anc->tSem, "ASF NC t", 0);

    asl_thread_create(&anc->rthr, asf_nc_recv, anc);
    asl_thread_create(&anc->pthr, asf_nc_proc, anc);
    asl_thread_create(&anc->tthr, asf_nc_send, anc);

    return anc;
}
result_t asf_nc_stop(struct asf_nc **anc)
{
    if(anc != NULL && *anc != NULL)
    {
        struct asf_nc *asfNc = *anc;
        asl_thread_cancel(&asfNc->rthr);
        asl_thread_join(&asfNc->rthr);
        
        asl_thread_cancel(&asfNc->pthr);
        asl_thread_join(&asfNc->pthr);
        
        asl_thread_cancel(&asfNc->tthr);
        asl_thread_join(&asfNc->tthr);

        asl_net_destroy_sock(&asfNc->rsock);

        asfNc->rlist->delFunc = asf_nc_rdata_delFunc;
        asfNc->tlist->delFunc = asf_nc_tdata_delFunc;

        asd_list_destroy(asfNc->rlist);
        asd_list_destroy(asfNc->tlist);

        asl_sem_destroy(&asfNc->rOpeSem);
        asl_sem_destroy(&asfNc->rSem);

        asl_sem_destroy(&asfNc->tOpeSem);
        asl_sem_destroy(&asfNc->tSem);
        
        asl_free(asfNc);
        *anc = NULL;
    }
    return SUCCESS;
}
