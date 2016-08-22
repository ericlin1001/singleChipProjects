 /*********************************************************************
*                                                                     *
*          STM32- 3.2寸TFT液晶屏字符和汉字显示程序                               *
*              （  普中科技  ）                                       *
        
***********************************************************************/

#include "stm32f10x_lib.h"
#include "LCD_driver.h"


/******************************** 变量定义 ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;

/*********************************声明函数 -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Delay(vu32 nCount);


/*******************************************************************************
                                  主函数
*******************************************************************************/
void delay(u32 ms){
	ms*=72*500;
	while(ms--);
}


//u16 LCD_buffer[400][240];

#define MY_HEIGHT 100
#define MY_WIDTH 60
u16 LCD_buffer[MY_HEIGHT+1][MY_WIDTH+1];
#include "header.h"
void LCD_refresh();
void myinit(void ){
	sysTickInit();
	//registerTick(LCD_refresh,1000/25);
}
void mydelayms(u16 ms){
	u32 endTime=getTime()+ms;
	while(runTimeMs<endTime);
}
void fillColor(u16 c){
	u32 i,j;
	for(i=0;i<MY_HEIGHT+1;i++)
  {
		for(j=0;j<MY_WIDTH+1;j++){
			LCD_buffer[i][j]=c;
		}
	}
}

#define LCD_6BIT_RGB(r,g,b) ((((r)>>1)<<11)|((g)<<6)|((b)>>1))
#define LCD_8BIT_RGB(r,g,b) LCD_6BIT_RGB((r)*0x3f/0xff,(g)*0x3f/0xff,(b)*0x3f/0xff)
#define LCD_RGB(v) LCD_8BIT_RGB((v>>16)&0xff,(v>>8)&0xff,(v)&0xff)

#define rgb888(r,g,b) ((((r)&0xff)<<16)|(((g)&0xff)<<8)|(((b)&0xff)))
#define rgb565Torbg888Scale(r,g,b) rgb888((((r)<<1)|(((r)>>4)&0x1))*0xff/0x3f,(g)*0xff/0x3f,(((b)<<1)|(((b)>>4)&0x1))*0xff/0x3f)
#define rgb565Torgb888(c) (rgb565Torbg888Scale(((c)>>11)&0x1f,((c)>>5)&0x3f,(c)&0x1f))

#define rbg_r(v) ((v>>16)&0xff)
#define rbg_g(v) ((v>>8)&0xff)
#define rbg_b(v) ((v)&0xff)

/*
#define rgb565scale_r(r) ((((r)<<1)|(((r)>>4)&0x1))*0xff/0x3f)
#define rgb565_r(c) (rgb565scale_r(((c)>>11)&0x1f))

#define rgb565scale_g(g) (g)*0xff/0x3f
#define rgb565_g(c) (rgb565scale_g(((c)>>5)&0x3f))

#define rgb565scale_b(b) (((b)<<1)|(((b)>>4)&0x1))*0xff/0x3f
#define rgb565_b(c) (rgb565scale_b(((c)&0x1f))
*/
void setPixel(u16 x,u16 y,u16 color){
	LCD_buffer[y][x]=color;
}
u16 mycolor=0x0000;
#define abs(x) (((x)>0)?(x):(-(x)))
void drawLine(s16 x,s16 y,s16 xend,s16 yend){
	s16 tx,ty;
	s16 mu=max(abs(x-xend),abs(y-yend));
	s16 p;
	for(p=0;p<=mu;p++){
		tx=(x*p+xend*(mu-p))/mu;
		ty=(y*p+yend*(mu-p))/mu;
		setPixel(tx,ty,mycolor);
	}
}
void drawSquare(u16 x,u16 y,u16 size){
	u16 dx,dy;
/*	x%=100-size;
	y%=60-size;*/
	for(dx=0;dx<=size;dx++){
		for(dy=0;dy<=size;dy++){
			setPixel(x+dx,y+dy,mycolor);
		}
	}
}

void mytest(){
u16 x=0;	
	u16 y=0;
	myinit();
	fillColor(LCD_RGB(0xffffff));
	mycolor=LCD_RGB(0xff0000);
	
	while(1){
		x+=3;
		if(x%95==0)y++;
		fillColor(LCD_RGB(0xffffff));
		drawSquare(x,y,5);
		LCD_refresh();
		//mydelayms(200);
	}
	
/*	mycolor=LCD_RGB(0xff0000);
	
	drawLine(10,10,40,30);
	mycolor=LCD_RGB(0x00ff00);
	drawLine(50,2,3,70);
	
	mycolor=LCD_RGB(0x0000ff);
	
	drawLine(50,2,3,3);
	
	mycolor=LCD_RGB(0xffff00);
	*/
	drawLine(50,8,3,3);
	while(1);
}

u16 getPixel(s16 x,s16 y){	
	x=max(0,x);
	y=max(0,y);
	x=min(240-1,x);
	y=min(400-1,y);
	return LCD_buffer[y/4][x/4];
}

u16 getFilterPixel(s16 x,s16 y){		
	
u8 filter[3][3]={{1,2,1},
									{2,3,2},
									{1,2,1}};
s8 dx,dy;
u32 color=0;
u32 r=0,g=0,b=0;
u8 sum=0;
	x=max(0,x);
	y=max(0,y);
	x=min(240-1,x);
	y=min(400-1,y);

for(dx=-1;dx<=1;dx++){
	for(dy=-1;dy<=1;dy++){
			//color+=rgb565Torgb888(getPixel(x+dx,y+dy))*filter[dy+1][dx+1];
		color=rgb565Torgb888(getPixel(x+dx,y+dy));
		r+=rbg_r(color)*filter[dy+1][dx+1];
		g+=rbg_g(color)*filter[dy+1][dx+1];
		b+=rbg_b(color)*filter[dy+1][dx+1];
			sum+=filter[dy+1][dx+1];
	}
}
//color/=sum;
r/=sum;
g/=sum;
b/=sum;
color=rgb888(r,g,b);
//return LCD_RGB(rgb565Torgb888(getPixel(x,y)));
	return LCD_RGB(color);
}





#define sqrt(x) ((x)*(x))
void LCD_refresh(void)
{
  s32 i,j;
	#define TILE_WIDTH 4
	#define TILE_HEIGHT 4
  LCD_SetRamAddr(0,240-1, 0,400-1);
  for(i=0;i<400;i++)//y
  {
		for(j=0;j<240;j++){//x
			LCD_Write_Data16(getPixel(j,i));
		}
  }
}
/*void LCD_refresh(void)
{
  u32 i,j;
	#define TILE_WIDTH 4
	#define TILE_HEIGHT 4
  LCD_SetRamAddr(0,MY_WIDTH, 0,MY_HEIGHT);
  for(i=0;i<MY_HEIGHT;i++)
  {
		for(j=0;j<MY_WIDTH;j++){
			//LCD_Write_Data16(LCD_buffer[i][j]);
			u32 tr,tc;
			LCD_SetRamAddr(j*TILE_WIDTH,(j+1)*TILE_WIDTH-1,i*TILE_HEIGHT,(i+1)*TILE_HEIGHT-1);
			//
			{
			u16 c1=LCD_buffer[i][j];
					u16 c2=LCD_buffer[i+1][j];
					u16 c3=LCD_buffer[i][j+1];
					u16 c4=LCD_buffer[i+1][j+1];
			
			for(tr=0;tr<TILE_HEIGHT;tr++){
				for(tc=0;tc<TILE_WIDTH;tc++){
					u16 color;
					u16 p4=sqrt(tr)+sqrt(tc);
					u16 p3=sqrt(TILE_HEIGHT-1-tr)+sqrt(tc);
					u16 p2=sqrt(tr)+sqrt(TILE_WIDTH-1-tc);
					u16 p1=sqrt(TILE_HEIGHT-1-tr)+sqrt(TILE_WIDTH-1-tc);
					
					
					LCD_Write_Data16((p1*c1+p2*c2+p3*c3+p4*c4)/(p1+p2+p3+p4));
					//LCD_Write_Data16(c1);
						//LCD_Write_Data16(LCD_buffer[i][j]);
				}
			}
		}
		}
  }
}*/
int main(void)
{
	u8 k=0;
unsigned char ii;
#ifdef DEBUG
  debug();
#endif

   RCC_Configuration();      //系统时钟配置函数   

   NVIC_Configuration();     //NVIC配置函数

    //启动GPIO模块时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    //把调试设置普通IO口
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);  
                            
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;	 //所有GPIO为同一类型端口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //输出的最大频率为50HZ
  GPIO_Init(GPIOA, &GPIO_InitStructure);   //初始化GPIOA端口
  GPIO_Init(GPIOB, &GPIO_InitStructure);   //初始化GPIOB端口

	delay(500);
   GPIO_Write(GPIOA,0xffff);  //将GPIOA 16个端口全部置为高电平
   GPIO_Write(GPIOB,0xffff);  //将GPIOB 16个端口全部置为高电平

   LCD_Init();
mytest();
	
while(1){
	k++;
	k%=5;
	fillColor(colors[k]);
	mydelayms(500);
}
    for(ii=0;ii<7;ii++)
    LCD_clear(ii);	//刷屏测试
    LCD_clear(2);  // 这句是显示背景颜色。


   ChineseChar(60,20,2,colors[5],colors[2],1);	//普
   ChineseChar(90,20,2,colors[5],colors[2],2);	//中
   ChineseChar(120,20,2,colors[5],colors[2],3);//科
   ChineseChar(150,20,2,colors[5],colors[2],4);	//技

    LCD_ShowString(0,90,colors[0],colors[5],"WWW.PRECHIN.com");
	LCD_ShowString(0,150,colors[7],colors[2],"www.PRECHIN.net");	   
    LCD_ShowString(12,180,colors[0],colors[7],"TELL:");
    LCD_ShowString(12,210,colors[2],colors[7],"0755-61190227");	
    LCD_ShowString(12,240,colors[2],colors[7],"abcdefJhiJKLMNOP"); 
    LCD_ShowString(12,265,colors[2],colors[7],"pqrstuvwxyz!@#$"); 
    LCD_ShowString(12,290,colors[2],colors[7],"<>CDEF^&*()/,[]"); 
   
   while(1);
    
}


/*******************************************************************************
*                           配置RCC
*******************************************************************************/
void RCC_Configuration(void)
{   
 //复位RCC外部设备寄存器到默认值
  RCC_DeInit();

  //打开外部高速晶振
  RCC_HSEConfig(RCC_HSE_ON);

   //等待外部高速时钟准备好
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)   //外部高速时钟已经准别好
  {								    
    //开启FLASH的预取功能
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    //FLASH延迟2个周期
    FLASH_SetLatency(FLASH_Latency_2);
 	
  //配置AHB(HCLK)时钟=SYSCLK
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  
  
   //配置APB2(PCLK2)钟=AHB时钟
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    //配置APB1(PCLK1)钟=AHB 1/2时钟
    RCC_PCLK1Config(RCC_HCLK_Div2);

     //配置PLL时钟 == 外部高速晶体时钟*9  PLLCLK = 8MHz * 9 = 72 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

		SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
		
    //使能PLL时钟
    RCC_PLLCmd(ENABLE);

   //等待PLL时钟就绪
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

  //配置系统时钟 = PLL时钟
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

   //检查PLL时钟是否作为系统时钟
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}

/*******************************************************************************
*                             NVIC配置函数
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
}

/*******************************************************************************
*                              延时函数
*******************************************************************************/
void Delay(vu32 nCount)
{
  //for(; nCount != 0; nCount--);
}



#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
