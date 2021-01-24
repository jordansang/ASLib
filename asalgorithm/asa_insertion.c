#include "asl_inc.h"
void insertion(s_int32_t A[], s_int32_t count)
{
    s_int32_t tmp;
    int i,j;
    for(i = 1; i < count; i++)
    {
        tmp = A[i];
        for(j = i; j > 0 && tmp < A[j - 1]; j--)
        {
            A[j] = A[j - 1];
        }
        A[j] = tmp;
    }
}
