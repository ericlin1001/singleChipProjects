 /*********************************************************************
*                                                                     *
*          STM32- 3.2寸TFT液晶屏字符和汉字显示程序                               *
*              （  普中科技  ）                                       *
        
***********************************************************************/

#include "stm32f10x_lib.h"
#include "LCD_driver.h"
#include "touch.h"

/******************************** 变量定义 ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;

/*********************************声明函数 -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Delay(vu32 nCount);
void delay_ms(int count);
void LineDDA(int  x0,int y0,int x1,int y1);
u16 TP_X,TP_Y;
/*******************************************************************************
                                  主函数
*******************************************************************************/
int main(void)
{
unsigned char ii;
//unsigned int TP_X,TP_Y;
		unsigned int lx,ly,temp_x = 0,temp_y = 0 ,flag = 0;
	unsigned long i,j;
	char   X[6] = {0,0,0,0,0,0,},Y[6]= {0,0,0,0,0,0,};
u16   X_Addata,Y_Addata;
 char ss[6];
 int TchScr_Xmax=3859,TchScr_Xmin=26,
   	TchScr_Ymax=3842,TchScr_Ymin=38;//触摸屏返回电压值范围
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

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ; // 选择所有脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //输出的最大频率为50HZ
  GPIO_Init(GPIOB, &GPIO_InitStructure);   //初始化GPIOB端口

   GPIO_Write(GPIOA,0xffff);  //将GPIOA 16个端口全部置为高电平
   GPIO_Write(GPIOB,0xffff);  //将GPIOB 16个端口全部置为高电平

   LCD_Init();
   Delay(150); 
   for(ii=0;ii<7;ii++)
   LCD_clear(ii);	//刷屏测试

    LCD_clear(2);  // 这句是显示背景颜色。
		AD7843(CMD_RDX);
	AD7843(CMD_RDY);

   ChineseChar(60,20,2,colors[5],colors[2],1);	//普
   ChineseChar(90,20,2,colors[5],colors[2],2);	//中
   ChineseChar(120,20,2,colors[5],colors[2],3);//科
   ChineseChar(150,20,2,colors[5],colors[2],4);	//技
   LCD_CS(1);

 /*   LCD_ShowString(0,90,colors[0],colors[5],"WWW.PRECHIN.com");
	LCD_ShowString(0,150,colors[7],colors[2],"www.PRECHIN.net");	   
    LCD_ShowString(12,180,colors[0],colors[7],"TELL:");
    LCD_ShowString(12,210,colors[2],colors[7],"0755-61190227");	
    LCD_ShowString(12,240,colors[2],colors[7],"abcdefJhiJKLMNOP"); 
    LCD_ShowString(12,265,colors[2],colors[7],"pqrstuvwxyz!@#$"); 
    LCD_ShowString(12,290,colors[2],colors[7],"<>CDEF^&*()/,[]"); 	 */
	  while(1)	
   
	{	
		 LCD_CS (0);  //打开片选使能
		 if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
		{		
			 Read_ADS2(&X_Addata,&Y_Addata);
			 inttostr(X_Addata,ss);
			 LCD_ShowString(0,295,0xf800,0xfff0,"X:");
             LCD_ShowString(32,295,0xf800,0xfff0,ss); //显示字符串 	
			
			 inttostr(Y_Addata,ss);
 		 	 LCD_ShowString(120,295,0xf800,0xfff0,"Y:");
             LCD_ShowString(152,295,0xf800,0xfff0,ss);  //显示字符串 	
	        
			//对采样结果进行转换
	    	//lx=(X_Addata)*LCDWIDTH/(TchScr_Xmax-TchScr_Xmin);
			
			j = X_Addata;
			j = j*240;
			i = TchScr_Xmax-TchScr_Xmin	;
	    	lx= j/i;
			if(lx >  250)  lx = 250;
	    	if(lx <= 250)  lx = 250-lx;

			X[0] =	(lx%100000)/1000 + 0x30;
		    X[1] =	(lx%10000)/1000 + 0x30;
			X[2] =	(lx%1000)/100 + 0x30;
			X[3] =	(lx%100)/10 + 0x30;
			X[4] =	lx%10 + 0x30;
			LCD_ShowString(0,270,0xf800,0xfff0,"x:"); //显示LCD像素点X坐标
		    LCD_ShowString(32,270,0xf800,0xfff0,X);  //显示字符串 	
			
		//	Y_Addata = 3634;
		    j = Y_Addata;
			j = (j*413);
			i = TchScr_Ymax-TchScr_Ymin;
			ly=j/i;
			if(ly >  413)  ly = 413;
	    	if(ly <= 413)  ly = 413 -ly;

		
			
			Y[0] =	(ly%100000)/10000 + 0x30;
			Y[1] =	(ly%10000)/1000 + 0x30;
			Y[2] =	(ly%1000)/100 + 0x30;
			Y[3] =	(ly%100)/10 + 0x30;
			Y[4] =	ly%10 + 0x30;
			LCD_ShowString(120,270,0xf800,0xfff0,"y:");//显示LCD像素点Y坐标
			LCD_ShowString(152,270,0xf800,0xfff0,Y);  //显示字符串 	


			 if (flag)
			 {
			    LineDDA(temp_x,temp_y,lx,ly);
			 }
			 		 if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
					 {flag=1;}
					 else
					 {flag=0;}

	//	   flag = 1;	
		   temp_x = lx;
		   temp_y = ly;

		}	
		
	LCD_CS (1);  //关闭片选使能	

   	 }
    
}	 
void LineDDA(int x0,int y0,int x1,int y1)
{
    float dy,dx,x,y,m;
    dx=x1-x0;
    dy=y1-y0;
    m=dy/dx;
    if(x0<x1)
    {
        if(m<=1&&m>=-1)
        {
              y=y0;
              for(x=x0;x<=x1;x++)
              {
                   LCD_SetRamAddr(x-1,x+1-1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff);  
				   LCD_SetRamAddr(x,x+1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff); 
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x-1,x+1-1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff);
                   LCD_SetRamAddr(x,x+1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff);  
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff); 
				   LCD_SetRamAddr(x-1,x+1-1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x,x+1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
                  y+=m;
              }
        }
    }
    if(x0>x1)
    {
        if(m<=1&&m>=-1)
        {
              y=y0;
              for(x=x0;x>=x1;x--)
              {
                   LCD_SetRamAddr(x-1,x+1-1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff);  
				   LCD_SetRamAddr(x,x+1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff); 
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x-1,x+1-1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff);
                   LCD_SetRamAddr(x,x+1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff);  
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff); 
				   LCD_SetRamAddr(x-1,x+1-1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x,x+1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
                  y-=m;
              }
        }
    }
    if(y0<y1)
    {
        if(m>=1||m<=-1)
        {
              m=1/m;
              x=x0;
              for(y=y0;y<=y1;y++)
              {
              
                   LCD_SetRamAddr(x-1,x+1-1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff);  
				   LCD_SetRamAddr(x,x+1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff); 
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x-1,x+1-1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff);
                   LCD_SetRamAddr(x,x+1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff);  
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff); 
				   LCD_SetRamAddr(x-1,x+1-1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x,x+1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
                  x+=m;
              }
        }
    }
    if(y0>y1)
    {
        if(m<=-1||m>=1)
        {
              m=1/m;
              x=x0;
              for(y=y0;y>=y1;y--)
              {
             
                   LCD_SetRamAddr(x-1,x+1-1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff);  
				   LCD_SetRamAddr(x,x+1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff); 
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5-1,y+1+0.5-1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x-1,x+1-1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff);
                   LCD_SetRamAddr(x,x+1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff);  
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5,y+1+0.5);
	              LCD_Write_Data16(0xffff); 
				   LCD_SetRamAddr(x-1,x+1-1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x,x+1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
				   LCD_SetRamAddr(x+1,x+1+1,y+0.5+1,y+1+0.5+1);
	              LCD_Write_Data16(0xffff);
                  x-=m;
              }
        }
    }
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
  for(; nCount != 0; nCount--);
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
