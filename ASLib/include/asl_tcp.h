#ifndef ASL_TCP_H
#define ASL_TCP_H

#include "asl_net.h"

result_t asl_tcp_create(struct asl_socket *sock);
result_t asl_tcp_listen(struct asl_socket *sock, u_int32_t count);
struct asl_socket* asl_tcp_accept(struct asl_socket *sock);
result_t asl_tcp_connect(struct asl_socket *sock, struct asl_net_info *info);

#endif /* ASL_TCP_H */