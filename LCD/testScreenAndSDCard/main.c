 /*********************************************************************
*                                                                     *
*          STM32- 3.2寸TFT液晶屏字符和汉字显示程序                               *
*              （  普中科技  ）                                       *
        
***********************************************************************/

#include "stm32f10x_lib.h"
#include "LCD_driver.h"
#include "MMC.h"


/******************************** 变量定义 ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;


unsigned char  csd_data[16];
unsigned char  cid_data[16];

unsigned char by;
unsigned int c_size, c_size_mult, read_bl_len;
unsigned long drive_size;
unsigned long   size_mb;

/*********************************声明函数 -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Delay(vu32 nCount);


/*******************************************************************************
                                  主函数
*******************************************************************************/
int main(void)
{

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

   GPIO_Write(GPIOA,0xffff);  //将GPIOA 16个端口全部置为高电平
   GPIO_Write(GPIOB,0xffff);  //将GPIOB 16个端口全部置为高电平

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	 //所有GPIO为同一类型端口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //输出的最大频率为50HZ
  GPIO_Init(GPIOA, &GPIO_InitStructure);   //初始化GPIOA端口

   LCD_Init();
   LCD_clear(2);  // 这句是显示背景颜色。
   T_CS(1);	
    
    LCD_CS(0);
    LCD_ShowString(70,5,colors[4],colors[7],"SD/MMC");
    ChineseChar(32+30,50,1,colors[2],colors[7],1);	//程
    ChineseChar(64+30,50,1,colors[2],colors[7],2);	//序
    ChineseChar(96+30,50,1,colors[2],colors[7],3);	//测
    ChineseChar(128+30,50,1,colors[2],colors[7],4); //试
	LCD_CS(1);

	MMC_Init(); //初始化SD卡
    if(Init_Flag == 0)
   { LCD_CS(0);

  	 LCD_ShowString(20,90,colors[0],colors[7],"INIT SUCCESS");
	 LCD_ShowString(2,120,colors[0],colors[7],"TYPE:");
	
     if(card_type)
     {  LCD_ShowString(52+30,120,colors[2],colors[7],"SD");
        ChineseChar(84+30,120,1,colors[2],colors[7],5);	//卡
     }       
      else
     { LCD_ShowString(52,120,colors[2],colors[7],"MMC");
       ChineseChar(68+30,120,1,colors[2],colors[7],5);	//卡
     }
   	  LCD_CS(1);
      Read_CID_MMC(cid_data);
	  LCD_CS(0);

     LCD_ShowString(2,150,colors[0],colors[7],"NAME:");
     LCD_ShowChar(52+30,150,colors[2],colors[7],cid_data[3]);
     LCD_ShowChar(68+30,150,colors[2],colors[7],cid_data[4]);
     LCD_ShowChar(84+30,150,colors[2],colors[7],cid_data[5]);
     LCD_ShowChar(100+30,150,colors[2],colors[7],cid_data[6]);
     LCD_ShowChar(116+30,150,colors[2],colors[7],cid_data[7]);	  

     
     Read_CSD_MMC(csd_data);
	 
	 //C_SIZE为CSD[73:62]
     c_size=csd_data[6] & 0x03; //bits 1..0
     c_size<<=10;
     c_size+=(unsigned int)csd_data[7]<<2;
     c_size+=csd_data[8]>>6;
	 
	 //READ_BL_LEN为CSD[83:80]
     by= csd_data[5] & 0x0F;
     read_bl_len=1;
     read_bl_len<<=by;	  //2^by

	 //C_SIZE_MULT为CSD[49:47]
     by=csd_data[9] & 0x03;      //获取multiply
     by<<=1;
     by+=csd_data[10] >> 7;

     c_size_mult=1;
     c_size_mult<<=(2+by);	  //2^(c_size_mult+2)
	 //存储器容量＝BLOCKNR*BLOCK_LEN  
	 //BLOCKNR = (C_SIZE+1) * MULT  MULT = (C_SIZE_MULT) * 2^(C_SIZE_MULT + 2)  BLOCK_LEN = (READ_BL_LEN < 12) * 2^(READ_BL_LEN)

     drive_size=(ulong)(c_size+1) * (ulong)c_size_mult * (ulong)read_bl_len;

     size_mb= drive_size>>20;    //1024*1024=1048576=2^20; 
     LCD_ShowString(2,180,colors[0],colors[7],"SIZE:");
     LCD_ShowChar(52+30,180,colors[2],colors[7],size_mb/1000|0x30); 
     LCD_ShowChar(68+30,180,colors[2],colors[7],size_mb%1000/100|0x30);
     LCD_ShowChar(84+30,180,colors[2],colors[7],size_mb%100/10|0x30); 
     LCD_ShowChar(100+30,180,colors[2],colors[7],size_mb%10|0x30); 
     LCD_ShowChar(116+30,180,colors[2],colors[7],'M');
     LCD_ShowChar(132+30,180,colors[2],colors[7],'B');
	 LCD_CS(1);
   }
   else
   { LCD_CS(0);
     LCD_ShowString(20,90,colors[0],colors[7],"INIT FAILURE");	 //初始化失败
     while(1)
     { LCD_CS(0);
       LCD_ShowString(14,120,colors[2],colors[7],"CARD CHECK-UP"); //卡检查提示闪动显示
       delayms(1000);
       LCD_ShowString(14,120,colors[2],colors[7],"             ");	 
       delayms(1000);
	   LCD_CS(1);
     } 
   }  
      
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
