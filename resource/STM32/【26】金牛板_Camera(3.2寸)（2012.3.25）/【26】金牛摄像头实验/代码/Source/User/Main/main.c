/**************************************************************
** 	��ţ������
**	�ۺϹ��ܲ��ԡ���������ͷʵ��
**  �汾��V1.0  
**  ��̳��bbs.openmuc.com
**	������www.taobao.com   
**  ���䣺openmcu@openmcu.com 
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
/* II2C ���� */
#define EEPROM_WriteAddress    0
#define EEPROM_ReadAddress     0
#define TEXT_Size             (Get_Size(TEXT_Buffer)-1)

/* LED ���� */
#define LED1 PDout(8)	   //�����϶�Ӧ��LD1
#define LED2 PDout(9)	   //�����϶�Ӧ��LD2
#define LED3 PDout(10)	   //�����϶�Ӧ��LD3
#define LED4 PDout(11)	   //�����϶�Ӧ��LD4

/* ���� ���� */
#define KEY2   PCin(13)	   //�����ϵ�S2��
#define KEY3   PAin(0)	   //�����ϵ�S3��
#define KEY4   PAin(8)	   //�����ϵ�S4��

/* Private macro -------------------------------------------------------------*/
#define Get_Size(a) (sizeof(a) / sizeof(*(a)))
/* Private variables ---------------------------------------------------------*/
/* II2C ���� */
uint8_t TEXT_Buffer[]={"Fire-Bull STM32 I2C TEST"};
uint8_t REXT_Buffer[TEXT_Size];

/* ������ ���� */
u8 i=0;
u32 m=0;   //���б�������
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
** ��������: main
** ��������: ���������
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
int main(void)
{
  	SystemInit();				// ϵͳʱ�����ã�clock, PLL and Flash configuration��
	while(SysTick_Config(SystemFrequency / 1000));	//Systick ������ʱn*ms
	NVIC_Configuration();		// ϵͳ�ж�����
  	LCD_Init();					// LCD��ʼ��
	LCD_Clear(BLACK);			// ����
	Camera_Configuration();		//����ͷ����
	BACK_COLOR = BLACK;
	/* LCD ���� */		
	LCD_WriteBMP(100,110,99,97,gImage_11);	  		//97�ֱ�99ΪͼƬ�ĸ߶ȺͿ��
	WriteString(0,170,"ILI932X TFT����ͷ����ʵ��\r\n\r           ��������STM32F103VC",BLUE); 
	WriteString(0,220,"OV7670 Init......",RED); 
  	Delay(100);

  	FIFO_WRST =0;	//дָ�븴λ
  	DelayNS(200);
  	FIFO_WRST =1;

  	FIFO_RRST =0;	//��ָ�븴λ
	FIFO_RCLK =0;	//������Ҫһ��ʱ�����ڵ�������ܸ�λ
	FIFO_RCLK =1;
	FIFO_RCLK =0;	//������Ҫһ��ʱ�����ڵ�������ܸ�λ
	FIFO_RCLK =1;
  	DelayNS(200);
  	FIFO_RRST =1;

  	while (1)
  	{
     	if(OV7660_init())	 //OV7660_init()==1��ʼ���ɹ�����whileѭ��
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
			for(m = 0; m <76800; m++)	//QVGA��ʽ,240��;ÿ��320����		
			{

				FIFO_RCLK =0;
				__nop();
				__nop();
				FIFO_RCLK =1;
				__nop();
				__nop();  
				__nop();
				__nop();
				Camera_Data = FIFO_DATA_PIN;	//�ȶ���8λ 
				Camera_Data <<= 8;            
		
				FIFO_RCLK =0;
				__nop();
				__nop();
				FIFO_RCLK =1;
				__nop();
				__nop();
				temp1 = FIFO_DATA_PIN;  //����λ
				Camera_Data |= temp1;
				GPIOE->ODR = Camera_Data;
				LCD_WR = 0;
				LCD_WR = 1;			 
			}
			LCD_CS = 1;
			FIFO_RRST =0;				//��ָ�븴λ										
			for(i=0;i<128;i++)		
			{
				FIFO_RCLK =0;				//������Ҫһ��ʱ�����ڵ�������ܸ�λ
				FIFO_RCLK =1;
			}
			FIFO_RRST =1;
		
//			FIFO_WEN =0;					//�����ֲ���˵��,��λʱWENҪ�ø�,�� 
			FIFO_WRST =0;				//MCU��WEN��HREF��������߼���������							
		 	__nop();					//��AL422B��,����MCU��WEN�õ��������422��WENһ��Ϊ��
			__nop();					//дָ�븴λ��Ҫһ������ʱ	
			DelayNS(2);				//�����ʱ������ͼ���׷ֿ�
			FIFO_WRST =1;	   
			VsyncCnt = 0;				//��ʼ��һ֡���ݲɼ�	   
		}
	}
}	
/*****************************************************************************
** ��������: Refreshes_Screen
** ��������: ˢ����Ļ				
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void Refreshes_Screen(void)
{
	LCD_Clear(WHITE);//����  
	WriteString(216,0,"RST",RED);
  	POINT_COLOR=BLUE;//���û�����ɫ 
}
/*****************************************************************************
** ��������: GPIO_Configuration
** ��������: ����IO��(LED��USART)
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
						   	| RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,ENABLE);	/* ʹ�ܸ����˿�ʱ�ӣ���Ҫ������*/ 
	
	/* Configure IO connected to LD1, LD2, LD3 and LD4 leds *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);	   

	/* Configure IO connected to S3 *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;			//��������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		   

	/* Configure IO connected to S2 *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			//��������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						 

	/* Configure IO connected to S3.S2 *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			//��������
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

	//PC5 ��Ϊģ��ͨ��15�������� ,ADCת��                        
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);			   
}
/******************************************************************************
** ��������: NVIC_Configuration
** ��������: ϵͳ�ж�����
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
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
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	   // �����ж��� Ϊ2 
	  
	/* Enable the EXTI4_IRQn Interrupt */
/*	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;		// �����ж���4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ����ռ�����ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // ���ø����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // ʹ���ж���4
	NVIC_Init(&NVIC_InitStructure);							  // ��ʼ���ж���4	*/
	
	/*  ����ͷ�ж����� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//�������ȼ���:��ռ���ȼ�1λ,�����ȼ�3λ
   	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
   	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;
   	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   	NVIC_Init(&NVIC_InitStructure);
}
/*****************************************************************************
** ��������: Delay
** ��������: ��Systick��ʱ
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}
/*****************************************************************************
** ��������: Delay
** ��������: Systick�жϽ��뺯��
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}
/*****************************************************************************
** ��������: KEY_Scan
** ��������: ����������
				0��û���κΰ�������	  1��KEY2���� 
				 2��KEY3����	3��KEY4���� 
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
uint8_t KEY_Scan(void)
{	 
	static uint8_t KEY_UP=1;	//�������ɿ���־	   
	if(KEY_UP&&(KEY2==0||KEY4==0))
	{
		Delay(10);//ȥ���� 
		KEY_UP=0;
		if(KEY2==0)return 1;
		//else if(KEY3==1)return 2;
		else if(KEY4==0)return 2;
	}else if(KEY2==1&&KEY4==1)KEY_UP=1; 	    
	return 0;		// �ް�������
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
