#include "header.h"
#include "d7led.h"
#include "algorithm.h"
#include "ultrasonic.h"
void initInterrupt();
void d7led_main();
void d7led_main(){
	
	while(1)
	{
		D7LED_refresh();
		delayms(1);
	}
}

#define TIMER_GATE(bit) ((bit)<<3)
/*0: only controlled by TR0/TR1
1: controlled by both of TR0/TR1 and INT0/INT1
*/
#define TIMER_TC(bit) ((bit)<<2) //0:timer,1 for counter.
#define TIMER_MODE(t) (t) 
/*
0:13bit-coutner,N=13,
1:16bit-counter,N=16
2: two 8bit-counter,auto initialized with initial value,N=8
3: two 8bit-counter,only work for T0,N=8
*/

#define BIT_HIGH8(m) ((m)>>4)
#define BIT_LOW8(m) ((m)&0xff)


#define TIMER0_INIT(m) TMOD&=0xff00;TMOD|=(m);
#define TIMER0_START TR0=1
#define TIMER0_SET(m) TH0=BIT_HIGH8((0x10000-(m)));TL0=BIT_LOW8((0x10000-(m)));

#define TIMER1_INIT(m) TMOD&=0x00ff;TMOD|=((m)<<4);
#define ENABLE_ALL_INT EA=1
#define ENABLE_TIMER0 ET0=1





timer0() interrupt 1  
{  /****for the leds scan dispaly****/
	TIMER0_SET(0x000f);
	
	D7LED_refresh();
}

void initTimer0(){
	
}


static void init(){
/****/
TIMER0_INIT(TIMER_GATE(0)|TIMER_TC(0)|TIMER_MODE(1));
ENABLE_TIMER0;
ENABLE_ALL_INT;
TIMER0_START;

}

void main(void){
init();

D7LED_show(2345678);
while(1);

initInterrupt();
D7LED_show(2345678);
while(1);
	d7led_main();
	
	
}
