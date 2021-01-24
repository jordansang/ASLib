#include "asl_inc.h"

void bubble(s_int32_t A[], s_int32_t count)
{
    s_int32_t tmp;
    int i,j;
    for(i = 0; i < count; i++)
    {
        for(j = 0; j < count - 1 - i; j++)
        {
            if(A[j] > A[j + 1])
            {
                tmp = A[j];
                A[j] = A[j + 1];
                A[j + 1] = tmp;
            }
        }
    }
}
