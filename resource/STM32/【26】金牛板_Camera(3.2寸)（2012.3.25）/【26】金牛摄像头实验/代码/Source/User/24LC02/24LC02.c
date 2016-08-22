/**************************************************************
** 	火牛开发板
**	EEPROM(24LC02)程序代码
**  版本：V1.0  
**	论坛：www.openmcu.com
**	淘宝：http://shop36995246.taobao.com/   
**  技术支持群：121939788 
***************************************************************/

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "24LC02.h" 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_Speed              400000	   /* 速度不要超过400KHz */
#define I2C1_SLAVE_ADDRESS7    0xA0		   /* 24LC02的地址 */
//#define I2C_PageSize           8		   /* 总共8页，每页256字节 */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t EEPROM_ADDRESS;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*****************************************************************************
** 函数名称: II2C_Configuration
** 功能描述: II2C配置
					速度不能大于400KHz
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void II2C_Configuration(void)
{
  	I2C_InitTypeDef  I2C_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能IO复用功能，使用中断功能重要！！！

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);	/* 使能各个端口时钟，重要！！！*/ 
  	/* I2C1 Periph clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  	/* Configure I2C1 pins: SCL and SDA */
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

  	/* I2C configuration */
  	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  	I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;
  	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
  	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

  	/* Apply I2C configuration after enabling it */
  	I2C_Init(I2C1, &I2C_InitStructure);
  
  	/* I2C Peripheral Enable */
  	I2C_Cmd(I2C1, ENABLE);
}
/*****************************************************************************
** 函数名称: II2C_Init
** 功能描述: II2C初始化		
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void II2C_Init()
{
	I2C_DeInit(I2C1);
  	/* I2C configuration */
  	II2C_Configuration();
}
/*****************************************************************************
** 函数名称: delay
** 功能描述: 出于稳定性的考虑，本驱动的底层接口只能单次读、写一个字节
				经测试工作稳定，多次测试未见错误。
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
static void delay(uint32_t cnt)
{
	while(cnt--);
}
/*****************************************************************************
** 函数名称: I2C_AcknowledgePolling
** 功能描述: 等待获取I2C总线控制权
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
static void I2C_AcknowledgePolling()
{ 
    do{ 
		I2C_GenerateSTART(I2C1,ENABLE);
        I2C_ReadRegister(I2C1,I2C_Register_SR1);//清除ADDR位
		I2C_ReadRegister(I2C1,I2C_Register_SR2);//
        I2C_Send7bitAddress(I2C1,ADDR_24LC02,I2C_Direction_Transmitter);
		delay(10);								//发送地址后，不能马上收到ACK，需延时等待。
		
    }while(!(I2C_ReadRegister(I2C1,I2C_Register_SR1)&0x0002)); 
 
    I2C_ClearFlag(I2C1,I2C_FLAG_AF);
} 
/*****************************************************************************
** 函数名称: I2C_ReadOneByte
** 功能描述: 通过指定I2C接口读取一个字节数据
				   addr 预读取字节存储位置
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
uint8_t I2C_ReadOneByte(uint8_t addr)
{
	uint8_t res=0;
	I2C_AcknowledgePolling();
	
	I2C_GenerateSTART(I2C1, ENABLE);	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1,ADDR_24LC02,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	
	I2C_SendData(I2C1,addr);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(I2C1,ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1,ADDR_24LC02,I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));
	res=I2C1->DR;

	I2C_GenerateSTOP(I2C1,ENABLE);

	return res;
	
	
}
/*****************************************************************************
** 函数名称: I2C_Read
** 功能描述: 通过指定I2C接口读取多个字节数据
				addr 预读取字节存储位置
					*buf 读取数据的存储位置，num读取字节数 成功返回 0
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
uint8_t I2C_Read(uint8_t *buf,uint8_t addr,uint8_t num)
{
	while(num--)
	{		
		*buf++=I2C_ReadOneByte(addr++);
	}		
	
	return 0;
	
	
}
/*****************************************************************************
** 函数名称: I2C_WriteOneByte
** 功能描述: 通过指定I2C接口写入一个字节数据
				addr 预写入字节地址，value 写入数据
					成功返回0
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
static uint8_t I2C_WriteOneByte(uint8_t value,uint8_t addr)
{
	
	I2C_AcknowledgePolling();
	
	I2C_GenerateSTART(I2C1, ENABLE);	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1,ADDR_24LC02,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1,addr);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_SendData(I2C1,value);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));	
	
	I2C_GenerateSTOP(I2C1,ENABLE);
	return 0;
}
/*****************************************************************************
** 函数名称: I2C_WriteOneByte
** 功能描述: 通过指定I2C接口写入多个字节数据
				从器件地址,addr 预写入字节地址
					*buf 预写入数据存储位置，num写入字节数	 
						成功返回0
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
uint8_t I2C_Write(uint8_t *buf,uint8_t addr,uint8_t num)
{
	uint8_t err=0;
	
	while(num--)
	{
		if(I2C_WriteOneByte(*buf++,addr++))
		{
			err++;
		}
	}
	if(err)
		return 1;
	else 
		return 0;	
}
/*********************************************************************************************************
** End of File
*********************************************************************************************************/
