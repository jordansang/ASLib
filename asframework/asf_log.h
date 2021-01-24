#ifndef ASF_LOG_H
#define ASF_LOG_H

#include "asl_type.h"
#include "asl_net.h"

s_int32_t asf_log_file_create(const char* path);
s_int32_t asf_log_net_create(struct asl_net_info *info);
void asf_log(s_int32_t id, const char* fmt, ...);


#endif /* ASF_LOG_H */
