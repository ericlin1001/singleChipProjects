
#include "d7led.h"
#include "ultrasonic.h"
//****for count the time used by ultrasonic************
TimeType timeAfterUltrasonic=0;	

void ultrasonicInit(){
	ULTRA_TRIG_PIN=0;
}
void ultrasonicTrigger(){
    //had better be reinvoked after 	 60ms
	//keep a 10us, high level 
	//using timer1 to count the time.
	uchar i;
	TH1=0;
	TL1=0;
	TR1=1;
	timeAfterUltrasonic=0;
	
	/****start trigger****/
	ULTRA_TRIG_PIN=1;//keep high level for 10us.
	for(i=0;i<5;i++);
	ULTRA_TRIG_PIN=0;
	/****end trigger******/

	/***start receive***/
	EX1=1;
}
/************init....***/
void initTimer(){
	TMOD=0x91; // as 16bit-counter.
	//close all T1,T0
	TR0=0;
	TR1=0;
}
void initExternelInt(){
	IT0=1;    //1: is negative edged triggered.  
	IT1=1;    //1: is negative edged triggered.  
}

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

}
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

uchar isGatheringData=0; 
externlInt1()   interrupt 2
{
	ulong temp;
	if(!isGatheringData){
		isGatheringData=1;
		/****stop timer1,and externel INT!****/
		TR1=0;
		EX1=0;
		/******save the time used to timeAfterUltrasonic****/
		temp=TH1;
		temp<<=8;
		temp|=TL1;
		timeAfterUltrasonic+=temp;
		//saveToBuffer();
		//TODO::saveToBuffer(); saving the data from altrasonic.
		isGatheringData=0;
	}
} 
/******************end counting and gathering data from ultrasonic**************/

