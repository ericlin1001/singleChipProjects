 /*********************************************************************
*                                                                     *
*          STM32- 3.2��TFTҺ�����ַ��ͺ�����ʾ����                               *
*              ��  ���пƼ�  ��                                       *
        
***********************************************************************/

#include "stm32f10x_lib.h"
#include "LCD_driver.h"
#include "fat.h"
#include "MMC.h"



struct FAT32_Init_Arg Init_Arg_SD;	   //��ʼ�������ṹ��ʵ��
struct FAT32_Init_Arg *pArg;
struct FileInfoStruct  FileInfo;        //�ļ���Ϣ�ṹ��ʵ��
uchar  file_buf[150];	   //�ļ����ݻ���������ȡ�����ļ����ݷ�������

uchar Dev_No=0; //�豸�ţ�SDCARD��SD���豸�ŵĺ궨�壬�����豸�ţ�znFAT�Ὣ�ײ�洢�豸�������ӵ���Ӧ�Ĵ洢�豸����
                        //��̬���л��豸�ţ��Ϳ���ʵ���ڸ��ִ洢�豸֮������л�����Ҳ��znFATʵ�ֶ��豸����Ҫ�ֶΡ����磬��
						//���Խ�SD���ϵ��ļ�������CF����U���ϣ�����Ƕ��豸�������Ӧ��

#define COLOR(r,g,b) (((((((uint)r)>>3)<<6)|(((uint)g)>>2))<<5)|(((uint)b)>>3))	 //r��ɫ���� 0~31  g��ɫ���� 0~63  b��ɫ���� 0~31	


#include "LCD.c"

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
  uchar item;


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
   
   //SD����ʼ�� 
   MMC_Reset();
   MMC_Init();	  
   if(Init_Flag == 0) LCD_ShowString(20,5,colors[0],colors[7],"SD SUCCESS");
   else               LCD_ShowString(20,5,colors[0],colors[7],"SD FAILURE");
 


   pArg=&Init_Arg_SD;  //pArg��znFAT�ġ���ʼ�����ṹָ�롱������ָ��Ľṹ���м�¼�˴洢�������ļ�ϵͳ����Ҫ������Ϣ
                       //���л��洢�豸ʱ������Ҫ�����豸�ţ���Ҫ��pArgָ����Ӧ�洢�豸�ġ���ʼ�����ṹ��
   Dev_No=SDCARD; 	   //�豸��ΪSDCARD��znFAT�н���궨��Ϊ 0
   FAT32_Init(); 	   //�ļ�ϵͳ��ʼ����ʹ��znFATǰ������г�ʼ������ʼ�������н���ʹ�õĴ洢�豸���ļ�ϵͳ��һЩ��Ҫ����
               		   //��¼����pArg��ָ��Ľṹ�У��Ա�����ʹ��
   item=0;
   while(1)
   { UINT8 rRe;
				
	  rRe=FAT32_Open_File(&FileInfo,"\\*.bmp",item);
	  if(rRe==0) //�򿪳ɹ�������ʾͼ��
	  {
	    display_pic();
	    item++;
	  }
	  else item=0;			
   }
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
