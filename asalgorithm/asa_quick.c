#include "asl_inc.h"
#include "asa_inc.h"
/* Get Pivot */
s_int32_t getPivot(s_int32_t A[], s_int32_t left, s_int32_t right)
{
    s_int32_t tmp;
    s_int32_t center = (left + right)/2;
    if(A[left] > A[center])
    {
        tmp = A[center];
        A[center] = A[left];
        A[left] = tmp;
    }
    if(A[left] > A[right])
    {
        tmp = A[right];
        A[right] = A[left];
        A[left] = tmp;
    }
    if(A[center] > A[right])
    {
        tmp = A[right];
        A[right] = A[center];
        A[center] = tmp;
    }
    tmp = A[right - 1];
    A[right - 1] = A[center];
    A[center] = tmp;
    return A[right - 1];
}

/* Recursion Quick Sort */
void quickSort(s_int32_t A[], s_int32_t left, s_int32_t right)
{
    s_int32_t pivot;
    s_int32_t tmp;
    s_int32_t lpos, rpos;
    //asl_print_dbg("left:%u right:%u\n", left, right);
    if(left < right)
    {
        pivot = getPivot(A, left, right);
        lpos = left;
        rpos = right - 1;
        while(lpos < rpos)
        {
            while(A[++lpos] < pivot);
            while(A[--rpos] > pivot);

            if(lpos < rpos)
            {
                tmp = A[lpos];
                A[lpos] = A[rpos];
                A[rpos] = tmp;
            }
            else
            {
                break;
            }
        }
        //asl_print_dbg("lpos:%u right:%u\n", lpos, right);
        tmp = A[lpos];
        A[lpos] = A[right - 1];
        A[right - 1] = tmp;
        if(lpos > 0)
            quickSort(A, left, lpos - 1);
        quickSort(A, lpos + 1, right);
    }
}
void quickinsertionSort(s_int32_t A[], s_int32_t left, s_int32_t right)
{
    s_int32_t pivot;
    s_int32_t tmp;
    s_int32_t lpos, rpos;
    if(left + 3 < right)
    {
        pivot = getPivot(A, left, right);
        lpos = left;
        rpos = right - 1;
        while(lpos < rpos)
        {
            while(lpos < rpos && A[++lpos] < pivot);
            while(rpos > lpos && A[--rpos] > pivot);

            if(lpos < rpos)
            {
                tmp = A[lpos];
                A[lpos] = A[rpos];
                A[rpos] = tmp;
            }
        }
        tmp = A[lpos];
        A[lpos] = A[right - 1];
        A[right - 1] = tmp;
        quickSort(A, left, lpos - 1);
        quickSort(A, lpos + 1, right);
    }
    else
    {
        insertion(A + left, right - left +1);
    }
}

void quick(s_int32_t A[], s_int32_t count)
{
    quickSort(A, 0, count - 1);
}
