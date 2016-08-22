//ultransonic-byEric.cpp
//曾吉申～ 可以直接复制这里的。。。
/***
TMOD:
T1                T0
GATE C/~T M1 M0 | GATE C/~T M1 M0 | 

M1M0: 
00  13bit-counter,N=13
01	 16bit-counter,N=16
10	 2 8bit-counter,auto initialized with initial value,N=8
11	 2 8bit-counter,only for T0,N=8


C/~T:
0: counter the machine period,used for timer.
1: counter the pulse in T0,T1 pin. negative pulse is valid.


GATE:
0: only controlled by TR0/TR1
1: controlled by both of TR0/TR1 and INT0/INT1
*/
/*
IP:interrup priority
IP bit: 4	3	 2	 1	 0
PS	PT1	 PX1 PT0 PT0
1:is high priority, 0 is lower priority.

TCON: timer,externel interrup control register.
7~0
7    6    5    4    3   2  1   0
TF1	 TR1  TF0 TR0 IE1 IT1 IE0 IT0
TF1:timer1 overflow request.
IE1:interrupt externel 1 request.
IT1:~INT1 control bit. 1: is edged triggered. 0:TLL level triggered.

IE:interrupt enable.
7~0
7  4  3   2   1   0
EA ES ET1 EX1 ET0 EX0	
EA: enable all interrup. 1 allows all interrup.
ES:serial int enable	
ET1:timer1 enable
EX1:externel enable.
*/
#include <reg52.h>  
sbit P10=P1^0;
sbit P11=P1^1;

void initUltrasonic();

/*externlInt0()   interrupt 0
  {
  }*/


/*******************filter and show in Leds,from data:timeAfterUltrasonic********/
#define Buffer_len 8
TimeType timeBuffer[Buffer_len];
uchar pBuffer=0;

/*	
#define realTimeBuffer_len 3
TimeType realTimeBuffer[realTimeBuffer_len];
uchar pRealTimeBuffer=0;

#define realTime_Delta_Max 30*/	

uchar ignoreDeltaCount=3;

void filterAndShow(){
	TimeType realTime=0;
	/*****start calulate the realTime******/
	quicksort(timeBuffer,Buffer_len);
	/**
	  a very interesting bug!!!
	  quicksort(timeBuffer,Buffer_len); when i delete this instruction,compiler outputs:*** ERROR L107: ADDRESS SPACE OVERFLOW
	  I really can't understand what happened?!

*/
	realTime=timeBuffer[Buffer_len*5/10]		   ;
	/*****end calulate the realTime******/

	/****limit the delta*******/
	if(ignoreDeltaCount==0){
		/*long oldRealTime=realTimeBuffer[pRealTimeBuffer];
		  long delta=realTime-oldRealTime;
		  if(abs(delta)>realTime_Delta_Max){
		  realTime=oldRealTime+sign(delta)*realTime_Delta_Max;
		  } */
	}else{
		ignoreDeltaCount--;
	}
	/****end limit the delta*******/ 

	/*	pRealTimeBuffer++;
		pRealTimeBuffer%=realTimeBuffer_len;
		realTimeBuffer[pRealTimeBuffer]=realTime;	  	*/
	/**************/
	//showNumInLeds(realTime/10);

	/*
	   realTime is us.
	   t=realTime*10^-6s
	   L=t*(340/2)m/s=t*170=realTime*17*10^-5m
	   Lcm=L*10^2cm/m=realTime*17*10^-3cm.
	   Lmm=L*10^3mm/m=realTime*17*10^-2mm.
	   Lum=L*10^6um/m=realTime*17*10^1um;
	   */
	realTime*=17;
	realTime*=10;
	showNumInLeds(realTime);  
}

void saveToBuffer(){
	pBuffer++;
	pBuffer%=Buffer_len;
	timeBuffer[pBuffer]=timeAfterUltrasonic;
	if(pBuffer==0){
		filterAndShow();
	}	 
}
/*******************end filter and show in Leds,from data:timeAfterUltrasonic********/

void myDelay(int round){
	while(round--){
		int i=0;
		for(i=0;i<1000;i++);}
}

void main(void)   // 主程序
{ 
	//ulong showLedNum=12345678;
	//  	init();
//	LED_DATA=0x03;
//	LED_DATA=~SEG7[4];
//	LED_SELECT=5;
//	while(1);

	showNumInLeds(123);
	while(1){
		refreshLeds();
	}
	//start T0,stop T1.
	TR0=1;
	TR1=0;
	while(1){
		if(!isGatheringData){
			trigerUltrasonic();
		}
		//showNumInLeds(showLedNum--);
		myDelay(2);
	}

	while(1);
}
