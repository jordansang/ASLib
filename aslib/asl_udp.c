#include "asl_inc.h"

result_t asl_udp_create(struct asl_socket *sock)
{
    int state = ENABLE;
    struct sockaddr_in addr;
    
    asl_memset(&addr, 0, sizeof(struct sockaddr_in));
    asl_net_set_sockaddr(&addr, &(sock->info));
    sock->s = socket(ASL_IPV4_AF, SOCK_DGRAM, 0);
    //asl_print_dbg("Create UDP Socket 0x%08x:%u", ip, port);
    if(sock->s < 0)
    {
        asl_print_err("Create UDP sock failed");
        return FAILURE;
    }
    setsockopt(sock->s, SOL_SOCKET, SO_REUSEADDR, (void *)&state, sizeof(int));
    //setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void *)&state, sizeof(int));
    if(bind(sock->s, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        asl_print_err("Socket(%d) bind to 0x%08x:%d failed", 
                            sock->s, sock->info.ip, sock->info.port);
        return FAILURE;
    }
    return SUCCESS;
}

