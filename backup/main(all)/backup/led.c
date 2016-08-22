#include "header.h"
#include "led.h"
void LED_show(u8 num){
	LED_Port=bit_inv(num);
}

int led_main(void){
	static uchar num=0;
	while(1){
		LED_show(num++);
		delays(1);
	}
	return 0;
}
