#ifndef ASF_NET_CORE_H
#define ASF_NET_CORE_H

#include "asd_list.h"
#include "asd_stream.h"
#include "asl_thread.h"
#include "asl_ipv4.h"
#include "asl_net.h"

#define ASF_NET_CORE_MTU 1500


typedef void(*proc_func)(struct asd_stream *s);
typedef u_int32_t(*prio_check_func)(struct asd_stream *s);

struct asf_nc
{
    struct asl_socket *rsock;
    struct asl_socket *tsock;

    prio_check_func prioCkFunc;
    proc_func func;

    struct asl_thread rthr;
    struct asl_thread pthr;
    struct asl_thread tthr;
    
    struct asd_list *rlist;
    struct asl_sem rOpeSem;
    struct asl_sem rSem;
    
    struct asd_list *tlist;
    struct asl_sem tOpeSem;
    struct asl_sem tSem;
};

struct asf_nc* asf_nc_start(struct asl_net_info *src, struct asl_net_info *dst, prio_check_func prioCkFunc, proc_func func);
result_t asf_nc_stop(struct asf_nc **anc);
void asf_nc_tras_to(struct asf_nc* anc, struct asd_stream *s, u_int32_t ip, u_int16_t port);
void asf_nc_tras(struct asf_nc* anc, struct asd_stream *s);

#endif /* ASF_NET_CORE_H */
