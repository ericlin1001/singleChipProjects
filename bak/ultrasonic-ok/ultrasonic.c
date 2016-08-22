
#include "d7led.h"
#include "ultrasonic.h"
//****for count the time used by ultrasonic************


/************init....***/
/*
void initTimer(){
	TMOD=0x91; // as 16bit-counter.
	//close all T1,T0
	TR0=0;
	TR1=0;
}*//*
void initExternelInt(){
	IT0=1;    //1: is negative edged triggered.  
	IT1=1;    //1: is negative edged triggered.  
}*/
/*
void initInterrupt(){
	initTimer();
	//initExternelInt();
	//enable timer T1,T0
	ET0=1;
	ET1=1;
	//disable externel interrupt INT1,INT0
	EX0=0;
	EX1=0;
	//disable serial INT
	ES=0;
	//enable all interrupt.
	EA=1;

}*/
/*
void init(){
	initInterrupt();
	initUltrasonic();
}*/


/************init....***/

/******************counting and gathering data from ultrasonic,save data to :timeAfterUltrasonic**************/
/*timer1() interrupt 3  
{
	timeAfterUltrasonic+=0x10000;
	//continue count the time....
	TH1=0;
	TL1=0;
}*/


/******************end counting and gathering data from ultrasonic**************/

