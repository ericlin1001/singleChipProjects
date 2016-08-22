#include<stdlib.h>
#include "header.h"
#include "key8.h"
#include "algorithm.h"
#include "ultrasonic.h"
#define LIMIT(x,min,max) if(x>max)x=max;else if(x<min)x=min;
/*
TimeType timeBuffer0[Buffer_len];
uchar pBuffer0=0;//point to the current data.

TimeType timeBuffer1[Buffer_len];
uchar pBuffer1=0;//point to the current data.
*/
/*
  len(mm)*100/17=time(us)
*/
void tt();
volatile TimeType t0=0xffff,t1=0xffff;
/**************
  program restriction:
  detect distance:2cm~11m
 ***/

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
	if(t1<3000)t1+=200;
	tt();
}

timer0() interrupt 1
{
	//Error!!!!
	isWaitForData0=0;
	TIMER0_STOP;
	DISABLE_EXTINT0;
	if(t0<3000)t0+=200;
	tt();
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


void ultrasonicOk1(TimeType durTime){
//t1=filterData(timeBuffer1,durTime,&pBuffer1);
	t1=durTime;
	tt();
}
void ultrasonicOk0(TimeType durTime){
//t0=filterData(timeBuffer0,durTime,&pBuffer0);
	t0=durTime;
	tt();
}
//**************************


volatile s16 speed;
volatile s16 angle=0;

void mydelay(u16 us)   //误差 0us
{
unsigned char a,b;
if(us<=0)return;
while(us--){
    for(b=1;b>0;b--)
        for(a=2;a>0;a--);
		}
}

#define TURN_MOTOR P0_0
void doAngle(s16 a){
	uchar i=5;
	a+=43;
	while(i--){
		TURN_MOTOR=1;
		mydelay(a);
		TURN_MOTOR=0;
		mydelay(1800-a);
	}
}

void testTurnMotor(){
	while(1){
		doAngle(-14);
		delayms(1000);
		doAngle(14);
		delayms(1000);
		doAngle(0);
		delayms(1000);
	}
}

#define MOTOR0 P1_2
#define MOTOR1 P1_3

#define MOTOR_FORWARD {MOTOR0=0;MOTOR1=1;}
#define MOTOR_STOP {MOTOR0=0;MOTOR1=0;}
#define MOTOR_BACKWARD {MOTOR0=1;MOTOR1=0;}

void doMotor(s16 speed,uchar times){
	if(speed<0){
		speed=-speed;
		speed=speed/2+500;
		while(times--){
		if(rand()%1000<speed){
		MOTOR_BACKWARD;
		}else{
			MOTOR_STOP;
		}
		}
	}else{
		speed=speed/2+500;
		while(times--){
		if(rand()%1000<speed){
		MOTOR_FORWARD;
		}else{
			MOTOR_STOP;
		}
		}
	}
	
	
	
	
	
}

volatile uchar isTuring=0;
volatile uchar isBacking=0;
volatile uchar isLeft_back=0;
void controller(){
	s16 newAngle=0;
	speed=1000;
	newAngle=0;
	if(t0<150||isBacking){
		//backing...
		isBacking=1;
		speed=-1000;
		if(t0>270)isBacking=0;
	}else if(t0<240||isTuring){
		isTuring=1;
		if(t1>500){
			//left turn
			newAngle=-14;
		}else{
			//right turn
			newAngle=14;
		}
		if(t0>400)isTuring=0;
	}else{
	#define DIST 200
		//keep the distance.
		if(t1<70||isLeft_back){
			//slightly right turn.
			isLeft_back=1;
			speed=1000;
			newAngle=4;
			if(t1>DIST+20)isLeft_back=0;
		}else if(t1<DIST*3){
			speed=1000;
			newAngle=-4;
		}else{
			speed=1000;
			newAngle=0;
		}
	}
	if(newAngle!=angle){
		angle=newAngle;
		doAngle(angle);
	}
}
	
	
void tt(){
}



void main(void){
	s16 a=0;
	init();
	//mydelay(5000);
	doAngle(0);
	speed=1000;
	while(1){
		P0_1=1;
		controller();
		ultrasonicTrigger0();
		ultrasonicTrigger1();
		P0_1=0;
		doMotor(speed,100);	
	}
	while(1);
}
