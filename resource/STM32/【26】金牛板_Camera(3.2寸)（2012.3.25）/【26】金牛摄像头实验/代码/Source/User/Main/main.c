/**************************************************************
** 	金牛开发板
**	综合功能测试———摄像头实验
**  版本：V1.0  
**  论坛：bbs.openmuc.com
**	旺宝：www.taobao.com   
**  邮箱：openmcu@openmcu.com 
***************************************************************/ 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "hardware_conf.h"
#include "Variable.h"
#include "USART.h"
#include "ili932x_conf.h"
#include "ov7670.h"	
#include "Include.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* II2C 定义 */
#define EEPROM_WriteAddress    0
#define EEPROM_ReadAddress     0
#define TEXT_Size             (Get_Size(TEXT_Buffer)-1)

/* LED 定义 */
#define LED1 PDout(8)	   //板子上对应于LD1
#define LED2 PDout(9)	   //板子上对应于LD2
#define LED3 PDout(10)	   //板子上对应于LD3
#define LED4 PDout(11)	   //板子上对应于LD4

/* 按键 定义 */
#define KEY2   PCin(13)	   //板子上的S2键
#define KEY3   PAin(0)	   //板子上的S3键
#define KEY4   PAin(8)	   //板子上的S4键

/* Private macro -------------------------------------------------------------*/
#define Get_Size(a) (sizeof(a) / sizeof(*(a)))
/* Private variables ---------------------------------------------------------*/
/* II2C 定义 */
uint8_t TEXT_Buffer[]={"Fire-Bull STM32 I2C TEST"};
uint8_t REXT_Buffer[TEXT_Size];

/* 主函数 定义 */
u8 i=0;
u32 m=0;   //行列变量定义
u16 Camera_Data = 0,temp1 = 0;
  
extern u8 VsyncCnt;

static __IO uint32_t TimingDelay;
extern uint8_t gImage_11[];
	 
/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void RCC_Configuration(void);
uint8_t KEY_Scan(void);
void Delay(__IO uint32_t nTime);

void Refreshes_Screen(void);

void HexToASCII(uint16_t data);
/* Private functions ---------------------------------------------------------*/

/*****************************************************************************
** 函数名称: main
** 功能描述: 主函数入口
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
int main(void)
{
  	SystemInit();				// 系统时钟配置（clock, PLL and Flash configuration）
	while(SysTick_Config(SystemFrequency / 1000));	//Systick 配置延时n*ms
	NVIC_Configuration();		// 系统中断配置
  	LCD_Init();					// LCD初始化
	LCD_Clear(BLACK);			// 清屏
	Camera_Configuration();		//摄像头配置
	BACK_COLOR = BLACK;
	/* LCD 测试 */		
	LCD_WriteBMP(100,110,99,97,gImage_11);	  		//97分别99为图片的高度和宽度
	WriteString(0,170,"ILI932X TFT摄像头驱动实验\r\n\r           ——基于STM32F103VC",BLUE); 
	WriteString(0,220,"OV7670 Init......",RED); 
  	Delay(100);

  	FIFO_WRST =0;	//写指针复位
  	DelayNS(200);
  	FIFO_WRST =1;

  	FIFO_RRST =0;	//读指针复位
	FIFO_RCLK =0;	//至少需要一个时钟周期的跳变才能复位
	FIFO_RCLK =1;
	FIFO_RCLK =0;	//至少需要一个时钟周期的跳变才能复位
	FIFO_RCLK =1;
  	DelayNS(200);
  	FIFO_RRST =1;

  	while (1)
  	{
     	if(OV7660_init())	 //OV7660_init()==1初始化成功跳出while循环
	 	break;
  	}
	WriteString(0,220,"OV7670 Init......OK",RED); 
  	Delay(800);
	set_qvga();

  	while (1)
  	{
		if(VsyncCnt == 2)
  	   	{
		  	LCD_SetDisplayWindow(0, 0, 239, 319);
  			LCD_WriteRAM_Prepare();
			LCD_CS = 0;
			LCD_RS = 1;
			for(m = 0; m <76800; m++)	//QVGA格式,240行;每行320个点		
			{

				FIFO_RCLK =0;
				__nop();
				__nop();
				FIFO_RCLK =1;
				__nop();
				__nop();  
				__nop();
				__nop();
				Camera_Data = FIFO_DATA_PIN;	//先读高8位 
				Camera_Data <<= 8;            
		
				FIFO_RCLK =0;
				__nop();
				__nop();
				FIFO_RCLK =1;
				__nop();
				__nop();
				temp1 = FIFO_DATA_PIN;  //读低位
				Camera_Data |= temp1;
				GPIOE->ODR = Camera_Data;
				LCD_WR = 0;
				LCD_WR = 1;			 
			}
			LCD_CS = 1;
			FIFO_RRST =0;				//读指针复位										
			for(i=0;i<128;i++)		
			{
				FIFO_RCLK =0;				//至少需要一个时钟周期的跳变才能复位
				FIFO_RCLK =1;
			}
			FIFO_RRST =1;
		
//			FIFO_WEN =0;					//按照手册上说明,复位时WEN要置高,但 
			FIFO_WRST =0;				//MCU的WEN和HREF是做与非逻辑后再输入							
		 	__nop();					//到AL422B的,所以MCU的WEN置低则输出到422的WEN一定为低
			__nop();					//写指针复位需要一定的延时	
			DelayNS(2);				//如果延时不够，图像易分块
			FIFO_WRST =1;	   
			VsyncCnt = 0;				//开始下一帧数据采集	   
		}
	}
}	
/*****************************************************************************
** 函数名称: Refreshes_Screen
** 功能描述: 刷新屏幕				
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void Refreshes_Screen(void)
{
	LCD_Clear(WHITE);//清屏  
	WriteString(216,0,"RST",RED);
  	POINT_COLOR=BLUE;//设置画笔蓝色 
}
/*****************************************************************************
** 函数名称: GPIO_Configuration
** 功能描述: 配置IO口(LED及USART)
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
						   	| RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,ENABLE);	/* 使能各个端口时钟，重要！！！*/ 
	
	/* Configure IO connected to LD1, LD2, LD3 and LD4 leds *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);	   

	/* Configure IO connected to S3 *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;			//下拉输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		   

	/* Configure IO connected to S2 *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			//上拉输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						 

	/* Configure IO connected to S3.S2 *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			//上拉输入
  	GPIO_Init(GPIOC, &GPIO_InitStructure);		

    /*Configure USART1 Tx (PA9) as alternate function push-pull*/ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);			
    
   	/*Configure USART1 Rx (PA10) as input floating*/ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);			

	//PC5 作为模拟通道15输入引脚 ,ADC转换                        
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);			   
}
/******************************************************************************
** 函数名称: NVIC_Configuration
** 功能描述: 系统中断配置
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void NVIC_Configuration(void)
{ 
  	NVIC_InitTypeDef NVIC_InitStructure;
	
	#ifdef  VECT_TAB_RAM  
	  /* Set the Vector Table base location at 0x20000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
	  /* Set the Vector Table base location at 0x08000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif

	/* Configure one bit for preemption priority */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	   // 设置中断组 为2 
	  
	/* Enable the EXTI4_IRQn Interrupt */
/*	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;		// 配置中断线4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 设置占先优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 设置副优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // 使能中断线4
	NVIC_Init(&NVIC_InitStructure);							  // 初始化中断线4	*/
	
	/*  摄像头中断配置 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//设置优先级组:先占优先级1位,从优先级3位
   	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
   	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;
   	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   	NVIC_Init(&NVIC_InitStructure);
}
/*****************************************************************************
** 函数名称: Delay
** 功能描述: 用Systick延时
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}
/*****************************************************************************
** 函数名称: Delay
** 功能描述: Systick中断进入函数
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}
/*****************************************************************************
** 函数名称: KEY_Scan
** 功能描述: 按键处理函数
				0，没有任何按键按下	  1，KEY2按下 
				 2，KEY3按下	3，KEY4按下 
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
uint8_t KEY_Scan(void)
{	 
	static uint8_t KEY_UP=1;	//按键按松开标志	   
	if(KEY_UP&&(KEY2==0||KEY4==0))
	{
		Delay(10);//去抖动 
		KEY_UP=0;
		if(KEY2==0)return 1;
		//else if(KEY3==1)return 2;
		else if(KEY4==0)return 2;
	}else if(KEY2==1&&KEY4==1)KEY_UP=1; 	    
	return 0;		// 无按键按下
}
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/*********************************************************************************************************
** End of File
*********************************************************************************************************/
