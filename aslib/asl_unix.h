#ifndef ASL_UNIX_H
#define ASL_UNIX_H

#include <sys/un.h>

struct asl_unix_socket
{
    s_int32_t sock;
    char* path;
};
result_t asl_unix_socket_create(struct asl_unix_socket *sock);


#endif /* ASL_UNIX_H */
