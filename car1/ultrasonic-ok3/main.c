#include<stdlib.h>
#include "header.h"

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
	P1_6=~P1_6;
}
uchar p37=0;
ulong testcount=0;

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
	P1_7=~P1_7;
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

void initBuffer(){
	//take care of the init value.
	/*uchar i;
	for(i=0;i<Buffer_len;i++){
		timeBuffer1[i]=0;
		timeBuffer0[i]=0;
	}*/
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

ulong temp_time;
#define doTime(m) temp_time=m;while(temp_time--)

sbit leftWheelControl0=P1^1;
sbit leftWheelControl1=P1^0;

sbit rightWheelControl1=P1^2;
sbit rightWheelControl0=P1^3;
/*
#define LeftWheel_Stop 		{leftWheelControl0=0;leftWheelControl1=0;}
#define LeftWheel_Forward 	{leftWheelControl0=1;leftWheelControl1=0;}
#define LeftWheel_Backward 	{leftWheelControl0=0;leftWheelControl1=1;}

#define RightWheel_Stop 		{rightWheelControl0=0;rightWheelControl1=0;}
#define RightWheel_Forward 		{rightWheelControl0=1;rightWheelControl1=0;}
#define RightWheel_Backward 	{rightWheelControl0=0;rightWheelControl1=1;}
*/
#define LeftWheel_Stop 		doTime(50){leftWheelControl0=0;leftWheelControl1=0;}
#define LeftWheel_Forward 	doTime(50){leftWheelControl0=1;leftWheelControl1=0;}
#define LeftWheel_Backward 	doTime(50){leftWheelControl0=0;leftWheelControl1=1;}

#define RightWheel_Stop 		doTime(50){rightWheelControl0=0;rightWheelControl1=0;}
#define RightWheel_Forward 		doTime(50){rightWheelControl0=1;rightWheelControl1=0;}
#define RightWheel_Backward 	doTime(50){rightWheelControl0=0;rightWheelControl1=1;}




#define LeftHighWidth  800
#define LeftLowWidth	180
//100 left >right

#define RightHighWidth  100
#define RightLowWidth	0

//************************************


ulong myRandLong(){
  ulong temp=0;
   temp=rand();
   temp*=rand();
   return temp;
}


u16 speed=1000;



#define MODE_FORWARD 0
#define MODE_LEFT 1
#define MODE_B 1
volatile uchar mode=0;
volatile s16 vr,vl;
volatile s16 vx=0,vy=-1000;
volatile s16 fx=0,fy;
volatile uchar isChange=1;




void controlLeftWheel(s16 v){
	if(v>=0){
		if(rand()%1100<v){
			LeftWheel_Forward
		}else{
			LeftWheel_Stop
		}	
	}else{
		v=-v;
		if(rand()%1100<v){
			LeftWheel_Backward
		}else{
			LeftWheel_Stop
		}	
	}
}			
void controlRightWheel(s16 v){
	if(v>=0){
		if(rand()%1000<v){
			RightWheel_Forward
		}else{
			RightWheel_Stop
		}	
	}else{
		v=-v;
		if(rand()%1000<v){
			RightWheel_Backward
		}else{
			RightWheel_Stop
		}	
	}
}
/*
void calForce(){
fx=0;
fy=100;
if(t0>500 && t1>500){
//safe area.
}else{
	if(t0<100){
		fx+=500;
		fy+=-800;
	}
	if(t1<100){
		fx+=-500;
		fy+=-800;
	}
	if(t0>200&&t1>200){
		fx+=t1;
		fx=-t0;
		//
		fy-=(t1+t0)/10;
	}
}
}*/

void calForce(){
	fx=0;
	fy=400;
	if(t1<300&&t0<300){
		fy-=800;
		fx+=t0;
		fx-=t1;
	}
	if(t1<200&&t0<200){
		fy-=1000;
		fx+=t0;
		fx-=t1; 
	}
}

uchar isTuring=0;
uchar isBacking=0;

uchar isLeft_back=0;
void calVRL(){
LIMIT(vx,-1000,1000);
	LIMIT(vy,-1000,1000);
	vl=vy+vx;
	vr=vy-vx;
	}
	
void tt(){
#define DIST 150
	vx=0;
	vy=0;
	if(t0<80||isBacking){
		isBacking=1;
		vx+=0;
		vy+=-((int)1200-t0*7);
		if(t0>200)isBacking=0;
	}else if(t0<230||isTuring){
		isTuring=1;
		if(t1>250){
			vx+=-((int)300+(230-t0)*3);
		}else{
			vx+=((int)300+(230-t0)*3);
		}
		vy+=t0-120;
		if(t0>400)isTuring=0;
	}else{
		//keep the distance.
		if(t1<70||isLeft_back){
			isLeft_back=1;
			vx+=300;
			vy+=t1/2;
			if(t1>DIST+20)isLeft_back=0;
		}else if(t1<DIST*3){
			vy+=500+(t1-80)*3;
			vx+=-(t1-DIST);
		}else{
			vy+=1000;
			vx+=0;
		}
	}
	calVRL();
}


void controlMotors(){
controlLeftWheel(vl);
controlRightWheel(vr);
}
void motorInitAction(){
doTime(2){
vx=0;
vy=1000;
//
calVRL();
		controlLeftWheel(vl);
		controlRightWheel(vr);
		}
		doTime(10){
vx=0;
vy=-1000;
//
calVRL();
		controlLeftWheel(vl);
		controlRightWheel(vr);
	}
	
}

void test(){
uchar i=9;
while(1)
	//motorInitAction();
	while(1){
		if(++i%10==0){
			ultrasonicTrigger0();
			ultrasonicTrigger1();
			
		}
		controlLeftWheel(vl);
		controlRightWheel(vr);
/*
		controlMotors();*/
	}
}
void main(void){
uchar k=10;
//0.5-2.5ms
	while(1){
		P2_0=1;
		P2_1=0;
	}
	while(1);
	init();
	
	//initBuffer();
	//delayms(1000);
	test();
	while(1);
}
