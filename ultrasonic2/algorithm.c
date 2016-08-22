#include "algorithm.h"
/****************algorithm***********************/
static void swap(QuickSortType *a,int i,int j){*(a+i)^=*(a+j)^=*(a+i)^=*(a+j);}
s16 min(s16 a,s16 b){return a>b?b:a;}
s16 max(s16 a,s16 b){return a>b?a:b;}
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
//long abs(long a){if(a<0)return -a;return a;}
long abs2(long a,long b){if(a>b)return a-b;return b-a;}
char sign(long a){if(a>0)return 1;if(a<0)return -1;return 0;}
ulong sumArr(ulong *arr,uchar size){
	uchar i,temp=0;
	for(i=0;i<size;i++)temp+=arr[i];
	return temp;
}
TimeType filterData(TimeType timeBuffer[],TimeType curTime,uchar *ppBuffer){
	s16 a,b,c,d;
	s16 mind,maxd;
	s16 t;
	//
	a=timeBuffer[((*ppBuffer)+Buffer_len-2)%Buffer_len];
	b=timeBuffer[((*ppBuffer)+Buffer_len-1)%Buffer_len];
	c=timeBuffer[*ppBuffer];
	d=curTime;
	//now,check the value of d and correct it.
	mind=-MAX_K+c;
	maxd=MAX_K+c;
	if((b-a)*(c-b)>0){
		//restrict the second order differnce....
		t=2*c-b;	
		mind=max(mind,-MAX_K2+t);
		maxd=min(maxd,MAX_K2+t);
	}
	if(d>maxd)d=maxd;
	if(d<mind)d=mind;
	//
	(*ppBuffer)++;
	(*ppBuffer)%=Buffer_len;
	timeBuffer[*ppBuffer]=d;
	return d;
}
/****************end algorithm***********************/
