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
void mydelay(u16 us)   //delay 10us
{//delay 10us
unsigned char a,b;
if(us<=0)return;
while(us--){
    for(b=1;b>0;b--)
        for(a=2;a>0;a--);
		}
}