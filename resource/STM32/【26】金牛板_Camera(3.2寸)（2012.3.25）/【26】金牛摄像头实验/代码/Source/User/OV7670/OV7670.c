/**************************************************************
** 	��ţ������
**	LCD��������
**  �汾��V1.0  
**	��̳��www.openmcu.com
**	�Ա���http://shop36995246.taobao.com/   
**  ����֧��Ⱥ��121939788 
***************************************************************/
#include "stm32f10x.h"
#include "hardware_conf.h"
#include "ov7670.h"	
#include "sccb.h"

/*******************************************************************************
* �������� : void EXTI_Configuration(void)
* ��������: �ж�������
* �����������
* �����������
* ����ֵ����
*******************************************************************************/
void EXTI_Configuration(void)
{
  	EXTI_InitTypeDef EXTI_InitStructure;
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
  	/* EXTI Line15 �½��ز����ж� */
  	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
  	EXTI_GenerateSWInterrupt(GPIO_PinSource12);
}
/*****************************************************************************
** ��������: Touch_Configuration
** ��������: ������IO����
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void Camera_Configuration(void)
{
  	EXTI_Configuration();		//����ͷ֡Ƶ�ж���������
  	FIFO_GPIO_Configuration();  //AL422����ܽ�����
}
/*****************************************************
��������OV7670_GPIO_Init
���ܣ�OV7670GPIO��ʼ��
���룺��
�������
����ֵ����
*****************************************************/
void OV7670_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  OV7670_VSYNC_BIT; //VSYNC
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*****************************************************
��������FIFO_GPIO_Configuration
���ܣ�FIFO_GPIO��ʼ��
���룺��
�������
����ֵ����
*****************************************************/
void FIFO_GPIO_Configuration(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
                                | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2
                                |GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; //DATA
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*****************************************************
��������XCLK_init_ON
���ܣ��ṩ7660ʱ��
���룺��
�������
����ֵ����
*****************************************************/
void XCLK_init_ON(void)
{
  
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = OV7670_XCLK_BIT; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    RCC_MCOConfig(RCC_MCO_HSE);		//�����ⲿ������ΪOV7670ʱ��
}
/*****************************************************
��������XCLK_init_OFF
���ܣ��ر�7660ʱ��
���룺��
�������
����ֵ����
*****************************************************/
void XCLK_init_OFF(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = OV7670_XCLK_BIT; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//���ܣ�дOV7660�Ĵ���
//���أ�1-�ɹ�	0-ʧ��
u8 wrOV7670Reg(u8 regID, u8 regDat)
{
	startSCCB();
	if(SCCBwriteByte(0x42)==0)
	{
		stopSCCB();
		return(0);
	}
	DelaySCCB(100);	
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	DelaySCCB(100);
  	if(0==SCCBwriteByte(regDat))
	{
		stopSCCB();
		return(0);
	}
  	stopSCCB();
	
  	return(1);
}
////////////////////////////
//���ܣ���OV7660�Ĵ���
//���أ�1-�ɹ�	0-ʧ��
u8 rdOV7660Reg(u8 regID, u8 *regDat)
{
	//ͨ��д�������üĴ�����ַ
	startSCCB();
	if(0==SCCBwriteByte(0x42))
	{
		stopSCCB();
		return(0);
	}
	DelaySCCB(100);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	stopSCCB();
	
	DelaySCCB(100);
	
	//���üĴ�����ַ�󣬲��Ƕ�
	startSCCB();
	if(0==SCCBwriteByte(0x43))
	{
		stopSCCB();
		return(0);
	}
	DelaySCCB(100);
  	*regDat=SCCBreadByte();
  	noAck();
  	stopSCCB();
  	return(1);
}


//(140,16,640,480) is good for VGA
//(272,16,320,240) is good for QVGA
/* config_OV7660_window */
void OV7660_config_window(u16 startx,u16 starty,u16 width, u16 height)
{
	u16 endx=(startx+width);
	u16 endy=(starty+height*4);// "v*2"����
	u8 temp_reg1, temp_reg2;
	u8 state,temp;
	
	state = rdOV7660Reg(0x03, &temp_reg1 );
	temp_reg1 &= 0xC0;
	state = rdOV7660Reg(0x32, &temp_reg2 );
	temp_reg2 &= 0xC0;
	
	// Horizontal
	temp = temp_reg2|((endx&0x7)<<3)|(startx&0x7);
	state = wrOV7670Reg(0x32, temp );
	temp = (startx&0x7F8)>>3;
	state = wrOV7670Reg(0x17, temp );
	temp = (endx&0x7F8)>>3;
	state = wrOV7670Reg(0x18, temp );
	
	// Vertical
	temp = temp_reg1|((endy&0x7)<<3)|(starty&0x7);
	state = wrOV7670Reg(0x03, temp );
	temp = (starty&0x7F8)>>3;
	state = wrOV7670Reg(0x19, temp );
	temp = (endy&0x7F8)>>3;
	state = wrOV7670Reg(0x1A, temp );
	if(state==0){}
}

void my_delay_ms(u16 time)//delay some time
{
	u16 i;
	for(;time!=0;time--)
	{
		for(i=0;i<10000;i++);
	}
}
/*****************************************************
��������OV7660_init
���ܣ�OV7670��ʼ��
���룺��
�������
����ֵ������1�ɹ�������0ʧ��
*****************************************************/
u8 OV7660_init(void)
{
	u8  i , temp;       
	u8 ovidmsb=0;
	u8 ovidlsb=0;
	SCCB_GPIO_Config();//SCCB���߶˿�����
	OV7670_GPIO_Init();
	XCLK_init_ON();	   //��OV7670�ṩʱ��
	temp=0x80;		 //��λֵ
	if(0==wrOV7670Reg(0x12, temp)) //Reset SCCB
	{
	  return 0;
	}
	rdOV7660Reg(0x12, &ovidmsb);	   //��һ��Ҫ����,���ӵĻ�����ֵ������Ϊ0

	rdOV7660Reg(0x0a, &ovidmsb);	  //����Ʒ��λ���к�
	if(ovidmsb !=0x76)
	{
		return 0;
	} 
    //rdOV7660Reg(0x12, &ovidlsb);	  //��һ��Ҫ����,���ӵĻ�����ֵ������Ϊ0

	rdOV7660Reg(0x0b, &ovidlsb );		  //����Ʒ��λʶ���
	if(ovidlsb !=0x73)
	{
		return 0;
	} 
	for(i=0;i<CHANGE_REG_NUM;i++)
	{
		if( 0 == wrOV7670Reg(change_reg[i][0],change_reg[i][1]))
		{
			return 0;
		}
	}
	my_delay_ms(200);	
	
	return 0x01; //ok
}
void set_qvga(void)   
{   
    while(1!= wrOV7670Reg(0x12, 0x14));
   // wrOV7670Reg(0x40, 0xd0);     
    wrOV7670Reg(0x17, 0x16);   
    wrOV7670Reg(0x18, 0x04);   
    wrOV7670Reg(0x19, 0x02);   
    wrOV7670Reg(0x1A, 0x7B); 
    wrOV7670Reg(0x32, 0x40);
     
    wrOV7670Reg(0x03, 0x0a); 
  //  wrOV7670Reg(0x3E, 0x1A); 
    
    wrOV7670Reg(0x70, 0x3A); 
    wrOV7670Reg(0x71, 0x35);  
    wrOV7670Reg(0x72, 0x11);   
    wrOV7670Reg(0x73, 0xF9); 
    wrOV7670Reg(0xA2, 0x02);
    
}
/**************************************************************************************
* ��    ��: void Delay(u16 time) 
* ��    ��: ������ʱ����
* ��    ��: ��
* �� �� ֵ: ��
*
* �޸���ʷ:
*   �汾    ����     ����     �Ķ����ݺ�ԭ��
*   ----------------------------------------------------
*   1.0   2009.3.23 
**************************************************************************************/
void DelayNS(u16 time)                
{
    while(time--!=0); 
}
