#include "asl_stdio.h"
#include "asl_memory.h"
#include "asl_net.h"
#include "asl_udp.h"
#include "asl_tcp.h"

struct asl_socket* asl_net_create_sock(struct asl_net_info *info, u_int32_t sockType)
{
    if(info == NULL)
        return NULL;
    result_t ret = FAILURE;
    struct asl_socket* sock = ASL_MALLOC_T(struct asl_socket);
    if(sock != NULL)
    {
        asl_memcpy(&sock->info, info, sizeof(struct asl_net_info));
        sock->sockType = sockType;
        switch(sock->sockType)
        {
            case ASL_NET_UDP:
                ret = asl_udp_create(sock);
                break;
            case ASL_NET_TCP:
                ret = asl_tcp_create(sock);
            default:
                break;
        }
    }
    if(ret != SUCCESS)
    {
        asl_free(sock);
        sock = NULL;
    }
    return sock;
}

result_t asl_net_destroy_sock(struct asl_socket **sock)
{
    if(sock != NULL && *sock != NULL)
    {
        if((*sock)->s > 0)
            close((*sock)->s);
        asl_free(*sock);
        *sock = NULL;
    }
    return SUCCESS;
}

result_t asl_net_set_sockaddr(struct sockaddr_in *addr, const struct asl_net_info *netInfo)
{
    if(addr != NULL && netInfo != NULL)
    {
        asl_memset(addr, 0, sizeof(struct sockaddr_in));
        addr->sin_family = AF_INET;
        addr->sin_port   = ASL_HTONS(netInfo->port);
        addr->sin_addr.s_addr = ASL_HTONL(netInfo->ip);
        return SUCCESS;
    }
    return FAILURE;
}

result_t asl_net_get_netInfo(const struct sockaddr_in *addr, struct asl_net_info *netInfo)
{
    if(addr != NULL && netInfo != NULL)
    {
        netInfo->ip = ASL_NTOHL(addr->sin_addr.s_addr);
        netInfo->port = ASL_NTOHS(addr->sin_port);
        return SUCCESS;
    }
    return FAILURE;
}

s_int32_t asl_sendto(struct asl_socket *sock, void* buf, u_int32_t len, struct asl_net_info *dst)
{
    if(sock == NULL || sock->s <= 0)
        return FAILURE;
    if(buf == NULL || len == 0)
        return SUCCESS;
    struct sockaddr_in *in = NULL;
    if(dst != NULL)
    {
        struct sockaddr_in addr;
        asl_memset(&addr, 0, sizeof(struct sockaddr_in));
        asl_net_set_sockaddr(&addr, dst);
        in = &addr;
    }
    return (sendto(sock->s, (char*)buf, len, 0, (struct sockaddr*)in, sizeof(struct sockaddr_in)));
}

s_int32_t asl_recvfrom(struct asl_socket *sock, void *buf, u_int32_t len, struct asl_net_info *dst)
{
    struct sockaddr_in *in = NULL;
    u_int32_t addrLen = sizeof(struct sockaddr_in);
    if(sock == NULL || sock->s <= 0 || buf == NULL || len == 0)
        return FAILURE;
    if(dst != NULL)
    {
        struct sockaddr_in addr;
        asl_memset(&addr, 0, sizeof(struct sockaddr_in));
        asl_net_set_sockaddr(&addr, dst);
        in = &addr;
    }
    return recvfrom(sock->s, buf, len, 0, (struct sockaddr*)in, (socklen_t*)&addrLen);
}

result_t asl_net_set_nonblocking(struct asl_socket *sock, s_int32_t state)
{
    if(sock != NULL)
    {
        return ioctl(sock->s, FIONBIO, (int)&state);
    }
    return FAILURE;
}
