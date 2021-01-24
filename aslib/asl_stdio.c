#include "asl_stdio.h"

void asl_print_hex(const void* data, int size, const char* desc)
{
    int index;
    if(data == NULL || size <= 0)
    {
        return;
    }
    if(desc == NULL)
    {
        asl_printf("Temp Data(%d)",size);
    }
    else
    {
        asl_printf("%s Data(%d)", desc, size);
    }
    for(index = 0; index < size; index++)
    {
        /* new line for every 10 items */
        if(index%10 == 0)
        {
            asl_printf("\n");
        }
        asl_printf("%02x ", ((unsigned char*)(data))[index]);
    }
    asl_printf("\n");
    return;
}
char* asl_get_state_str(result_t state)
{
    return (state == ENABLE ? "ENABLE" : "DISABLE");
}
char* asl_get_ret_str(result_t ret)
{
    return (ret == SUCCESS ? "SUCCESS" : "FAILURE");
}
