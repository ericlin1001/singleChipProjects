#ifndef LED_H
#define LED_H
#include "header.h"
//
void D7LED_init(void);
void D7LED_show(ulong num);
void D7LED_refresh(void);
//
void D7LED_showBase(ulong num,u8 base);
void D7LED_output(u8 data,u8 select);
void D7LED_showc(u8 num);
void D7LED_showBase(ulong num,u8 base);

#endif
