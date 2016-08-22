#include "header.h"
#include "d7led.h"
#include "algorithm.h"
#include "ultrasonic.h"


#define EXT1
#define ULTRA1
#define TIM1

TimeType timeBuffer1[Buffer_len];
uchar pBuffer1=0;//point to the current data.

/*
  len(mm)*100/17=time(us)
*/

/**************
  program restriction:
  detect distance:2cm~11m
 ***/

ulong showNum=0;

static uchar isWaitForData0=0;
static uchar countForResend0=0;
static uchar isReceivingData0=0; 

static uchar isWaitForData1=0;
static uchar countForResend1=0;
static uchar isReceivingData1=0; 

void ultrasonicOk0(TimeType durTime);
void ultrasonicOk1(TimeType durTime);

static void init(){
	ENABLE_ALL_INT;

	ENABLE_TIMER1;
	TIMER1_INIT(TIMER_GATE(1)|TIMER_TC(0)|TIMER_MODE(1));
	TIMER1_STOP;

	ENABLE_TIMER0;
	//TIMER0_INIT(TIMER_GATE(1)|TIMER_TC(0)|TIMER_MODE(1));
	TMOD=(TIMER_GATE(1)|TIMER_TC(0)|TIMER_MODE(1))|(TIMER_GATE(1)|TIMER_TC(0)|TIMER_MODE(1))<<4;
	TIMER0_STOP;
	//
	DISABLE_EXTINT1;
	DISABLE_EXTINT0;
	EXTINT1_MODE_TRIG(1);//negative edge trigger.
	EXTINT0_MODE_TRIG(1);
}


timer1() interrupt 3  
{
	//Error!!!!
	isWaitForData1=0;
	TIMER1_STOP;
	DISABLE_EXTINT1;
}

timer0() interrupt 1
{
	//Error!!!!
	isWaitForData0=0;
	TIMER0_STOP;
	DISABLE_EXTINT0;
}

void ultrasonicInit(){
	ULTRA1_TRIG_PIN=0;
	ULTRA0_TRIG_PIN=0;
}

void ultrasonicTrigger0(){
	//had better be reinvoked after 	 60ms
	//keep a 10us, high level 
	//using timer1 to count the time.
	uchar i;
	#define MAX_COUNTFORRESEND 10
	if(isWaitForData0){
		//it means ,the last time data haven't recevied!.
		countForResend0++;
		if(countForResend0>=MAX_COUNTFORRESEND){
			countForResend0=0;
			isWaitForData0=0;
		}
	}else{
		//normally trigger the ultrasonic.
		isWaitForData0=1;
	
		TIMER0_SET(65536);
		TIMER0_START;//however,it only count for 

		
		/****start trigger****/
	
		ULTRA0_TRIG_PIN=1;//keep high level for 10us.
		for(i=0;i<5;i++);
		ULTRA0_TRIG_PIN=0;

		/****end trigger******/

		/***start receive***/
		ENABLE_EXTINT0;
	}
}

void ultrasonicTrigger1(){
	//had better be reinvoked after 	 60ms
	//keep a 10us, high level 
	//using timer1 to count the time.
	uchar i;
	if(isWaitForData1){
		//it means ,the last time data haven't recevied!.
		countForResend1++;
		if(countForResend1>=MAX_COUNTFORRESEND){
			countForResend1=0;
			isWaitForData1=0;
		}
	}else{
		//normally trigger the ultrasonic.
		isWaitForData1=1;
	
		TIMER1_SET(65536);
		TIMER1_START;//however,it only count for 
	
		/****start trigger****/
		ULTRA1_TRIG_PIN=1;//keep high level for 10us.
		for(i=0;i<5;i++);
		ULTRA1_TRIG_PIN=0;

		/****end trigger******/

		/***start receive***/
		ENABLE_EXTINT1;
	}
}


void ultrasonicReceive0(){
	u16 temp=0;
	if(!isReceivingData0 && isWaitForData0){//to avoid reenter this function!.
		isReceivingData0=1;
		/******save the time used to timeAfterUltrasonic****/
		temp=TH0;
		temp<<=8;
		temp|=TL0;
		if(temp<10){
			isReceivingData0=0;
			return;
		}
		/****stop timer1,and externel INT!****/
		TIMER0_STOP;
		DISABLE_EXTINT0;
		ultrasonicOk0(temp);
		isReceivingData0=0;
		isWaitForData0=0;
	}
}

void ultrasonicReceive1(){
	u16 temp=0;
	if(!isReceivingData1 && isWaitForData1){//to avoid reenter this function!.
		isReceivingData1=1;
		/******save the time used to timeAfterUltrasonic****/
		temp=TH1;
		temp<<=8;
		temp|=TL1;
		if(temp<10){
			isReceivingData1=0;
			return;
		}
		/****stop timer1,and externel INT!****/
		TIMER1_STOP;
		DISABLE_EXTINT1;
		ultrasonicOk1(temp);
		isReceivingData1=0;
		isWaitForData1=0;
	}
}

void initBuffer(){
	//take care of the init value.
	uchar i;
	for(i=0;i<Buffer_len;i++){
		timeBuffer1[i]=0;
	}

}


void showDistance1(u16 time){
showNum=(ulong)(time*17/100)*10000+(showNum%10000);
D7LED_show(showNum);
}
void showDistance0(u16 time){
showNum=(time*17/100)+(showNum/10000*10000);
D7LED_show(showNum);
}

void ultrasonicOk0(TimeType durTime){
	//the time is in var:timeAfterUltrasonic.
	//this will be invoked whenever we have received the ultrasonic echo.
	//showDistance(timeAfterUltrasonic);
	//filterData(timeBuffer1,durTime,&pBuffer1);
	//showDistance(timeBuffer1[pBuffer1],0);
	showDistance0(durTime);
}
void ultrasonicOk1(TimeType durTime){
	//the time is in var:timeAfterUltrasonic.
	//this will be invoked whenever we have received the ultrasonic echo.
	//showDistance(timeAfterUltrasonic);
	//filterData(timeBuffer1,durTime,&pBuffer1);
	//showDistance(timeBuffer1[pBuffer1],0);
	showDistance1(durTime);
}

externlInt0() interrupt 0
{
	ultrasonicReceive0();
} 


externlInt1() interrupt 2
{
	ultrasonicReceive1();
} 

void test(){
	while(1){
		ultrasonicTrigger0();
		ultrasonicTrigger1();
		D7LED_refresh();
		//delayms(20);
		delayms(2);
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
