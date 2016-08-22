#include "stm32f10x.h"
#include "hardware_conf.h"
#include "sccb.h"
// #include "define.h"

void DelaySCCB(unsigned int i)
 {     
    while( i--)
       {	
        
       }
 }
/*****************************************************
��������SCCB_GPIO_Config
���ܣ�SCCB���߶˿�����
���룺��
�������
����ֵ��
*****************************************************/
void SCCB_GPIO_Config(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
   	/* Enable GPIOE  clock */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 
}
void SCCB_SID_GPIO_OUTPUT(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
   	/* Enable GPIOE  clock */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void SCCB_SID_GPIO_INPUT(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
   	/* Enable GPIOE  clock */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*
-----------------------------------------------
   ����: start����,SCCB����ʼ�ź�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void startSCCB(void)
{
    SCCB_SID =1;     //�����߸ߵ�ƽ
    DelaySCCB(300);

    SCCB_SIC =1;	   //��ʱ���߸ߵ�ʱ���������ɸ�����
    DelaySCCB(300);
 
    SCCB_SID =0;
    DelaySCCB(300);

    SCCB_SIC =0;	 //�����߻ָ��͵�ƽ��������������Ҫ
    DelaySCCB(300);


}
/*
-----------------------------------------------
   ����: stop����,SCCB��ֹͣ�ź�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void stopSCCB(void)
{
    SCCB_SID =0;
    DelaySCCB(500);
 
    SCCB_SIC =1;	
    DelaySCCB(500);
  

    SCCB_SID =1;	
    DelaySCCB(500);   
}

/*
-----------------------------------------------
   ����: noAck,����������ȡ�е����һ����������
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void noAck(void)
{
	
	SCCB_SID =1;	
	DelaySCCB(500);
	
	SCCB_SIC =1;	
	DelaySCCB(500);
	
	SCCB_SIC =0;	
	DelaySCCB(500);
	
	SCCB_SID =0;	
	DelaySCCB(500);

}

/*
-----------------------------------------------
   ����: д��һ���ֽڵ����ݵ�SCCB
   ����: д������
 ����ֵ: ���ͳɹ�����1������ʧ�ܷ���0
-----------------------------------------------
*/
unsigned char SCCBwriteByte(unsigned char m_data)
{
	unsigned char j,tem;

	for(j=0;j<8;j++) //ѭ��8�η�������
	{
		if((m_data<<j)&0x80)
		{
			SCCB_SID =1;	
		}
		else
		{
			SCCB_SID =0;	
		}
		DelaySCCB(500);
		SCCB_SIC =1;	
		DelaySCCB(500);
		SCCB_SIC =0;	
		DelaySCCB(500);

	}
	DelaySCCB(100);
	SCCB_SID_IN;/*����SDAΪ����*/
	DelaySCCB(500);
	SCCB_SIC =1;	
	DelaySCCB(100);
	if(SCCB_SID_STATE) {tem=0;}   //SDA=1����ʧ�ܣ�����0}
	else {tem=1;}   //SDA=0���ͳɹ�������1
	SCCB_SIC =0;	
	DelaySCCB(500);	
    SCCB_SID_OUT;/*����SDAΪ���*/

	return (tem);  
}

/*
-----------------------------------------------
   ����: һ���ֽ����ݶ�ȡ���ҷ���
   ����: ��
 ����ֵ: ��ȡ��������
-----------------------------------------------
*/
unsigned char SCCBreadByte(void)
{
	unsigned char read,j;
	read=0x00;
	
	SCCB_SID_IN;	/*����SDAΪ����*/
	DelaySCCB(500);
	for(j=8;j>0;j--) //ѭ��8�ν�������
	{		     
		DelaySCCB(500);
		SCCB_SIC =1;
		DelaySCCB(500);
		read=read<<1;
		if(SCCB_SID_STATE) 
		{
			read=read+1;
		}
		SCCB_SIC =0;
		DelaySCCB(500);
	}
	/*�����������*/
	SCCB_SID_OUT;/*����SDAΪ���*/	
	return(read);
}
