#include "header.h"
//#include "led.h"
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

BOOL isKeyPress(uchar i){
	if(keyPressCount[i]>0){
		keyPressCount[i]--;
		return TRUE;
	}
	return FALSE;
}