 /*********************************************************************
*                                                                     *
*          STM32- 3.2寸TFT液晶屏字符和汉字显示程序                               *
*              （  普中科技  ）                                       *
        
***********************************************************************/

#include "stm32f10x_lib.h"
#include "LCD_driver.h"
#include "fat.h"
#include "MMC.h"



struct FAT32_Init_Arg Init_Arg_SD;	   //初始化参数结构体实体
struct FAT32_Init_Arg *pArg;
struct FileInfoStruct  FileInfo;        //文件信息结构体实体
uchar  file_buf[150];	   //文件数据缓冲区，读取到的文件数据放在其中

uchar Dev_No=0; //设备号，SDCARD的SD卡设备号的宏定义，根据设备号，znFAT会将底层存储设备驱动连接到相应的存储设备驱动
                        //动态的切换设备号，就可以实现在各种存储设备之间进行切换，这也是znFAT实现多设备的重要手段。比如，我
						//可以将SD卡上的文件拷贝到CF卡或U盘上，这就是多设备的最典型应用

#define COLOR(r,g,b) (((((((uint)r)>>3)<<6)|(((uint)g)>>2))<<5)|(((uint)b)>>3))	 //r红色分量 0~31  g绿色分量 0~63  b蓝色分量 0~31	


#include "LCD.c"

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
  uchar item;


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
   
   //SD卡初始化 
   MMC_Reset();
   MMC_Init();	  
   if(Init_Flag == 0) LCD_ShowString(20,5,colors[0],colors[7],"SD SUCCESS");
   else               LCD_ShowString(20,5,colors[0],colors[7],"SD FAILURE");
 


   pArg=&Init_Arg_SD;  //pArg是znFAT的“初始参数结构指针”，它所指向的结构体中记录了存储器及其文件系统的重要参数信息
                       //在切换存储设备时，不光要更改设备号，还要将pArg指向相应存储设备的“初始参数结构”
   Dev_No=SDCARD; 	   //设备号为SDCARD，znFAT中将其宏定义为 0
   FAT32_Init(); 	   //文件系统初始化，使用znFAT前必须进行初始化，初始化过程中将所使用的存储设备的文件系统的一些重要参数
               		   //记录到了pArg所指向的结构中，以备后面使用
   item=0;
   while(1)
   { UINT8 rRe;
				
	  rRe=FAT32_Open_File(&FileInfo,"\\*.bmp",item);
	  if(rRe==0) //打开成功，则显示图像
	  {
	    display_pic();
	    item++;
	  }
	  else item=0;			
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
