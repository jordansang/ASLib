#include "asl_inc.h"
#include "asf_thread_pool.h"

static void test_tcp_release(void *arg)
{
    struct asl_socket *sock = (struct asl_socket*)arg;
    asl_net_destroy_sock(&sock);
}

static void test_tcp_server_do(void *arg)
{
    u_int8_t buf[128] = {0};
    s_int32_t rlen, tlen;
    u_int64_t stamp;
    struct asl_socket *sock = (struct asl_socket*)arg;
    const char* banner = "Welcome to AS command Server\nEnter Your Command\n>";
    write(sock->s, banner, strlen(banner) + 1);
    while(TRUE)
    {
        rlen = asl_recv(sock, buf, sizeof(buf));
        if(rlen <= 0)
            break;
        asl_print_hex(buf, rlen, "Server Recv");
        if(buf[0] == '0')
            break;
        stamp = asl_time_get_stamp();
        sprintf(buf, "Current time %llu >", stamp);
        tlen = asl_send(sock, buf, strlen(buf) + 1);
        if(tlen <= 0)
            break;
    }
    asl_net_destroy_sock(&sock);
}

static void test_tcp_server(void *arg)
{
    struct asf_tp *at = (struct asf_tp*)arg;
    u_int8_t buf[128] = {0};
    s_int32_t rlen, tlen;
    u_int64_t stamp;
    struct asl_net_info info;
    info.ip = 0x7f000001;
    info.port = 10001;
    struct asl_socket *sock = asl_net_create_sock(&info, ASL_NET_TCP);
    asl_thread_cleanup_begin(test_tcp_release, sock);
    struct asl_socket *csock;
    if(sock == NULL)
    {
        asl_print_err("sock is NULL");
        return;
    }
    if(asl_tcp_listen(sock, 3) != SUCCESS)
    {
        asl_print_err("Listen on socket(%d) failed", sock->s);
        return;
    }
    while(TRUE)
    {
        asl_print_dbg("Ready to Accept...");
        csock = asl_tcp_accept(sock);
        asl_print_dbg("Accepted:%d", csock->s);
        if(csock->s > 0)
            asf_tp_start_routine(at, test_tcp_server_do, csock);
        else
            asl_print_err("Accept client failed");
    }
    asl_thread_cleanup_end(ASL_THREAD_NOT_NEED_CLEANUP);
}

static void test_tcp_client(void *arg)
{
    u_int8_t buf[128] = {0};
    s_int32_t rlen, tlen;
    u_int64_t stamp;
    struct asl_net_info info;
    info.ip = 0x7f000001;
    info.port = 10002;
    struct asl_net_info dinfo;
    dinfo.ip = 0x7f000001;
    dinfo.port = 10001;
    struct asl_socket *sock = asl_net_create_sock(&info, ASL_NET_TCP);
    asl_thread_cleanup_begin(test_tcp_release, sock);
    if(sock == NULL)
        return;
    while(TRUE)
    {
        while(asl_tcp_connect(sock, &dinfo) != SUCCESS)
            sleep(1);
        while(TRUE)
        {
            stamp = asl_time_get_stamp();
            tlen = asl_send(sock, &stamp, sizeof(stamp));
            if(tlen <= 0)
                break;
            rlen = asl_recv(sock, buf, sizeof(buf));
            if(rlen <= 0)
                break;
            asl_print_hex(buf, rlen, "Client Recv");
            sleep(1);
        }
    }
    asl_thread_cleanup_end(ASL_THREAD_NOT_NEED_CLEANUP);
}

void test_tcp()
{
    struct asf_tp *at = asf_tp_create();
    asf_tp_start_routine(at, test_tcp_server, at);
    asf_tp_start_routine(at, test_tcp_client, NULL);


    while(TRUE)
    {
        sleep(1000);
    }
}
