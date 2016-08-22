#include "header.h"
#include "d7led.h"
#include "algorithm.h"
#include "ultrasonic.h"
void initInterrupt();
void d7led_main();

/*
   len(mm)*100/17=time(us)


*/

/**************
  program restriction:
  detect distance:2cm~11m
 ***/


TimeType timeAfterUltrasonic=0;	
void d7led_main(){
	while(1)
	{
		D7LED_refresh();
		delayms(1);
	}
}

/*0: only controlled by TR0/TR1
1: controlled by both of TR0/TR1 and INT0/INT1
*/
/*
0:13bit-coutner,N=13,
1:16bit-counter,N=16
2: two 8bit-counter,auto initialized with initial value,N=8
3: two 8bit-counter,only work for T0,N=8
*/

#define BIT_HIGH8(m) ((m)>>4)
#define BIT_LOW8(m) ((m)&0xff)
//
#define TIMER_GATE(bit) ((bit)<<3)
#define TIMER_TC(bit) ((bit)<<2) //0:timer,1 for counter.
#define TIMER_MODE(t) (t) 
//
#define TIMER0_INIT(m) TMOD&=0xff00;TMOD|=(m);
#define TIMER0_START TR0=1
#define TIMER0_STOP TR0=0
#define TIMER0_SET(m) TH0=BIT_HIGH8((0x10000-(m)));TL0=BIT_LOW8((0x10000-(m)));
//
#define TIMER1_INIT(m) TMOD&=0x00ff;TMOD|=((m)<<4);
#define TIMER1_START TR1=1
#define TIMER1_STOP TR1=0
#define TIMER1_SET(m) TH1=BIT_HIGH8((0x10000-(m)));TL1=BIT_LOW8((0x10000-(m)));
//
#define ENABLE_ALL_INT EA=1
#define ENABLE_TIMER0 ET0=1
#define ENABLE_EXTINT0 EX0=1 
#define ENABLE_EXTINT1 EX1=1

//
#define DISABLE_EXTINT0 EX0=0
#define DISABLE_EXTINT1 EX1=0
//
#define EXTINT1_MODE_TRIG(m) IT1=1;//1: is negative edged triggered. 



timer0() interrupt 1  
{  /****for the leds scan dispaly****/
	TIMER0_SET(0x0080);
	D7LED_refresh();
}


static void init(){
	ENABLE_ALL_INT;
	ENABLE_TIMER0;

	//
	TIMER0_INIT(TIMER_GATE(0)|TIMER_TC(0)|TIMER_MODE(1));
	TIMER0_STOP;
	TIMER0_START;
	//
	TIMER1_INIT(TIMER_GATE(1)|TIMER_TC(0)|TIMER_MODE(1));
	TIMER1_STOP;
	//
	DISABLE_EXTINT1;
	EXTINT1_MODE_TRIG(1);//negative edge trigger.
}
static uchar isWaitForData=0;
timer1() interrupt 3  
{
	//Error!!!!
	isWaitForData=0;
	TIMER1_STOP;
	DISABLE_EXTINT1;
}



void ultrasonicInit(){
	ULTRA_TRIG_PIN=0;
}
uchar countForResend=0;
#define MAX_COUNTFORRESEND 10
void ultrasonicTrigger(){
	//had better be reinvoked after 	 60ms
	//keep a 10us, high level 
	//using timer1 to count the time.
	uchar i;
	if(isWaitForData){
		//it means ,the last time data haven't recevied!.
		countForResend++;
		if(countForResend>=MAX_COUNTFORRESEND){
			countForResend=0;
			isWaitForData=0;
		}
	}else{
		//normally trigger the ultrasonic.
		timeAfterUltrasonic=0;
		TIMER1_SET(65536);
		TIMER1_START;//however,it only count for 
		//the high level signal.

		isWaitForData=1;
		/****start trigger****/
		ULTRA_TRIG_PIN=1;//keep high level for 10us.
		for(i=0;i<5;i++);
		ULTRA_TRIG_PIN=0;
		/****end trigger******/

		/***start receive***/
		ENABLE_EXTINT1;
	}
}

void ultrasonicOk();

static uchar isReceivingData=0; 
void ultrasonicReceive(){
	u16 temp=0;
	if(!isReceivingData && isWaitForData){//to avoid reenter this function!.
		isReceivingData=1;
		/******save the time used to timeAfterUltrasonic****/
		temp=TH1;
		temp<<=8;
		temp|=TL1;
		if(temp<10){
			isReceivingData=0;
			return;
		}
		/****stop timer1,and externel INT!****/
		TIMER1_STOP;
		DISABLE_EXTINT1;
		timeAfterUltrasonic=temp;
		ultrasonicOk();
		isReceivingData=0;
		isWaitForData=0;
	}
}

#define Buffer_len 4
TimeType timeBuffer[Buffer_len];
uchar pBuffer=0;//point to the current data.
#define MAX_K 1300
#define MAX_K2 300
void showDistance(u16 time){
	//D7LED_show(time*17/100);
	D7LED_show(time);
}

void initBuffer(){
	//take care of the init value.
	uchar i;
	for(i=0;i<Buffer_len;i++){
		timeBuffer[i]=0;
	}

}
u16 min(u16 a,u16 b){return a>b?b:a;}
u16 max(u16 a,u16 b){return a>b?a:b;}

void filterData(){
	u16 a,b,c;
	u16 mind,maxd;
	u16 s,t;
	//.
	a=timeBuffer[(pBuffer+Buffer_len-2)%Buffer_len];
	b=timeBuffer[(pBuffer+Buffer_len-1)%Buffer_len];
	c=timeBuffer[pBuffer];
	//d=timeAfterUltrasonic;
	//now,check the value of d and correct it.
	s=3*b-3*c-a;
	t=2*c-b;	
	mind=max(-MAX_K2-s,t-MAX_K);
	maxd=min(MAX_K2-s,MAX_K+t);
	//
	if(timeAfterUltrasonic>maxd){
	//	timeAfterUltrasonic=maxd;
	}
	if(timeAfterUltrasonic<mind){
	//	timeAfterUltrasonic=mind;
	}
	//
	pBuffer++;
	pBuffer%=Buffer_len;
	timeBuffer[pBuffer]=timeAfterUltrasonic;
}

void ultrasonicOk(){
	//the time is in var:timeAfterUltrasonic.
	//this will be invoked whenever we have received the ultrasonic echo.
	//showDistance(timeAfterUltrasonic);
	filterData();
	showDistance(timeAfterUltrasonic);
}



externlInt1() interrupt 2
{
	ultrasonicReceive();
} 



void test(){
	while(1){
		//if(!isGatheringData){
		ultrasonicTrigger();
		//}
		//showNumInLeds(showLedNum--);
		delayms(20);
	}
}

void main(void){
	D7LED_show(12345678);
	init();
	initBuffer();
	delayms(1000);
	test();
	while(1);

}
