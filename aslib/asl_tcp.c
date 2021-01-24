#include "asl_inc.h"

result_t asl_tcp_create(struct asl_socket *sock)
{
    int state = ENABLE;
    struct sockaddr_in addr;
    
    asl_memset(&addr, 0, sizeof(struct sockaddr_in));
    asl_net_set_sockaddr(&addr, &(sock->info));
    sock->s = socket(ASL_IPV4_AF, SOCK_STREAM, 0);
    //asl_print_dbg("Create UDP Socket 0x%08x:%u", ip, port);
    if(sock->s < 0)
    {
        asl_print_err("Create TCP sock failed");
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
result_t asl_tcp_listen(struct asl_socket *sock, u_int32_t count)
{
    if(sock == NULL || sock->s <= 0 || count == 0)
        return FAILURE;
    return (listen(sock->s, count) < 0 ? FAILURE : SUCCESS);
}
struct asl_socket* asl_tcp_accept(struct asl_socket *sock)
{
    size_t len;
    struct sockaddr_in addr;
    struct asl_socket *csock = ASL_MALLOC_T(struct asl_socket);

    asl_memset(&addr, 0, sizeof(struct sockaddr_in));
    asl_net_set_sockaddr(&addr, &(sock->info));
    len = sizeof(struct sockaddr);
    csock->s = accept(sock->s, (struct sockaddr*)&addr, (socklen_t*)&len);
    csock->sockType = ASL_NET_TCP;
    getsockname(csock->s, (struct sockaddr*)&addr, (socklen_t*)&len);
    asl_net_get_netInfo(&addr, &(csock->info));
    return csock;
}

result_t asl_tcp_connect(struct asl_socket *sock, struct asl_net_info *info)
{
    if(sock == NULL || sock->s <= 0)
        return FAILURE;
    struct sockaddr_in addr;
    asl_memset(&addr, 0, sizeof(struct sockaddr_in));
    asl_net_set_sockaddr(&addr, info);
    size_t len = sizeof(struct sockaddr);
    if(connect(sock->s, (struct sockaddr*)&addr, len) < 0)
        return FAILURE;
    else
        return SUCCESS;
}
