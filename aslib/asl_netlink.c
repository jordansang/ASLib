#include "asl_stdio.h"
#include "asl_memory.h"
#include "asl_thread.h"
#include "asl_net.h"
#include "asl_netlink.h"

/* kernel messages */
struct asl_netlink_sock netlinkKernelSock   ={ -1, 0,{ 0 }, "netlinkKernelSock-listen" };
struct asl_netlink *ntGlobal = NULL;

static void asl_netlink_parse_rtattr(struct rtattr **tb, int max,
    struct rtattr *rta, int len)
{
    while (RTA_OK(rta, len))
    {
        if (rta->rta_type <= max)
            tb[rta->rta_type] = rta;
        rta = RTA_NEXT(rta, len);
    }
}

static result_t asl_netlink_route_change(struct sockaddr_nl *snl, struct nlmsghdr *h)
{
    int len;
    struct rtmsg *rtm;
    struct rtattr *tb[RTA_MAX + 1];
    struct asl_netlink_rt rtInfo;
    
    rtm = NLMSG_DATA(h);

    if (!(h->nlmsg_type == RTM_NEWROUTE || h->nlmsg_type == RTM_DELROUTE))
    {
        /* If this is not route add/delete message print warning. */
        asl_print_err("Kernel message: %d.", h->nlmsg_type);
        return SUCCESS;
    }

    #if 0
    asl_print_dbg("%s %s %s proto %s %u.",
                h->nlmsg_type == RTM_NEWROUTE ? "RTM_NEWROUTE" : "RTM_DELROUTE",
                rtm->rtm_family == AF_INET ? "IPV4" : "IPV6",
                rtm->rtm_type == RTN_UNICAST ? "UNICAST" : "MULTICAST",
                lookup_str(routeTypeString, rtm->rtm_protocol), rtm->rtm_protocol);
    #endif    
    if(rtm->rtm_family != AF_INET)
    {
        //asl_print_err("IP Family is NOT AF_INET.");
        return SUCCESS;
    }
    if (rtm->rtm_type != RTN_UNICAST)
    {
        //asl_print_err("route information is not RTN_UNICAST type.");
        return SUCCESS;
    }

    len = h->nlmsg_len - NLMSG_LENGTH(sizeof(struct rtmsg));
    if (len < 0)
    {
        //asl_print_dbg("netlinkKernelSock message length is illegal.");
        return FAILURE;
    }

    asl_memset(tb, 0, sizeof tb);
    asl_netlink_parse_rtattr(tb, RTA_MAX, RTM_RTA(rtm), len);
    
    if (rtm->rtm_flags & RTM_F_CLONED)
    {
        //asl_print_err("route is cloned.");
        return SUCCESS;
    }
    //asl_print_dbg("rtm->rtm_protocol:%d ker:%d zebra:%d static:%d", 
    //            rtm->rtm_protocol, RTPROT_KERNEL, RTPROT_ZEBRA, RTPROT_STATIC);
    if (rtm->rtm_protocol == RTPROT_REDIRECT
        || rtm->rtm_protocol == RTPROT_KERNEL)
    {
        //asl_print_err("route protocol is %s.", (rtm->rtm_protocol == RTPROT_REDIRECT) ? "REDIRECT" : "KERNEL");
        return SUCCESS;
    }

    if (rtm->rtm_protocol == RTPROT_ZEBRA)
    {
        //asl_print_err("route protocol is ZEBRA. %d", rtm->rtm_protocol);
        //return SUCCESS;
    }

    if (rtm->rtm_src_len != 0)
    {
        //asl_print_err("rtm_src_len is illegal.");
        return SUCCESS;
    }

    if (tb[RTA_OIF])
    {
        rtInfo.ifindex = *(int *)RTA_DATA(tb[RTA_OIF]);
    }

    if (tb[RTA_DST])
    {
        rtInfo.ip = *(u_int32_t*)RTA_DATA(tb[RTA_DST]);
        rtInfo.ip = ASL_NTOHL(rtInfo.ip);
    }

    if (tb[RTA_GATEWAY])
    {
        rtInfo.nt = *(u_int32_t*)RTA_DATA(tb[RTA_GATEWAY]);
        rtInfo.nt = ASL_NTOHL(rtInfo.nt);
    }

    rtInfo.mask = rtm->rtm_dst_len;
    rtInfo.ope = (h->nlmsg_type == RTM_NEWROUTE ? ENABLE : DISABLE);
    //asl_print_dbg("%s ifindex:%u dest:0x%08x gate:0x%08x", 
    //        asl_get_state_str(rtInfo.ope), rtInfo.ifindex, rtInfo.ip, rtInfo.nt);
    ntGlobal->rtCallback(rtInfo);
    return SUCCESS;
}

static result_t asl_netlink_create(struct asl_netlink_sock *nl, unsigned long groups)
{
    int ret;
    struct sockaddr_nl snl;
    int sock;
    int namelen;

    sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0)
    {
        asl_print_err("Can't open %s socket: %s", nl->name,strerror (errno));
        return sock;
    }
    #if 0 /* Set this socket nonblock may cause loop to fast */
    ret = fcntl(sock, F_SETFL, O_NONBLOCK);
    if (ret < 0)
    {
        asl_print_err("Can't set %s socket flags: %s", nl->name,strerror (errno));
        close(sock);
        return ret;
    }
    #endif
    asl_memset(&snl, 0, sizeof snl);
    snl.nl_family = AF_NETLINK;
    snl.nl_groups = groups;

    /* Bind the socket to the netlinkKernelSock structure for anything. */
    ret = bind(sock, (struct sockaddr *) &snl, sizeof snl);
    if (ret < 0)
    {
        asl_print_dbg("Can't bind %s socket to group 0x%x: %s", nl->name, snl.nl_groups, strerror (errno));
        close(sock);
        return ret;
    }

    /* multiple netlinkKernelSock sockets will have different nl_pid */
    namelen = sizeof snl;
    ret = getsockname(sock, (struct sockaddr *) &snl, (socklen_t*)&namelen);
    if (ret < 0 || namelen != sizeof snl)
    {
        asl_print_dbg("Can't get %s socket name: %s", nl->name,strerror (errno));
        close(sock);
        return ret;
    }

    nl->snl = snl;
    nl->sock = sock;
    return ret;
}

static result_t asl_netlink_read(struct asl_netlink_sock *nl)
{
    int status;
    int ret = SUCCESS;
    int error;
    while (TRUE)
    {
        char buf[4096];
        struct iovec iov = { buf, sizeof buf };
        struct sockaddr_nl snl;
        struct msghdr msg = { (void*)&snl, sizeof snl, &iov, 1, NULL, 0, 0 };
        struct nlmsghdr *h;

        status = recvmsg(nl->sock, &msg, 0);
        //asl_print_dbg("Receive Netlink MSG");
        if (status < 0)
        {
            if (errno == EINTR)
                continue;
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                break;
            continue;
        }

        if (snl.nl_pid != 0)
        {
            continue;
        }

        if (status == 0)
        {
            asl_print_err("Receive netlinkKernelSock message length is 0.");
            return FAILURE;
        }

        if (msg.msg_namelen != sizeof snl)
        {
            asl_print_err("netlinkKernelSock message is not illegal.");
            return FAILURE;
        }
        for (h = (struct nlmsghdr *) buf; NLMSG_OK(h, status); h = NLMSG_NEXT(h, status))
        {
            //asl_print_dbg("h->nlmsg_type:0x%04x {0x%04x 0x%04x}", h->nlmsg_type, NLMSG_DONE, NLMSG_ERROR);
            /* Finish of reading. */
            if (h->nlmsg_type == NLMSG_DONE)
                return ret;

            /* Error handling. */
            if (h->nlmsg_type == NLMSG_ERROR)
            {
                struct nlmsgerr *err = (struct nlmsgerr *) NLMSG_DATA(h);

                /* If the error field is zero, then this is an ACK */
                if (err->error == 0)
                {
                    asl_print_dbg("Get ACK from netlinkKernelSock.");
                    continue;
                }

                if (h->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr)))
                {
                    asl_print_err("Can't analyse the netlinkKernelSock error message.");
                }
                continue;
            }

            if(h->nlmsg_type == RTM_NEWROUTE || h->nlmsg_type == RTM_DELROUTE)
            {
                error = asl_netlink_route_change(&snl, h);
            }
            else
            {
                //asl_print_err("Unknown netlinkKernelSock nlmsg_type %d.", h->nlmsg_type);
                continue;
            }
            if (error < 0)
            {
                asl_print_err("%s filter function error.", nl->name);
                ret = error;
            }

        }

        /* After error care. */
        if (msg.msg_flags & MSG_TRUNC)
        {
            asl_print_err("%s error: message truncated.", nl->name);
            continue;
        }
        if (status)
        {
            asl_print_err("%s error: data remnant size %d.", nl->name, status);
            return FAILURE;
        }
    }
    return ret;
}

result_t asl_netlink_init(struct asl_netlink *aslNetlink)
{
    if(aslNetlink == NULL)
    {
        asl_print_err("Must designate Callback Functions");
        return FAILURE;
    }
    ntGlobal = aslNetlink;
    int ret = SUCCESS;
    unsigned long groups;
    groups = RTMGRP_LINK | RTMGRP_IPV4_ROUTE | RTMGRP_IPV4_IFADDR;

    ret = asl_netlink_create(&netlinkKernelSock, groups);
    if (ret < 0)
    {
        asl_print_dbg("Create default netlinkKernelSock failed.");
        return ret;
    }

    /* Register kernel socket. */
    if (netlinkKernelSock.sock > 0)
    {
        asl_thread_create(&(ntGlobal->readThr), asl_netlink_read, &netlinkKernelSock);
    }
    return ret;
}

result_t asl_netlink_destroy()
{
    close(netlinkKernelSock.sock);
    return SUCCESS;
}

