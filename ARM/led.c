#include "led.h"
/***********************d7led***************/
void D7LED_showBase(ulong num,u8 base);
//
u8 const digitToD7led[16]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};
volatile u8 d7led_outputBuffer[8]={0,0,0,0,0,0,0,0};
volatile u8 d7led_cur=0;
/********need to modify*******/
void D7LED_init(){
	//config GPIOA:output push-pull 
	GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All; //select all pin
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //output push-pull 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //I/O speed =50Hz
   GPIO_Init(GPIOA, &GPIO_InitStructure);  //set GPIOA
}
void D7LED_output(u8 data,u8 select){
	u16 t;
	t=select;
	t<<=8+5;
	t+=data;
	GPIO_Write(GPIOA,t);
}
/*******************/

void D7LED_showc(u8 num){
	D7LED_output(digitToD7led[num],7-d7led_cur);
}
void D7LED_show(ulong num){
	D7LED_showBase(num,10);
}
void D7LED_showBase(ulong num,u8 base){
	u8 i=0;
	while(num){
		d7led_outputBuffer[i]=digitToD7led[num%base];
		num/=base;
		i++;
	}
	if(i==0)d7led_outputBuffer[i++]=digitToD7led[0];
	/**delete the leading 0**/
	for(;i<8;i++)d7led_outputBuffer[i]=0;
}

void D7LED_refresh(){
	d7led_cur++;
	d7led_cur%=8;
	D7LED_output(d7led_outputBuffer[d7led_cur],7-d7led_cur);
}
/***********************end d7led***************/

