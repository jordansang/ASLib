#ifndef ASL_IPV4_H
#define ASL_IPV4_H


#include "asl_net.h"

#define ASL_IPV4_INADDR_ANY INADDR_ANY
#define ASL_IPV4_AF AF_INET
/*
typedef uint32_t in_addr_t;

struct in_addr {
   in_addr_t s_addr;
};

int inet_aton(const char *cp, struct in_addr *inp);

in_addr_t inet_addr(const char *cp);

in_addr_t inet_network(const char *cp);

char *inet_ntoa(struct in_addr in);

struct in_addr inet_makeaddr(int net, int host);

in_addr_t inet_lnaof(struct in_addr in);

in_addr_t inet_netof(struct in_addr in);

INET_ADDRSTRLEN
AF_INET
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
int inet_pton(int af, const char *src, void *dst);
*/

/* "192.168.1.1" to 0xc0a80101 */
result_t asl_ipv4_aton(char* src, u_int32_t *addr);
/* 0xc0a80101 to "192.168.1.1" */
char* asl_ipv4_ntoa(u_int32_t addr, char* dst);
/* "255.255.255.0" to 24 */
result_t asl_ipv4_mask_aton(char* src, u_int8_t *mask);
/* 24 to "255.255.255.0" */
char* asl_ipv4_mask_ntoa(u_int8_t mask, char* dst);

#endif /* ASL_IPV4_H */
