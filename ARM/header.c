#include "header.h"

volatile u32 runTimeMs=0;//Attention:this can only count up to 45 days.
u32 getTime(){
	return runTimeMs;
}
void delayms(u16 ms){
	u32 endTime=getTime()+ms;
	while(runTimeMs<endTime);
}

void delayus(u16 us){
	u8 i;while(us--)for(i=0;i<72;i++);
}



/*********************tickFunc*****************/
typedef struct TickFunc{
	void (*pCallBackFunc)();
	u16 period;
}TickFunc;
volatile TickFunc tickFuncs[MAX_TICKFUNC];
volatile u8 tickFuncLen=0;

void sysTickInit(){
	//SysTickClk=HCLK/8=72MHz/8=9MHz.
	SysTick_SetReload(9000);//count 9000,then INT every 1ms
	SysTick_ITConfig(ENABLE);//
	SysTick_CounterCmd(SysTick_Counter_Enable);
}

void registerTick(void (*pCallBackFunc)(),u16 period){
	tickFuncs[tickFuncLen].pCallBackFunc=pCallBackFunc;
	tickFuncs[tickFuncLen].period=max(period,1);
	tickFuncLen++;
}
/*********************end tickFunc*****************/

void SysTickHandler(void){
	u8 i;
	runTimeMs++;
	for(i=0;i<tickFuncLen;i++){
		if(runTimeMs%tickFuncs[i].period==0){
			(*tickFuncs[i].pCallBackFunc)();
		}
	}	
}
