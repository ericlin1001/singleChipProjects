#include "header.h"

void delayus(u16 us){while(us--);}
void delayms(u16 ms){
	uchar j=0;
	while(ms--){j=130;while(j--);}
}
void delays(u16 s){
	while(s--)delayms(1000);
}
void delay(u16 ms){
	delayms(ms);
}

uchar bit_inv(uchar ch){
	uchar res=0;
	uchar i=0;
	for(i=0;i<8;i++){
		res<<=1;
		res+=ch&0x01;
		ch>>=1;
	}
	return res;
}
