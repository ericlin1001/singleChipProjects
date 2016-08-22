#ifndef __USART_H
#define __USART_H

extern void USART1_Send_Byte(u16 dat);
extern void USART_Send_Str(const char* data);

void USART_Configuration(void);
void USART_Send_Enter(void);
void USART_Put_Inf(char *inf,u32 dat);
void USART_Put_Num(u32 dat);
void u32tostr(u32 dat,char *str); 

#endif
