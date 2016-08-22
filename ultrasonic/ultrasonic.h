#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include "header.h"

/**************
  program restriction:
  detect distance:2cm~11m
  not restrict to 20~2000mm.
 ***/
 
#define ULTRA0_TRIG_PIN P1_0
#define ULTRA1_TRIG_PIN P1_1
void ultrasonicInit();
void ultrasonicTrigger();
void initTimer();
//

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
	//TIMER1_INIT(TIMER_GATE(1)|TIMER_TC(0)|TIMER_MODE(1));
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
	ultrasonicOk1(2000);
}

timer0() interrupt 1
{
	//Error!!!!
	isWaitForData0=0;
	TIMER0_STOP;
	DISABLE_EXTINT0;
	ultrasonicOk0(2000);
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


		ULTRA1_TRIG_PIN=1;//keep high level for 10us.
		for(i=0;i<5;i++);
		ULTRA1_TRIG_PIN=0;

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
		ultrasonicOk0(temp*17/100);
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
		ultrasonicOk1(temp*17/100);
		isReceivingData1=0;
		isWaitForData1=0;
	}
}

externlInt0() interrupt 0
{
	ultrasonicReceive0();
}

externlInt1() interrupt 2
{
	ultrasonicReceive1();
}

#endif

