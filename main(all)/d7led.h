#ifndef D7LED_H
#define D7LED_H
#include "header.h"
#define d7led_port P2
#define d7led_Select_port P3 //P3.0~2
void D7LED_showc(uchar num);
void D7LED_show(ulong num);
void D7LED_refresh();
#endif