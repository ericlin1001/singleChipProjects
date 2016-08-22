#include <math.h>
#include "header.h"
#include "key8.h"
#include "d7led.h"
static ulong mainLoopCount=0;

/***********************d7led***************/
uchar code digitToD7led[16]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};
uchar d7led_outputBuffer[8]={0,0,0,0,0,0,0,0};
uchar d7led_cur=0;
void D7LED_showc(uchar num){
d7led_port=digitToD7led[num];
d7led_Select_port=7-d7led_cur;
}
void D7LED_show(ulong num){
uchar i=0;
#define BASE 10
while(num){
d7led_outputBuffer[i]=digitToD7led[num%BASE];
num/=BASE;
i++;
}
if(i==0)d7led_outputBuffer[i++]=digitToD7led[0];
/**delete the leading 0**/
for(;i<8;i++)d7led_outputBuffer[i]=0;
}
void D7LED_refresh(){
d7led_cur++;d7led_cur%=8;
d7led_port=d7led_outputBuffer[d7led_cur];
d7led_Select_port=7-d7led_cur;
}
/***********************end d7led***************/


#define _DO_TEST
/**************test some thing****/
#ifdef main
#undef main
#endif
#define main test_main
#ifdef main

#endif
#ifdef main
#undef main
#endif
/************end test****/



static void init(){
mainLoopCount=0;
D7LED_show(0);
}


static void mainLoop(){
static ulong num=0;
#define SELECT 1

#if SELECT==0
	
#endif

#if SELECT==1
uchar i;
for(i=0;i<8;i++){
if(isKeyPress(i)){
	num+=pow(10,i);
	D7LED_show(num);
}
}


#endif

#if SELECT==2
	
#endif
#if SELECT==1
	
#endif



}

int d7led_main(){
#ifdef DO_TEST
test_main();
#endif
	init();
	while(1)
	{
		processKey();
		D7LED_refresh();
		delayms(1);
		mainLoop();
		mainLoopCount++;
	}
	return 0;
}
