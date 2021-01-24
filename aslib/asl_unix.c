#include "asl_stdio.h"
#include "asl_string.h"
#include "asl_memory.h"
#include "asl_net.h"
#include "asl_unix.h"

result_t asl_unix_socket_create(struct asl_unix_socket *sock)
{
    if(sock != NULL && sock->path != NULL)
    {
        struct sockaddr_un addr;
        socklen_t len;

        //asl_print_dbg("unix socket path:%s", sock->path);
        
        asl_memset(&addr, 0, sizeof (struct sockaddr_un));
        addr.sun_family = AF_UNIX;
        snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", sock->path);
        
        len = sizeof(addr.sun_family) + asl_strlen(addr.sun_path);
        
        if (access(addr.sun_path, W_OK) == 0)
        {
            if ((sock->sock = socket(AF_UNIX, SOCK_STREAM, 0)) > 0)
            {
                if (connect(sock->sock, (struct sockaddr *) &addr, len) == 0)
                {
                    return SUCCESS;
                }
                else
                {
                    asl_print_err("Connect to unix socket FAILED:%s", strerror(errno));
                    close(sock->sock);
                }
            }
        }
    }
    return FAILURE;
}
