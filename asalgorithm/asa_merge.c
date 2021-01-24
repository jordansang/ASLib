#include "asl_inc.h"
/* Merge Sort */
static void mergeSort(s_int32_t A[], s_int32_t *tmp, s_int32_t left, s_int32_t center, s_int32_t right)
{
    s_int32_t lpos,lend, rpos,rend, nums, pos;
    pos = left;
    nums = right - left + 1;
    lpos = left;
    rpos = center + 1;
    lend = center;
    rend = right;
    while(lpos <= lend && rpos <= rend)
    {
        if(A[lpos] < A[rpos])
        {
            tmp[pos++] = A[lpos++];
        }
        else
        {
            tmp[pos++] = A[rpos++];
        }
    }
    while(lpos <= lend)
    {
        tmp[pos++] = A[lpos++];
    }
    while(rpos <= rend)
    {
        tmp[pos++] = A[rpos++];
    }
    for(pos = left; pos < left + nums; pos++)
    {
        A[pos] = tmp[pos];
    }
}

/* Recursion Seprate List to two parts */
static void recursionSeprate(s_int32_t A[], s_int32_t *tmp, s_int32_t left, s_int32_t right)
{
    s_int32_t center;
    if(left < right)
    {
        center = (right + left)/2;
        recursionSeprate(A, tmp, left, center);
        recursionSeprate(A, tmp, center + 1, right);
        mergeSort(A, tmp, left, center, right);
    }
}


void merge(s_int32_t A[], s_int32_t count)
{
    s_int32_t *tmp = (s_int32_t*)asl_malloc(count * sizeof(s_int32_t));
    if(tmp == NULL)
    {
        asl_print_err("Malloc for tmp FAILED\n");
        return;
    }
    recursionSeprate(A, tmp, 0, count - 1);
    asl_free(tmp);
}
