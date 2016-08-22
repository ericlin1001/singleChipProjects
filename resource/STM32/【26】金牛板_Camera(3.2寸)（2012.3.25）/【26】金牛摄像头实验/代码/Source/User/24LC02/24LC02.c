/**************************************************************
** 	��ţ������
**	EEPROM(24LC02)�������
**  �汾��V1.0  
**	��̳��www.openmcu.com
**	�Ա���http://shop36995246.taobao.com/   
**  ����֧��Ⱥ��121939788 
***************************************************************/

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "24LC02.h" 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_Speed              400000	   /* �ٶȲ�Ҫ����400KHz */
#define I2C1_SLAVE_ADDRESS7    0xA0		   /* 24LC02�ĵ�ַ */
//#define I2C_PageSize           8		   /* �ܹ�8ҳ��ÿҳ256�ֽ� */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t EEPROM_ADDRESS;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*****************************************************************************
** ��������: II2C_Configuration
** ��������: II2C����
					�ٶȲ��ܴ���400KHz
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void II2C_Configuration(void)
{
  	I2C_InitTypeDef  I2C_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ��IO���ù��ܣ�ʹ���жϹ�����Ҫ������

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);	/* ʹ�ܸ����˿�ʱ�ӣ���Ҫ������*/ 
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
** ��������: II2C_Init
** ��������: II2C��ʼ��		
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void II2C_Init()
{
	I2C_DeInit(I2C1);
  	/* I2C configuration */
  	II2C_Configuration();
}
/*****************************************************************************
** ��������: delay
** ��������: �����ȶ��ԵĿ��ǣ��������ĵײ�ӿ�ֻ�ܵ��ζ���дһ���ֽ�
				�����Թ����ȶ�����β���δ������
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
static void delay(uint32_t cnt)
{
	while(cnt--);
}
/*****************************************************************************
** ��������: I2C_AcknowledgePolling
** ��������: �ȴ���ȡI2C���߿���Ȩ
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
static void I2C_AcknowledgePolling()
{ 
    do{ 
		I2C_GenerateSTART(I2C1,ENABLE);
        I2C_ReadRegister(I2C1,I2C_Register_SR1);//���ADDRλ
		I2C_ReadRegister(I2C1,I2C_Register_SR2);//
        I2C_Send7bitAddress(I2C1,ADDR_24LC02,I2C_Direction_Transmitter);
		delay(10);								//���͵�ַ�󣬲��������յ�ACK������ʱ�ȴ���
		
    }while(!(I2C_ReadRegister(I2C1,I2C_Register_SR1)&0x0002)); 
 
    I2C_ClearFlag(I2C1,I2C_FLAG_AF);
} 
/*****************************************************************************
** ��������: I2C_ReadOneByte
** ��������: ͨ��ָ��I2C�ӿڶ�ȡһ���ֽ�����
				   addr Ԥ��ȡ�ֽڴ洢λ��
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
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
** ��������: I2C_Read
** ��������: ͨ��ָ��I2C�ӿڶ�ȡ����ֽ�����
				addr Ԥ��ȡ�ֽڴ洢λ��
					*buf ��ȡ���ݵĴ洢λ�ã�num��ȡ�ֽ��� �ɹ����� 0
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
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
** ��������: I2C_WriteOneByte
** ��������: ͨ��ָ��I2C�ӿ�д��һ���ֽ�����
				addr Ԥд���ֽڵ�ַ��value д������
					�ɹ�����0
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
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
** ��������: I2C_WriteOneByte
** ��������: ͨ��ָ��I2C�ӿ�д�����ֽ�����
				��������ַ,addr Ԥд���ֽڵ�ַ
					*buf Ԥд�����ݴ洢λ�ã�numд���ֽ���	 
						�ɹ�����0
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
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
