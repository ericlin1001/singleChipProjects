#include "LCD.h"
#include "key8.h"
#include "led.h"
static ulong mainLoopCount=0;

                                                                         
/***************LCD 1602**************/
bit LCD_busy()                                                             
{                                                                          
    bit result;                                                            
    LCD_RS = 0;                                                            
    LCD_RW = 1;                                                            
    LCD_EN = 1;                                                            
    _nop_();                                                               
    _nop_();                                                               
    _nop_();                                                               
    _nop_();                                                               
    result = (bit)(P0&0x80);                                              
    LCD_EN = 0;                                                            
    return result;                                                         
} 
 
void LCD_wcmd(uchar cmd)                                                   
{                                                                          
   while(LCD_busy());                                                      
    LCD_RS = 0;                                                            
    LCD_RW = 0;                                                            
    LCD_EN = 0;                                                                                                                      
    P0 = cmd;                                                                                                                         
    LCD_EN = 1;                                                
    _nop_();                                                               
    _nop_();                                                               
    _nop_();                                                               
    _nop_();                                                               
    LCD_EN = 0; 
}  
void LCD_wdat(uchar dat)                                                   
{                                                                          
   while(LCD_busy());                                                      
    LCD_RS = 1;                                                            
    LCD_RW = 0;                                                            
    LCD_EN = 0;                                                            
    LCD_DATA = dat;                                                                                                                             
    LCD_EN = 1;                                                            
    _nop_();                                                               
    _nop_();                                                               
    _nop_();                                                               
    _nop_();                                                               
    LCD_EN= 0; 
}
/****/
void LCD_clear()                                                             
{                                                                          
    LCD_wcmd(0x01);  
	delayus(2);	
}  
void LCD_reset()                                                             
{                                                                          
    LCD_wcmd(0x02);                       
    delayus(2);                                                              
}
void LCD_ctl(BOOL screen,BOOL cursor,BOOL twinkle){
	LCD_wcmd((LCD_CMD_CURORCTL)|((uchar)screen<<2)|((uchar)cursor<<1)|((uchar)twinkle<<0));
	delayus(40); 
}
void LCD_offset(BOOL cursorScreen,BOOL leftRight){
	LCD_wcmd((LCD_CMD_MOVE)|((uchar)cursorScreen<<3)|((uchar)leftRight<<2));
	delayus(40);
}
void LCD_pos(uchar pos,uchar row)                                                    
{   
if(row==0)LCD_wcmd(pos|LCD_CMD_DDRAM);      //数据指针=80+地址变量  
 else LCD_wcmd((pos+0x40)|LCD_CMD_DDRAM);
	delayus(40);   
} 
void LCD_put(uchar ch){
	LCD_wdat(ch);
	delayus(40);
}
void LCD_show(char *str){
uchar len=0;
LCD_pos(0,0);
while(*str&&len<16){LCD_put(*str++);len++;}
if(*str){
LCD_pos(0,1);
len=0;
while(*str&&len<16){LCD_put(*str++);len++;}
}
}
/*************/                                                                       
void LCD_init()                                                            
{                                                                          
    delayms(10);                     //等待LCD电源稳定                                                                              
    LCD_wcmd(LCD_CMD_FUNCSET|(1<<4)|(1<<3)|(1<<2));  
	delayus(40);	                                                                                                                                 
    LCD_ctl(TRUE,FALSE,FALSE);                                                                                      
    LCD_wcmd(LCD_CMD_INPUTMETHOD|(1<<1)|(0<<0));//移动光标 
	delayus(40);	                                                            
    LCD_clear();
	LCD_reset();
	LCD_pos(0x00,1); 	
}                                                                          
                                                                   
void LCD_flash()                                                               
{                                                                          
    delayms(600);                     //控制停留时间                         
    LCD_ctl(FALSE,FALSE,FALSE);            //关闭显示                                  
    delayms(200);                    //延时                                  
    LCD_ctl(TRUE,FALSE,FALSE);           //开显示                                    
    delayms(200);                                                            
    LCD_ctl(FALSE,FALSE,FALSE);          //关闭显示                                  
    delayms(200);                     //延时                                 
    LCD_ctl(TRUE,FALSE,FALSE);            //开显示                                    
    delayms(200);                                                            
}                                                                          
                                                                           
/***************end LCD**************/


#define _DO_TEST
/**************test some thing****/
#ifdef main
#undef main
#endif
#define main static test_main
#ifdef main

                                                                       
		
#endif
#ifdef main
#undef main
#endif
/************end test****/


#define INT0_CALLBACK_FUNC updater
#include "timer.h"
void updater(){
	processKey();
}

static void init(){
	INT0_init();
	LCD_init(); 
}

static void mainLoop(){
#define SELECT 1

#if SELECT==1
uchar i;   
LCD_show("Press any key, e.g.K1. --By Eric");
for(i=0;i<8;i++){
	if(isKeyPress(i)){
		LCD_clear();
		LCD_show("This is Key");
		LCD_put(i+'0');
		LCD_put('!');
		delayms(500);
	}
}

#endif

#if SELECT==2
	
#endif
#if SELECT==0
	
#endif
}


int LCD_main(){
#ifdef DO_TEST
test_main();
#endif
	mainLoopCount=0;
	init();
	while(1)
	{
		
		mainLoop();
		mainLoopCount++;
	}
	return 0;
}
