#include "header.h"
#include "video.h"
//#include "led.h"
#include "key8.h"
ulong mainLoopCount=0;

/***********************LCD***************/

/************application interface******/
u16 LCD_bkColor=LCD_RGB_BALCK;
u16 LCD_fgColor=LCD_RGB(0xffffff);
u8 curX=0,curY=0;
BOOL isShowCursor=FALSE;
/*****/
u16 LCD_getBkColor(){return LCD_bkColor;}
u16 LCD_getTextColor(){return LCD_fgColor;}
void LCD_setTextColor(u16 color){LCD_fgColor=color;}
void LCD_setBkColor(u16 color){LCD_bkColor=color;}
void LCD_init(void){
P2 = 0xff;
LCD_DATA = 0xff;
LCD_basic_init();
LCD_clear(LCD_bkColor);
}
void nextLine(){
		curY++;
		if(curY>=LCD_CHAR_LEN_Y){
			curY=0;
		}
		curX=0;
}

void LCD_putChar(char ch){
	LCD_ShowChar((uint)curX*CHAR_W,(uint)curY*CHAR_H,LCD_fgColor,LCD_bkColor,ch);
	curX++;
	if(curX>=LCD_CHAR_LEN_X){
		curX-=LCD_CHAR_LEN_X;
		curY++;
		if(curY>=LCD_CHAR_LEN_Y){
			curY=0;
		}
	}
	if(isShowCursor)LCD_ShowChar((uint)curX*CHAR_W,(uint)curY*CHAR_H,LCD_fgColor,LCD_bkColor,'_');
}

void LCD_printf(char *str){
while(*str){
if(*str=='\n'){
LCD_ShowChar((uint)curX*CHAR_W,(uint)curY*CHAR_H,LCD_fgColor,LCD_bkColor,' ');
nextLine();
str++;
}else{
LCD_putChar(*str++);
}
}
}
/************end application interface******/

/************middle-level interface******/
 void  LCD_basic_init(void)
{
	LCD_RST=1;
	delayms(5);	
	LCD_RST=0;
	delayms(5);
	LCD_RST=1;
	delayms(5);
	LCD_CS =0;  //打开片选使能
//

    LCD_Reg_write(0x000,0x0000); delayms(5);
	LCD_Reg_write(0x000,0x0000); delayms(5);
	LCD_Reg_write(0x000,0x0000);delayms(5);
	LCD_Reg_write(0x000,0x0000); delayms(5);
	delayms(100);
       LCD_writeData(0x0000);delayms(5);
    LCD_writeData(0x0000);delayms(5);	
    LCD_writeData(0x0000);delayms(5);	
    LCD_writeData(0x0000); delayms(5);
  	delayms(100);


	LCD_Reg_write(0x400,0x6200);delayms(5);
 	LCD_Reg_write(0x008,0x0808); delayms(5);
//	LCD_Reg_write(0x010,0x0010);


	LCD_Reg_write(0x300,0x0c0c);delayms(5);//GAMMA
	LCD_Reg_write(0x301,0xff13); delayms(5);
	LCD_Reg_write(0x302,0x0f0f); delayms(5);
	LCD_Reg_write(0x303,0x150b); delayms(5);

	LCD_Reg_write(0x304,0x1020); delayms(5);

	LCD_Reg_write(0x305,0x0a0b); delayms(5);
	LCD_Reg_write(0x306,0x0003);delayms(5);	
	LCD_Reg_write(0x307,0x0d06);  delayms(5);
	LCD_Reg_write(0x308,0x0504);  delayms(5);
	LCD_Reg_write(0x309,0x1030);  delayms(5);

	LCD_Reg_write(0x010,0x001b);delayms(5);   //60Hz
 	LCD_Reg_write(0x011,0x0101);delayms(5);
	LCD_Reg_write(0x012,0x0000);delayms(5);
	LCD_Reg_write(0x013,0x0001);delayms(5);

	LCD_Reg_write(0x100,0x0330);delayms(5);//BT,AP 0x0330
	LCD_Reg_write(0x101,0x0247);delayms(5);//DC0,DC1,VC
	LCD_Reg_write(0x103,0x1000);delayms(5);//VDV	//0x0f00
	LCD_Reg_write(0x280,0xbf00);delayms(5);//VCM
	LCD_Reg_write(0x102,0xd1b0);delayms(5);//VRH,VCMR,PSON,PON

	delayms(1220);

	LCD_Reg_write(0x001,0x0100);delayms(5);
	LCD_Reg_write(0x002,0x0100);delayms(5);
	LCD_Reg_write(0x003,0x1030); delayms(5);
	LCD_Reg_write(0x009,0x0001);delayms(5);
	LCD_Reg_write(0x0C,0x0000);delayms(5);	//MCU interface
	LCD_Reg_write(0x090,0x8000);delayms(5);
	LCD_Reg_write(0x00f,0x0000);delayms(5);


   	LCD_Reg_write(0x210,0x0000);delayms(5);
	LCD_Reg_write(0x211,0x00ef);delayms(5);
	LCD_Reg_write(0x212,0x0000);delayms(5);
	LCD_Reg_write(0x213,0x018f); delayms(5);

	LCD_Reg_write(0x500,0x0000); delayms(5);
	LCD_Reg_write(0x501,0x0000); delayms(5);
	LCD_Reg_write(0x502,0x005f); delayms(5);
	LCD_Reg_write(0x401,0x0001); delayms(5);
	LCD_Reg_write(0x404,0x0000); delayms(5);
	delayms(500);

   	LCD_Reg_write(0x0007,0x0100);delayms(5);
	delayms(1000);

   	LCD_Reg_write(0x200,0x0000);delayms(5);
	LCD_Reg_write(0x201,0x0000);delayms(5);
   delayms(500);

  	LCD_WriteIR(0x0202);
  delayms(500);

}
void LCD_areaSelect(uint xStart, uint xEnd, uint yStart, uint yEnd) 
{

	//set the start RAM address
	LCD_Reg_write(0x200, xStart); 
    LCD_Reg_write(0x201, yStart); 

	//set the window area
    LCD_Reg_write(0x0210, xStart);
	LCD_Reg_write(0x211,xEnd);
    LCD_Reg_write(0x0212,yStart);
    LCD_Reg_write(0x213,yEnd);


	LCD_WriteIR(0x0202);
}
void  LCD_clear(u16 color)
{
  uint i;
   uint j;
	LCD_areaSelect(0,LCD_SIZE_X-1, 0,LCD_SIZE_Y-1);
	for(i=0;i<LCD_SIZE_X;i++){
		for(j=0;j<LCD_SIZE_Y;j++)
		LCD_writeData(color);
	}
}

void ChineseChar(uint x,uint y,int size,uint For_color,uint Bk_color ,char c)
{
   int e=0,i,j;
   int  ed;
   
   uint  ncols;
   uint  nrows;
   uint  nbytes;

   uchar *pfont;
   uchar *pchar;
   uchar *fonttable[]={(uchar *)hz12,(uchar *)hz16,(uchar *)hz24};
   
   pfont=(uchar *)fonttable[size];

   if(size==0)
   {
     ncols =12-1;
     nrows =12-1;
     nbytes =24;	 //(12*12)/72
   }
   else
   if(size==1)
   {
     ncols =16-1;
     nrows =16-1;
     nbytes =32;  //(16*16)/72
   }
   else
   if(size==2)
   {
     ncols =24-1;
     nrows =24-1;
     nbytes =72;   //(24*24)/72
   }
   
   pchar=pfont + (nbytes*(c-1));

   LCD_areaSelect(x, x+nrows, y,y+ncols);
   
   for(i=0;i<nbytes;i++)
   {
     ed=*pchar++;
	 if(size==0)
	 {
       if(i%2==0)
       e=8;
       else
       e=4;
	 }
	 else
	 e=8;
     
     for(j=0;j<e;j++)
     if((ed>>j)&0x01)
     {
       LCD_writeData(For_color);  //textcolor
	 }
     else
     {
       LCD_writeData(Bk_color);  //backcolor
	 }
  }
}
void LCD_ShowChar(uint x,uint y,uint For_color,uint Bk_color, char ch)
{       
   uchar temp;
   uchar pos,t;
   /*if(x>(LCD_SIZE_X-CHAR_W)||y>(LCD_SIZE_Y-CHAR_H))
   return;*/
   LCD_areaSelect(x, x+CHAR_W-1, y,y+CHAR_H-1);
   ch = ch-32;                       //按照ASCII编码顺序的到相应字母的编码
   for(pos=0;pos<CHAR_H;pos++)  
   {
      temp= Font8x16[ch][pos];
      for(t=0;t<CHAR_W;t++)
      {                 
	    if(temp&0x80)
	    LCD_writeData(For_color);  //textcolor
	    else 
	    LCD_writeData(Bk_color);  //backcolor
        temp<<=1; 
      } 
   }
}  
/************end middle-level interface******/

/***********hardware-level interface***/
void LCD_WriteIR(uint value)  
{
	LCD_CS = 0;
	LCD_RS 	= 0;    		// 0: command    1: LCD_DATA
	
	LCD_WR  = 0;

    LCD_DATA 	= (uchar)(value>>8)	;
	LCD_WR  = 1;
	_nop_();
	LCD_WR  = 0;

	LCD_DATA 	= (uchar)value;
    LCD_WR 	= 1;
_nop_();
	LCD_CS = 1;
}

void LCD_writeData(uint value)    // color LCD_DATA
{
/************2-transfer mode*
D15 ... D0 (16bit)
is extended to 
R5 ... R1  R0 | G5 ...G1 G0 | B5...B1 B0 |(RGB-18bit)
D15... D11 D15| D10...D6 D5 | D4...D0 D4 |
that is R0=D15,B0=D4
*****
D15... D11  D10  .. D6 D5 D4...D0
R5...  R1   G5   	G1 G0 B5   B1	
******/
	LCD_CS = 0;
    LCD_RS 	= 1;
	LCD_WR  = 0;
	LCD_DATA 	= (uchar)(value>>8)	;
	LCD_WR  = 1;
	LCD_WR  = 0;
	LCD_DATA 	= (uchar)value;
	LCD_WR 	= 1;
	LCD_CS = 1;
}

void LCD_Reg_write(uint reg,uint value) 
{
   LCD_WriteIR(reg);
   LCD_writeData(value);
}

/*************end hardware-level interface****************/

/***********************end LCD***************/
#define _DO_TEST
/**************test some thing****/
#ifdef main
#undef main
#endif
#define main test_main
#ifdef main





/*********************************************************/

#endif
#ifdef main
#undef main
#endif
/************end test****/

static void init(){
	unsigned  char i; 
	u16 color;
	 LCD_init();

	 //
	 for(i=1;i<10;i++){
		ChineseChar((uint)curX++*CHAR_H,(uint)curY*CHAR_H,0,LCD_RGB(0x00ff00),LCD_bkColor,i);
	 }
	 nextLine();
	 //
	 
	LCD_printf("hello world\n");
	LCD_printf("I'm eric.!\n");
	//
	color=LCD_getTextColor();
	LCD_setTextColor(LCD_RGB(0xff0000));
	LCD_printf("Nice to meet you!\n");
	 LCD_setTextColor(color);
	 //
	 LCD_printf("Actually,this is a test program.But i would like to find if there exist any problems.\n");
	 LCD_printf("\n\nAwesome! I like this one! \n(*^-^*) \n");
	
	 
}
uchar strlen(char *str){
uchar len=0;
while(*str++)len++;
return len;
}

static void mainLoop(){
#define SELECT 1
static uchar state=0;
static uchar chars[]="  .-|\/_<>?~!+*#";
#if SELECT==1
uchar i;
if(mainLoopCount%1000==0){
state=(++state)%strlen(chars);
}
LCD_putChar(chars[state]);
for(i=0;i<8;i++){
if(isKeyPress(i)){
LCD_printf("\nKey");
LCD_putChar(i+'0');
LCD_printf(" is pressed!\n");
}

}
#endif

#if SELECT==2
	
	
#endif


#if SELECT==0
	
#endif

}

int video_main(){
#ifdef DO_TEST
test_main();
#endif
	init();
	while(1)
	{
		mainLoopCount++;
		processKey();
		mainLoop();
	}
	return 0;
}
