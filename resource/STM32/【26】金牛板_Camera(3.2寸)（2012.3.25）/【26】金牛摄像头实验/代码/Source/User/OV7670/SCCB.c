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
函数名：SCCB_GPIO_Config
功能：SCCB总线端口配置
输入：无
输出：无
返回值：
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
   功能: start命令,SCCB的起始信号
   参数: 无
 返回值: 无
-----------------------------------------------
*/
void startSCCB(void)
{
    SCCB_SID =1;     //数据线高电平
    DelaySCCB(300);

    SCCB_SIC =1;	   //在时钟线高的时候数据线由高至低
    DelaySCCB(300);
 
    SCCB_SID =0;
    DelaySCCB(300);

    SCCB_SIC =0;	 //数据线恢复低电平，单操作函数必要
    DelaySCCB(300);


}
/*
-----------------------------------------------
   功能: stop命令,SCCB的停止信号
   参数: 无
 返回值: 无
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
   功能: noAck,用于连续读取中的最后一个结束周期
   参数: 无
 返回值: 无
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
   功能: 写入一个字节的数据到SCCB
   参数: 写入数据
 返回值: 发送成功返回1，发送失败返回0
-----------------------------------------------
*/
unsigned char SCCBwriteByte(unsigned char m_data)
{
	unsigned char j,tem;

	for(j=0;j<8;j++) //循环8次发送数据
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
	SCCB_SID_IN;/*设置SDA为输入*/
	DelaySCCB(500);
	SCCB_SIC =1;	
	DelaySCCB(100);
	if(SCCB_SID_STATE) {tem=0;}   //SDA=1发送失败，返回0}
	else {tem=1;}   //SDA=0发送成功，返回1
	SCCB_SIC =0;	
	DelaySCCB(500);	
    SCCB_SID_OUT;/*设置SDA为输出*/

	return (tem);  
}

/*
-----------------------------------------------
   功能: 一个字节数据读取并且返回
   参数: 无
 返回值: 读取到的数据
-----------------------------------------------
*/
unsigned char SCCBreadByte(void)
{
	unsigned char read,j;
	read=0x00;
	
	SCCB_SID_IN;	/*设置SDA为输入*/
	DelaySCCB(500);
	for(j=8;j>0;j--) //循环8次接收数据
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
	/*重新配置输出*/
	SCCB_SID_OUT;/*设置SDA为输出*/	
	return(read);
}
