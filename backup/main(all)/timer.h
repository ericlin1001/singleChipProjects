#ifndef TIMER_H
#define TIMER_H
#include "header.h"
void INT0_init();
//please do...
//#define INT0_CALLBACK_FUNC ... before use this one.
void INT0_init(){
	EA=1;
	ET0=1;
	TMOD=0x01;
	TH0=TIME_H(MS(50));
	TL0=TIME_L(MS(50));
	TR0=1;
}
void INT0_CALLBACK_FUNC();
void INT0_proc () interrupt 1{
	INT0_CALLBACK_FUNC();
	TH0=TIME_H(MS(50));
	TL0=TIME_L(MS(50));
}
#endif