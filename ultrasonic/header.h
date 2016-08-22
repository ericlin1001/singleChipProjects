#ifndef HEADER_H
#define HEADER_H
#include <reg51.h>
#include <intrins.h>
#define SBIT_DECL(sfreg,i) 	sbit sfreg##_##i=sfreg^i
#define SBIT_DELC_ALL(sfreg) \
  SBIT_DECL(sfreg,0);\
  SBIT_DECL(sfreg,1);\
  SBIT_DECL(sfreg,2);\
  SBIT_DECL(sfreg,3);\
  SBIT_DECL(sfreg,4);\
  SBIT_DECL(sfreg,5);\
  SBIT_DECL(sfreg,6);\
  SBIT_DECL(sfreg,7)


SBIT_DELC_ALL(P0);
SBIT_DELC_ALL(P1);
SBIT_DELC_ALL(P2);
SBIT_DELC_ALL(P3);	
  


typedef bit BOOL ;
//
typedef unsigned char uchar;
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef signed char schar;
typedef signed char s8;
typedef signed int s16;
//
typedef unsigned char BYTE;	//用BYTE代替unsigned char 
typedef unsigned int WORD;//WORD代替unsigned int
//
typedef s16 TimeType;
#define Buffer_len 4
#define MAX_K 1500
#define MAX_K2 300

 //
#define MAIN_CHOOSE(mainFunc) \
int mainFunc();\
int main(){\
return mainFunc();\
}

#define TRUE 1
#define FALSE 0

void delayus(u16 us);
void delayms(u16 ms);
void delays(u16 s);
void delay(u16 ms);
void mydelay(u16 us);   //delay 10us


#define US(value) (value)
#define MS(value) US(value*1000)
#define HIGH8_16(v) (u8)(v>>8)
#define LOW8_16(v)	(u8)(v)
#define TIME_H(time) HIGH8_16(65536-time) //be careful ,time <=65536us,or <=65ms
#define TIME_L(time) LOW8_16(65536-time)


/***********/
uchar bit_inv(uchar ch);


/***
---TMOD:
	  T1                T0
GATE C/~T M1 M0 | GATE C/~T M1 M0 | 

M1M0: 
00  13bit-counter,N=13
01	 16bit-counter,N=16
10	 2 8bit-counter,auto initialized with initial value,N=8
11	 2 8bit-counter,only for T0,N=8


C/~T:
0: counter the machine period,used for timer.
1: counter the pulse in T0,T1 pin. negative pulse is valid.


GATE:
0: only controlled by TR0/TR1
1: controlled by both of TR0/TR1 and INT0/INT1
*/
/*****
---IP:interrup priority
IP bit: 4	3	 2	 1	 0
		PS	PT1	 PX1 PT0 PT0
1:is high priority, 0 is lower priority.

---TCON: timer,externel interrup control register.
7~0
7    6    5    4    3   2  1   0
TF1	 TR1  TF0 TR0 IE1 IT1 IE0 IT0
TF1:timer1 overflow request.
IE1:interrupt externel 1 request.
IT1:~INT1 control bit. 1: is edged triggered. 0:TLL level triggered.

---IE:interrupt enable.
7~0
7  4  3   2   1   0
EA ES ET1 EX1 ET0 EX0	
EA: enable all interrup. 1 allows all interrup.
ES:serial int enable	
ET1:timer1 enable
EX1:externel enable.
*/

/*0: only controlled by TR0/TR1
1: controlled by both of TR0/TR1 and INT0/INT1
*/
/*
0:13bit-coutner,N=13,
1:16bit-counter,N=16
2: two 8bit-counter,auto initialized with initial value,N=8
3: two 8bit-counter,only work for T0,N=8
*/

#define BIT_HIGH8(m) ((m)>>4)
#define BIT_LOW8(m) ((m)&0xff)
//
#define TIMER_GATE(bit) ((bit)<<3)
#define TIMER_TC(bit) ((bit)<<2) //0:timer,1 for counter.
#define TIMER_MODE(t) (t) 
//
#define TIMER0_INIT(m) TMOD&=0xff00;TMOD|=(m);
#define TIMER0_START TR0=1
#define TIMER0_STOP TR0=0
#define TIMER0_SET(m) TH0=BIT_HIGH8((0x10000-(m)));TL0=BIT_LOW8((0x10000-(m)));
//
#define TIMER1_INIT(m) TMOD&=0x00ff;TMOD|=((m)<<4);
#define TIMER1_START TR1=1
#define TIMER1_STOP TR1=0
#define TIMER1_SET(m) TH1=BIT_HIGH8((0x10000-(m)));TL1=BIT_LOW8((0x10000-(m)));
//
#define ENABLE_ALL_INT EA=1
#define ENABLE_TIMER0 ET0=1
#define ENABLE_TIMER1 ET1=1
#define ENABLE_EXTINT0 EX0=1 
#define ENABLE_EXTINT1 EX1=1
//
#define DISABLE_EXTINT0 EX0=0
#define DISABLE_EXTINT1 EX1=0
//
#define EXTINT1_MODE_TRIG(m) IT1=1;//1: is negative edged triggered. 
#define EXTINT0_MODE_TRIG(m) IT0=1;//1: is negative edged triggered. 
#endif

