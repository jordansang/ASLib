#include "asl_type.h"
#include "asl_stdio.h"
#include "asl_ipv4.h"

result_t asl_ipv4_aton(char* src, u_int32_t *addr)
{
    struct in_addr addr_t;
    if(addr == NULL)
    {
        asl_print_err("Dest Address Buffer is NULL");
        return FAILURE;
    }
    if(inet_pton(AF_INET, src, &addr_t) < 0)
    {
        asl_print_err("Convert Address FAILED");
        return FAILURE;
    }
    *addr = ASL_NTOHL(addr_t.s_addr);
    return SUCCESS;
}
char* asl_ipv4_ntoa(u_int32_t addr, char* dst)
{
    if(dst == NULL)
    {
        asl_print_err("Dest Address Buffer is NULL");
        return NULL;
    }
    u_int32_t naddr = ASL_HTONL(addr);
    if(inet_ntop(AF_INET, &naddr, dst, INET_ADDRSTRLEN) < 0)
    {
        asl_print_err("Convert Address FAILED");
        return NULL;
    }
    //asl_print_dbg("dst:%s",dst);
    return dst;
}
result_t asl_ipv4_mask_aton(char* src, u_int8_t *mask)
{
    struct in_addr addr_t;
    *mask = 0;
    if(mask == NULL)
    {
        asl_print_err("Dest Mask Buffer is NULL");
        return FAILURE;
    }
    if(inet_pton(AF_INET, src, &addr_t) < 0)
    {
        asl_print_err("Convert Mask FAILED");
        return FAILURE;
    }
    while((ASL_HTONL(addr_t.s_addr) & (0x80000000 >> *mask)) != 0)
    {
        (*mask)++;
    }
    return SUCCESS;
}
char* asl_ipv4_mask_ntoa(u_int8_t mask, char* dst)
{
    u_int32_t m = 0xffffffff;
    if(dst == NULL)
    {
        asl_print_err("Dest Mask Buffer is NULL");
        return NULL;
    }
    m = m << (32 - mask);
    m = ASL_HTONL(m);
    if(inet_ntop(AF_INET, &m, dst, INET_ADDRSTRLEN) < 0)
    {
        asl_print_err("Convert Mask FAILED");
        return NULL;
    }
    return dst;
}
