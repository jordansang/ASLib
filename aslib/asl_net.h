#ifndef ASL_NET_H
#define ASL_NET_H
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum ASL_NET_SOCK_TYPE
{
    ASL_NET_UDP,
    ASL_NET_TCP,
    ASL_NET_NETLINK,
    ASL_NET_RAW,
    ASL_NET_MAX
};

#define ASL_HTONS(x) htons(x)
#define ASL_NTOHS(x) ntohs(x)
#define ASL_HTONL(x) htonl(x)
#define ASL_NTOHL(x) ntohl(x)

#define ASL_GET_NET_SEGMENT(IP, MASK) (IP & (0xffffffff << (32 - MASK)))


struct asl_net_info
{
    u_int32_t ip;
    u_int16_t port;
};

struct asl_socket
{
    struct asl_net_info info;
    u_int8_t sockType;
    s_int32_t s;
};

struct asl_socket* asl_net_create_sock(struct asl_net_info *info, u_int32_t sockType);
result_t asl_net_destroy_sock(struct asl_socket **sock);
result_t asl_net_set_sockaddr(struct sockaddr_in *addr, const struct asl_net_info *netInfo);
result_t asl_net_get_netInfo(const struct sockaddr_in *addr, struct asl_net_info *netInfo);
s_int32_t asl_sendto(struct asl_socket *sock, void* buf, u_int32_t len, struct asl_net_info *dst);
#define asl_send(sock, buf, len) asl_sendto(sock, buf, len, NULL)
s_int32_t asl_recvfrom(struct asl_socket *sock, void *buf, u_int32_t len, struct asl_net_info *dst);
#define asl_recv(sock, buf, len) asl_recvfrom(sock, buf, len, NULL)
result_t asl_net_set_nonblocking(struct asl_socket *sock, s_int32_t state);


#endif /* ASL_NET_H */
