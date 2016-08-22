#ifndef HEADER_H
#define HEADER_H
#include "stm32f10x_lib.h"
typedef unsigned char uchar;
typedef unsigned long ulong;
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
//
#define delay delayms
//
extern volatile u32 runTimeMs;//Attention:this can only count up to 45 days.
u32 getTime(void);
void delayms(u16 ms);
void delayus(u16 us);


#define MAX_TICKFUNC 20
void sysTickInit(void);
void registerTick(void (*pCallBackFunc)(void),u16 period);

#endif


