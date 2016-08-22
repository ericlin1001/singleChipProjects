#include <reg51.h>
#include <intrins.h>
typedef unsigned char uchar;
typedef unsigned char u8;
typedef unsigned int u16;

void delayms(u16 ms){
	uchar j=0;
	while(ms--)while(--j);
}
void delays(u16 s){
	while(s--)delayms(1000);
}

#define LED_Port P2
 void LED_show(u8 num){
	LED_Port=num;
}

void LED_test(){
	u8 num=0x33;
	while(1){
		LED_show(num);
		delays(1);
		num=_crol_(num,1);
	/*	if(num&0x01){
			num>>=1;
			num|=0x80;
		}else{
			num>>=1;
		}  */
	}
}
int main(void){
	LED_test();
	return 0;
}
