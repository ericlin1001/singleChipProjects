#ifndef FLASH_H
#define FLASH_H
#include "header.h"
// in manual , flash:0x0800 0000~ 0x0801 ffff.
#define FLASH_START_ADDR  ((u32)0x08008000) //why!! why not start from 0x08000000.
//
void FLASH_init(void);
void FLASH_set(u8 index,u32 data);
u32  FLASH_get(u8 index);
void FLASH_selectPage(u8 page);
//
void FLASH_flush(void);
#endif
