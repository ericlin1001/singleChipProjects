#include "algorithm.h"
/****************algorithm***********************/
static void swap(QuickSortType *a,int i,int j){*(a+i)^=*(a+j)^=*(a+i)^=*(a+j);}

void quicksort(QuickSortType *array, int n)
{
	if(n>1){
		int pivot=0,j;
		for(j=1;j<n;j++)
			if(array[j]<array[0])
				swap(array,++pivot,j);
		if(pivot!=0)swap(array,0,pivot);
		quicksort(array,pivot);
		quicksort(array+pivot+1,n-pivot-1);
	}
}
void copyArr(QuickSortType *from,QuickSortType *to,int n){
	while(n--)*to++=*from++;
}
long abs(long a){if(a<0)return -a;return a;}
long abs2(long a,long b){if(a>b)return a-b;return b-a;}
char sign(long a){if(a>0)return 1;if(a<0)return -1;return 0;}
ulong sumArr(ulong *arr,uchar size){
	uchar i,temp=0;
	for(i=0;i<size;i++)temp+=arr[i];
	return temp;
}
/****************end algorithm***********************/
