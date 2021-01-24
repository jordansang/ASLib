#ifndef  ASL_NETLINK_H
#define  ASL_NETLINK_H

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

struct asl_netlink_sock
{
    int sock;
    int seq;
    struct sockaddr_nl snl;
    char *name;
};
struct asl_netlink_route_request
{
    struct nlmsghdr n;
    struct rtmsg r;
    char buf[1024];
};
struct asl_netlink_addr_request
{
    struct nlmsghdr n;
    struct ifaddrmsg ifa;
    char buf[1024];
};

struct asl_netlink_msg_operation
{
    int type;
    int (*func)(struct sockaddr_nl *snl, struct nlmsghdr *h);
};

struct asl_netlink_rt
{
    u_int8_t ope;
    u_int32_t ip;
    u_int8_t mask;
    u_int32_t nt;
    u_int32_t ifindex;
};

struct asl_netlink_intf
{
    u_int8_t ope;
    u_int32_t ifindex;
};

typedef void(*asl_netlink_rt_callback)(struct asl_netlink_rt rtInfo);
typedef void(*asl_netlink_intf_callback)(struct asl_netlink_intf intfInfo);
struct asl_netlink
{
    struct asl_thread readThr;
    asl_netlink_rt_callback rtCallback;
    asl_netlink_intf_callback intfCallback;
};

result_t asl_netlink_init(struct asl_netlink *aslNetlink);
result_t asl_netlink_destroy();

#endif /* ASL_NETLINK_H */
