#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "header.h"
typedef u16 QuickSortType; 
void quicksort(QuickSortType *array, int n);
void copyArr(QuickSortType *from,QuickSortType *to,int n);
//long abs(long a);
long abs2(long a,long b);
char sign(long a);
ulong sum(ulong *arr,uchar size);
s16 min(s16 a,s16 b);
s16 max(s16 a,s16 b);
TimeType filterData(TimeType timeBuffer[],TimeType curTime,uchar *ppBuffer);
#endif

