#include "stm32f10x.h"
#include "hardware_conf.h"
#include "M45PE16.h"
#include "MMC_SD.h"

#define  Dummy_Byte  0xff

void  M45PE_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE );

	//Configure PB12 pin: M45PE_CS pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOC,&GPIO_InitStructure); 

	M45PE_CS = 1;                            //�ر�����   
}
//��ȡM45P��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
uint8_t M45PE_ReadSR(void)   
{  
	uint8_t byte=0;   
	M45PE_CS=0;                            //ʹ������   
	SPI_ReadWriteByte(M45P_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
	M45PE_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
}   
//M45Pдʹ��	
//��WEL��λ   
void M45PE_Write_Enable(void)   
{
	M45PE_CS=0;                            //ʹ������   
    SPI_ReadWriteByte(M45P_WriteEnable);      //����дʹ��  
	M45PE_CS=1;                            //ȡ��Ƭѡ     	      
} 
//M45Pд��ֹ	
//��WEL����  
void M45PE_Write_Disable(void)   
{  
	M45PE_CS=0;                            //ʹ������   
    SPI_ReadWriteByte(M45P_WriteDisable);     //����д��ָֹ��    
	M45PE_CS=1;                            //ȡ��Ƭѡ     	      
} 			    
//��ȡоƬID M45P16��ID:0X204015
uint32_t M45PE_ReadID(void)
{
  	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  	/* Select the FLASH: Chip Select low */
	M45PE_CS=0;				    

  	/* Send "RDID " instruction */
  	SPI_ReadWriteByte(M45P_JedecDeviceID);

  	/* Read a byte from the FLASH */
  	Temp0 = SPI_ReadWriteByte(Dummy_Byte);

  	/* Read a byte from the FLASH */
  	Temp1 = SPI_ReadWriteByte(Dummy_Byte);

  	/* Read a byte from the FLASH */
  	Temp2 = SPI_ReadWriteByte(Dummy_Byte);
  
  	/* Deselect the FLASH: Chip Select high */
  	M45PE_CS=1;				    

  	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  	return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void M45PE_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;    												    
	M45PE_CS=0;                            //ʹ������   
    SPI_ReadWriteByte(M45P_ReadData);         //���Ͷ�ȡ����   
    SPI_ReadWriteByte((uint8_t)((ReadAddr)>>16));  //����24bit��ַ    
    SPI_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    SPI_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI_ReadWriteByte(0XFF);   //ѭ������  
    }
	M45PE_CS=1;                            //ȡ��Ƭѡ     	      
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void M45PE_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    M45PE_Write_Enable();                  //SET WEL 
	M45PE_CS=0;                            //ʹ������   
    SPI_ReadWriteByte(M45P_PageProgram);      //����дҳ����   
    SPI_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //����24bit��ַ    
    SPI_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    SPI_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	M45PE_CS=1;                            //ȡ��Ƭѡ 
	M45PE_Wait_Busy();					   //�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void M45PE_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		M45PE_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)  		   
uint8_t M45PE_BUF[4096];
void M45PE_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    

	secpos=WriteAddr/4096;//������ַ 0~511 for M45P16
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		M45PE_Read(M45PE_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(M45PE_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			M45PE_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				M45PE_BUF[i+secoff]=pBuffer[i];	  
			}
			M45PE_Write_NoCheck(M45PE_BUF,secpos*4096,4096);//д����������  

		}else M45PE_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	}	 	 
}
/*******************************************************************************
* Function Name  : M45PE_PageErase
* Description    : Erases the specified FLASH Page.
* Input          : PageAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void M45PE_PageErase(uint32_t PageAddr)
{
  /* Enable the write access to the FLASH */
  M45PE_Write_Enable();

  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  M45PE_CS=0;                       //ʹ������   
  /* Send Sector Erase instruction */
  SPI_ReadWriteByte(M45P_PageErase);
  /* Send SectorAddr high nibble address byte */
  SPI_ReadWriteByte((PageAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPI_ReadWriteByte((PageAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPI_ReadWriteByte(PageAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  M45PE_CS=1;                            //ʹ������   

  /* Wait the end of Flash writing */
  M45PE_Wait_Busy();
}
//����һ������
//Dst_Addr:������ַ 0~511 for M45P16
//����һ��ɽ��������ʱ��:150ms
void M45PE_Erase_Sector(uint32_t Dst_Addr)   
{   
	Dst_Addr*=4096;
    M45PE_Write_Enable();                  //SET WEL 	 
  	M45PE_CS=0;                            //ʹ������   
    SPI_ReadWriteByte(M45P_SectorErase);      //������������ָ�� 
    SPI_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //����24bit��ַ    
    SPI_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    SPI_ReadWriteByte((uint8_t)Dst_Addr);  
	M45PE_CS=1;                            //ȡ��Ƭѡ     	      
    M45PE_Wait_Busy();   				   //�ȴ��������
}  
//�ȴ�����
void M45PE_Wait_Busy(void)   
{
  	//M45PE_CS=0;                            //ʹ������   
   
	while ((M45PE_ReadSR()&0x01)!=0);   // �ȴ�BUSYλ���

	//M45PE_CS=1;                            //ȡ��Ƭѡ     	      
}  
//�������ģʽ
void M45PE_PowerDown()   
{ 
	uint8_t i;
  	M45PE_CS=0;                            //ʹ������   
    SPI_ReadWriteByte(M45P_PowerDown);        //���͵�������  
	M45PE_CS=1;                            //ȡ��Ƭѡ     	      
    for(i=5;i>0;i--);                               //�ȴ�TPD  
}   
//����
void M45PE_WAKEUP()   
{  
	uint8_t i;
  	M45PE_CS=0;                            //ʹ������   
    SPI_ReadWriteByte(M45P_ReleasePowerDown);   //  send M45PE_PowerDown command 0xAB    
	M45PE_CS=1;                            //ȡ��Ƭѡ     	      
    for(i=5;i>0;i--);                               //�ȴ�TRES1
}   

