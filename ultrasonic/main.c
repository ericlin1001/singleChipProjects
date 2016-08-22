#include<stdlib.h>
#include "header.h"
#include "key8.h"
#include "algorithm.h"
#include "ultrasonic.h"
#include "motor.h"
#include <math.h>
#define LIMIT(x,min,max) if(x>max)x=max;else if(x<min)x=min;
//
void tt();

//global variables: 
volatile TimeType t0=0xffff,t1=0xffff,dt1=0,st1=0; //TimeType is s16
volatile s16 speed=1000;
volatile s16 angle=0;//-14~14
volatile uchar isStart=0;

void ultrasonicOk1(TimeType durTime){
if(durTime>2000)durTime=2000;
//t1=filterData(timeBuffer1,durTime,&pBuffer1);

	//restricted contrain.
	if(durTime<0)durTime=0;
	if(durTime>1000)durTime=1000;
	
	dt1=durTime-t1;
	t1=durTime;
	tt();
}
void ultrasonicOk0(TimeType durTime){
if(durTime>2000)durTime=2000;

//t0=filterData(timeBuffer0,durTime,&pBuffer0);
	t0=durTime;
	tt();
	
}
//**************************


void error();


#define DIST 500
s16 f1(s16 x){
	x/=10;
	x-=30;
	if(x>20)x=20;
	x*=x;
	x/=28;
	return x;
}
s16 f2(s16 x1,s16 x2){
	return 0;
}
s16 keepDist(s16 x){
	s16 an;
	s16 e;
	s16 r=1;
	e=x-DIST;
	st1=e+st1;
	if(e<0){
		r=-1;
		e=-e;
	}
	if(0<=e && e<100)an=1;
	else if(100<=e && e<200)an=2;
	else if(200<=e && e<300)an=4;
	else if(300<=e && e<400)an=6;
	else if(400<=e && e<=500)an=7;
	else an=1;
	an=r*an;
	return an;
}
/*
void controller(){
	angle=keepDist(t1);

	//angle=(t1-DIST)/20;
	LIMIT(angle,-14,14);
	doAngle(angle);
}*/


void controller(){
	if(dt1>DIST){
	//dt1:300~10000
		if(t0<DIST*2){
			angle=10;
		}else{
			angle=f1(dt1);
		}
	}else{
		//if(DIST/5<t1 && t1 <2*DIST){
			angle=keepDist(t1);
		
			if(t0>DIST){
				angle=keepDist(t1);
			}else{
				angle=-4;
			}
		//}else{   
			//error();//!!!
		//}
	}
	LIMIT(angle,-14,14);
	doAngle(angle);
}

void error(){
//not exit.
	EA=0;
	while(1){
		doMotor(0,500);
		doAngle(-14);
		doMotor(0,500);
		doAngle(14);
	}
	while(1);
}

volatile uchar receiveCount=0;
void tt(){
	if(!isStart){
		receiveCount++;
		//doMotor(t1*3,2);
	}else{
		controller();
	}
}

void initAction(){
	receiveCount=0;
	isStart=0;
	//
	doMotor(speed,100);	
	doAngle(-14);
	mydelay(500);
	ultrasonicTrigger0();
	ultrasonicTrigger1();
	doAngle(14);
	mydelay(500);
	ultrasonicTrigger0();
	ultrasonicTrigger1();
	doAngle(0);
	mydelay(500);
	ultrasonicTrigger0();
	ultrasonicTrigger1();
	mydelay(500);
	while(receiveCount<6){
		ultrasonicTrigger0();
		ultrasonicTrigger1();
	}
	/*doAngle(-14);
	mydelay(500);
	doAngle(14);
	mydelay(500);*/
	isStart=1;
}
	
void main(void){
	s16 a=0;
	init();
	//error();
	speed=1000;
	initAction();
	while(1){
		P0_1=1;
		//controller();
		ultrasonicTrigger0();
		ultrasonicTrigger1();
		P0_1=0;
		doMotor(speed,2);	//this should be invovked periodedly,to let motor runs continuously.
	}
	while(1);
}
