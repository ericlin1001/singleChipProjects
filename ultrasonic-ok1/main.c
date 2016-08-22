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



#define MODE_FORWARD 0
#define MODE_LEFT 1
#define MODE_B 1
volatile uchar mode=0;
volatile s16 speed;
volatile s16 vr,vl;
volatile s16 vx=0,vy=-1000;
volatile s16 fx=0,fy;
volatile uchar isChange=1;



void controlMotors(){
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
		controlMotors();
	}
}

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
	/*uchar i=5;
	a+=25;
	while(i--){
		TURN_MOTOR=1;
		mydelay(a);
		TURN_MOTOR=0;
		mydelay(700-a);
	}*/
	uchar i=5;
	a+=43;
	while(i--){
		TURN_MOTOR=1;
		mydelay(a);
		TURN_MOTOR=0;
		mydelay(1800-a);
	}
}
#define DELAYTIME 1000
//0xe2~
//11~43
//+20
//31-63
//0-32
void initAction(){
P0=0x33;
	delayms(DELAYTIME);
	P0=0xff;
	delayms(DELAYTIME);
}
void testTurnMotor(){
	while(1){
		doAngle(-14);
		delayms(DELAYTIME);
		doAngle(14);
		delayms(DELAYTIME);
		doAngle(0);
		delayms(DELAYTIME);
	}
}

#define MOTOR0 P1_2
#define MOTOR1 P1_3

#define MOTOR_FORWARD {MOTOR0=0;MOTOR1=1;}
#define MOTOR_STOP {MOTOR0=0;MOTOR1=0;}
#define MOTOR_BACKWARD {MOTOR0=1;MOTOR1=0;}

void doMotor(s16 speed){
	uchar isBack=0;
	if(speed<0){
		isBack=1;
		speed=-speed;
	}
	if(rand()%1000<speed){
		if(isBack){
		MOTOR_BACKWARD;
		}else{
		MOTOR_FORWARD;
		}
	}else{
		MOTOR_STOP;
	}
}
void tt(){
	//s16 angle;
	//speed=t0+800;
	//LIMIT(speed,-1000,1000);
	/*if(t1<200){
		doAngle(14);
	}else{
		doAngle(-14);
	}*/
	speed=t1*2+500;
	LIMIT(speed,-1000,1000);
	/*if(t0<300){
		if(t1>350){
			doAngle(-14);
		}else{
			doAngle(14);
		}
	}else{
		doAngle(0);
	}

	*/
	//P0=~(t0/10);
}

void testUltra(){
	while(1){
		ultrasonicTrigger0();
		ultrasonicTrigger1();
		delayms(20);
	}
}
/*
void testMotor(){
	s16 speed=800;
	while(1){
		if(isKeyPress(0)){
			speed+=100;
		}
		if(isKeyPress(1)){
			speed-=100;
		}
		if(isKeyPress(2)){
			speed=800;
		}
		if(isKeyPress(3)){
			speed=-speed;
		}
		P0=speed/100;
		processKey();
		doMotor(speed);
	}
}
*/
void doDelay(u16 us){
unsigned char a,b,c;
	while(--us){
    for(c=1;c>0;c--)
        for(b=142;b>0;b--)
            for(a=2;a>0;a--);
		}
}

#define TESTPIN P0
void main(void){
	s16 a=0;
	u16 b=0;
	s16 k=0;

	//testTurnMotor();
	init();
	//initAction();
	mydelay(5000);
	doAngle(0);
	speed=1000;
	while(1){
		if(++a%10==0){
			ultrasonicTrigger0();
			ultrasonicTrigger1();
		}
		//delayms(20);
		doMotor(speed);
	}


	testMotor();

	while(1)
	for(k=-20;k<20;k++){
		doAngle(k);
	}
	while(1){
	doAngle(0);
	}




	while(1);
}
/*
	testTurnMotor();
	testMotor();
	testUltra();
	while(1);
	init();
	//initBuffer();
	//delayms(1000);
	test();*/
//P1=0;
	/*while(1){
		for(a=0;a<65535/3;a++)
		P2=0xff;
		for(a=0;a<65535/3;a++)
		P2=0;
		//delayms(1000);
	}
	doAngle(0);
	testTurnMotor();*/
	/*
	while(1){

		doAngle(-14);
		delayms(DELAYTIME);
		doAngle(14);
		delayms(DELAYTIME);
		doAngle(0);
		delayms(DELAYTIME);
	}

	testTurnMotor();
	while(1){
		P1_2=0;
		doAngle(0);
	}*/
	/*while(1){
		P1=0;
		delayms(2000);
		P1=0xff;
		delayms(2000);
	}*/
	/*while(1){
		doAngle(14);
		delayms(200);
		doAngle(-14);
		delayms(200);
	}*/
	//testUltra();
	//testUltra();

/*while(1){
		if(isKeyPress(0)){
			a++;
			doAngle(a);
		}
		if(isKeyPress(1)){
			a--;
			doAngle(a);
		}
		if(isKeyPress(2)){
			a=0;
			doAngle(a);
		}
		P1=a;
		processKey();
	}*/