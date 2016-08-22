 /*********************************************************************
*                                                                     *
*          STM32- 3.2��TFTҺ�����ַ��ͺ�����ʾ����                               *
*              ��  ���пƼ�  ��                                       *
        
***********************************************************************/

#include "stm32f10x_lib.h"
#include "LCD_driver.h"
#include "MMC.h"


/******************************** �������� ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;


unsigned char  csd_data[16];
unsigned char  cid_data[16];

unsigned char by;
unsigned int c_size, c_size_mult, read_bl_len;
unsigned long drive_size;
unsigned long   size_mb;

/*********************************�������� -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Delay(vu32 nCount);


/*******************************************************************************
                                  ������
*******************************************************************************/
int main(void)
{

#ifdef DEBUG
  debug();
#endif

   RCC_Configuration();      //ϵͳʱ�����ú���   

   NVIC_Configuration();     //NVIC���ú���

    //����GPIOģ��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    //�ѵ���������ͨIO��
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);  
                            
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;	 //����GPIOΪͬһ���Ͷ˿�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //��������Ƶ��Ϊ50HZ
  GPIO_Init(GPIOA, &GPIO_InitStructure);   //��ʼ��GPIOA�˿�
  GPIO_Init(GPIOB, &GPIO_InitStructure);   //��ʼ��GPIOB�˿�

   GPIO_Write(GPIOA,0xffff);  //��GPIOA 16���˿�ȫ����Ϊ�ߵ�ƽ
   GPIO_Write(GPIOB,0xffff);  //��GPIOB 16���˿�ȫ����Ϊ�ߵ�ƽ

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	 //����GPIOΪͬһ���Ͷ˿�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //��������Ƶ��Ϊ50HZ
  GPIO_Init(GPIOA, &GPIO_InitStructure);   //��ʼ��GPIOA�˿�

   LCD_Init();
   LCD_clear(2);  // �������ʾ������ɫ��
   T_CS(1);	
    
    LCD_CS(0);
    LCD_ShowString(70,5,colors[4],colors[7],"SD/MMC");
    ChineseChar(32+30,50,1,colors[2],colors[7],1);	//��
    ChineseChar(64+30,50,1,colors[2],colors[7],2);	//��
    ChineseChar(96+30,50,1,colors[2],colors[7],3);	//��
    ChineseChar(128+30,50,1,colors[2],colors[7],4); //��
	LCD_CS(1);

	MMC_Init(); //��ʼ��SD��
    if(Init_Flag == 0)
   { LCD_CS(0);

  	 LCD_ShowString(20,90,colors[0],colors[7],"INIT SUCCESS");
	 LCD_ShowString(2,120,colors[0],colors[7],"TYPE:");
	
     if(card_type)
     {  LCD_ShowString(52+30,120,colors[2],colors[7],"SD");
        ChineseChar(84+30,120,1,colors[2],colors[7],5);	//��
     }       
      else
     { LCD_ShowString(52,120,colors[2],colors[7],"MMC");
       ChineseChar(68+30,120,1,colors[2],colors[7],5);	//��
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
	 
	 //C_SIZEΪCSD[73:62]
     c_size=csd_data[6] & 0x03; //bits 1..0
     c_size<<=10;
     c_size+=(unsigned int)csd_data[7]<<2;
     c_size+=csd_data[8]>>6;
	 
	 //READ_BL_LENΪCSD[83:80]
     by= csd_data[5] & 0x0F;
     read_bl_len=1;
     read_bl_len<<=by;	  //2^by

	 //C_SIZE_MULTΪCSD[49:47]
     by=csd_data[9] & 0x03;      //��ȡmultiply
     by<<=1;
     by+=csd_data[10] >> 7;

     c_size_mult=1;
     c_size_mult<<=(2+by);	  //2^(c_size_mult+2)
	 //�洢��������BLOCKNR*BLOCK_LEN  
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
     LCD_ShowString(20,90,colors[0],colors[7],"INIT FAILURE");	 //��ʼ��ʧ��
     while(1)
     { LCD_CS(0);
       LCD_ShowString(14,120,colors[2],colors[7],"CARD CHECK-UP"); //�������ʾ������ʾ
       delayms(1000);
       LCD_ShowString(14,120,colors[2],colors[7],"             ");	 
       delayms(1000);
	   LCD_CS(1);
     } 
   }  
      
   while(1);
    
}


/*******************************************************************************
*                           ����RCC
*******************************************************************************/
void RCC_Configuration(void)
{   
 //��λRCC�ⲿ�豸�Ĵ�����Ĭ��ֵ
  RCC_DeInit();

  //���ⲿ���پ���
  RCC_HSEConfig(RCC_HSE_ON);

   //�ȴ��ⲿ����ʱ��׼����
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)   //�ⲿ����ʱ���Ѿ�׼���
  {								    
    //����FLASH��Ԥȡ����
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    //FLASH�ӳ�2������
    FLASH_SetLatency(FLASH_Latency_2);
 	
  //����AHB(HCLK)ʱ��=SYSCLK
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  
  
   //����APB2(PCLK2)��=AHBʱ��
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    //����APB1(PCLK1)��=AHB 1/2ʱ��
    RCC_PCLK1Config(RCC_HCLK_Div2);

     //����PLLʱ�� == �ⲿ���پ���ʱ��*9  PLLCLK = 8MHz * 9 = 72 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    //ʹ��PLLʱ��
    RCC_PLLCmd(ENABLE);

   //�ȴ�PLLʱ�Ӿ���
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

  //����ϵͳʱ�� = PLLʱ��
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

   //���PLLʱ���Ƿ���Ϊϵͳʱ��
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}

/*******************************************************************************
*                             NVIC���ú���
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
*                              ��ʱ����
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
