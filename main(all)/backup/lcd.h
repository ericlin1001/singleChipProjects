#ifndef D7LED_H
#define D7LED_H
#include "header.h"
/***************LCD 1602**************/
#define LCD_DATA P0                                                         
#define LCD_RS   P2_6    //register select. 0:control, 1:data                                                  
#define LCD_RW   P2_5   // 0:write,1:read.                                                 
#define LCD_EN   P2_7   // in reading: E=1 is active, in writing,E=1->0 is active.                                                     
                               
/******LCD1602 command set***/
#define LCD_CMD_CLEAR 		 1<<0  //clear DDRAM,AC
#define LCD_CMD_RESET 		 1<<1  //AC=0,cursor,screen reset to HOME.
#define LCD_CMD_INPUTMETHOD  1<<2 //I/D,S	(I/D:AC dec~inc after R/W; S:stay~move after R/W)(However in manual:S:move~stay after R/W)
#define LCD_CMD_CURORCTL	 1<<3 //D,C,B	(D:show off~on; C:cursor off~on; twinkle:off~on)
#define LCD_CMD_MOVE		 1<<4 //S/C,R/L,*,* (R/L:left->right; S/C:cursor~screen move)
#define LCD_CMD_FUNCSET		 1<<5 //DL,N,F,*,*  (DL:4bit-data~8bit-data;N:1-row~2-row;F:5*7char~5*10char
#define LCD_CMD_CGRAM		 1<<6 //A5-A0
#define LCD_CMD_DDRAM		 1<<7 //A6-A0
/******end command set***/

/******usefule function*******/                                                                       
void LCD_init();
void LCD_pos(uchar pos,uchar row);   
void LCD_put(uchar ch);
void LCD_show(char *str);    //this combine LCD_pos and LCD_put, it will rewrite the data!                                                                                                                                 
void LCD_flash();
/******end usefule function*******/

/***low level interface***/
bit LCD_busy();
void LCD_wcmd(uchar cmd);
void LCD_wdat(uchar dat);
/***end low level interface***/

/******command set wrapper********/
void LCD_clear();
void LCD_reset();
void LCD_ctl(BOOL screen,BOOL cursor,BOOL twinkle);
void LCD_offset(BOOL cursorScreen,BOOL leftRight);
void LCD_pos(uchar pos,uchar row) ;
/******end command set wrapper********/

#endif