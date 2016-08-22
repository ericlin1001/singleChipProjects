#include "header.h"
#include "led.h"
#include "key8.h"
/***********************key***************/
static uchar keyPressCount[8];
static uchar lastKeys=0xff;
void processKey(){
	uchar curKeys=Keys;
	uchar i;
	lastKeys=lastKeys & ~curKeys;//trigger by press down.
	for(i=0;i<8;i++){
		if(lastKeys & (1<<i)){
			keyPressCount[i]++;
			if(keyPressCount[i]>KEY_COUNT_MAX*2)keyPressCount[i]=KEY_COUNT_MAX;
		}
	}
	lastKeys=curKeys;
}
BOOL isKeyDown(uchar i){
	return (lastKeys&(1<<i))==0;
}
BOOL isKeyUp(uchar i){
	return (lastKeys&(1<<i))>0;
}
BOOL isKeyRelease(uchar i){
	if(keyPressCount[i]>0 && (lastKeys&(1<<i))>0){
		keyPressCount[i]--;
		return TRUE;
	}
	return FALSE;
}
BOOL isKeyPress(uchar i){
	if(keyPressCount[i]>0){
		keyPressCount[i]--;
		return TRUE;
	}
	return FALSE;
}
void clearKeyCount(){
	uchar i;
	for(i=0;i<8;i++){
		keyPressCount[i]=0;
	}
}
/***********************end key***************/
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

static int num=0x03;
void moveLEDLeft(){
	num=_crol_(num,1);
	LED_show(num);
}
void moveLEDRight(){
	num=_cror_(num,1);
	LED_show(num);
}


static void init(){
num=0x03;
P1=0;
}

static void mainLoop(){
#define SELECT 1

#if SELECT==3
	if(isKeyPress(7)){
		P1++;
		//LED_show(num++);
	}
#endif

#if SELECT==2
	uchar i;
	for(i=0;i<8;i++){
		if(isKeyPress(i)){
			LED_show(1<<i);
		}
	}
#endif
#if SELECT==1
	if(isKeyPress(0)){
		moveLEDLeft();
	}
	if(isKeyPress(1)){
		moveLEDRight();
	}
	if(isKeyDown(2)){
		num++;
		LED_show(num);
		delay(300);
	}
#endif

#if SELECT==0
	uchar i;
	for(i=0;i<8;i++){
		if(isKeyRelease(i)){
			num^=(1<<i);
		}
	}
	LED_show(num);
	return ;
#endif

}

int key8_main(){
#ifdef DO_TEST
test_main();
#endif
	init();
	while(1)
	{
		processKey();
		mainLoop();
	}
	return 0;
}
