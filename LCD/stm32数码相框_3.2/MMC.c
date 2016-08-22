
#include "MMC.h"

unsigned char  Init_Flag;    //Set it to 1 when Init is processing.
unsigned char  card_type;  

void NOP(void);

unsigned char CMD[] = {0x40,0x00,0x00,0x00,0x00,0x95};//CMD0

/**********************************************************

 ��ʱ�Ӻ���

**********************************************************/
void delay(unsigned int time)
{
 while(time--);
}

/**********************************************************

 us��ʱ�ӳ���  (4.34us)

**********************************************************/
void delayNOP()
{
  NOP();
  NOP();
  NOP();
  NOP();
}

/**********************************************************

 ��ʱ���� 

**********************************************************/
void NOP()
{ vu16 i;
  for(i=0; i<10; i++); 
}

/************************************************************

 ���˿ڳ�ʼ��

************************************************************/
void MMC_Port_Init()
{
//   SPI_CLK=1;
 GPIO_SetBits(GPIOA,SPI_CLK);    //��λLCD_RS
//   SPI_DO =1;
 GPIO_SetBits(GPIOA,SPI_DO);
//   SPI_CS=1;  
 GPIO_SetBits(GPIOA,SPI_CS);           
}

/******************************************************************
 - ����������IOģ��SPI������һ���ֽ�
 - ����ģ�飺SD��ģ��
 - �������ԣ��ڲ�
 - ����˵����x��Ҫ���͵��ֽ�
 - ����˵�����޷���
 - ע��      ����is_initΪ1ʱ��д���ٶȷ�������ʼ��SD��SPI�ٶȲ���̫��
 ******************************************************************/
void Write_Byte_MMC(uchar value)
{ 
   unsigned char i;

   for (i=0;i<8;i++)
   { 
     if (value&0x80)
	 // SPI_DI=1;      //Send bit by bit(MSB First)
	  GPIO_SetBits(GPIOA,SPI_DI); 
     else 
	  //SPI_DI=0;
	  GPIO_ResetBits(GPIOA,SPI_DI);

    // SPI_CLK=0;         //Clock low
	  GPIO_ResetBits(GPIOA, SPI_CLK);
     if(Init_Flag)
	  delayNOP();
     //SPI_CLK=1;        //Clock High
	 GPIO_SetBits(GPIOA,SPI_CLK);
     if(Init_Flag)
	  delayNOP();
     value <<=1;
   }
}

/******************************************************************
 - ����������IOģ��SPI����ȡһ���ֽ�
 - ����ģ�飺SD��ģ��
 - �������ԣ��ڲ�
 - ����˵������
 - ����˵�������ض������ֽ�
 ******************************************************************/
unsigned char Read_Byte_MMC()
{ 
   unsigned char temp=0;
   unsigned char i;
   //SPI_DO=1;
    GPIO_SetBits(GPIOA,SPI_DO);

   for (i=0;i<8;i++)    //MSB First
   {
     //SPI_CLK=0;         //Clock Low
	 GPIO_ResetBits(GPIOA, SPI_CLK);
     if(Init_Flag)
	 delayNOP();   
     //SPI_CLK=1;         //Clock High
	  GPIO_SetBits(GPIOA,SPI_CLK);
     if(Init_Flag) 
	 delayNOP();	

	 temp=(temp<<1)|GPIO_ReadInputDataBit(GPIOA , SPI_DO); 	  //read mmc data out pin 
   }
   return (temp);
}

/******************************************************************
 - ������������SD��д����
 - ����ģ�飺SD��ģ��
 - �������ԣ��ڲ�
 - ����˵����SD����������6���ֽڣ�pcmd��ָ�������ֽ����е�ָ��
 - ����˵��������д���SD���Ļ�Ӧֵ�����ò��ɹ���������0xff
 ******************************************************************/
unsigned char Write_Command_MMC(unsigned char *CMD)
{
   unsigned char tmp;
   unsigned char i;


   GPIO_SetBits(GPIOA,SPI_CS);
   Write_Byte_MMC(0xFF); //��߼����ԣ����û�������ЩSD�����ܲ�֧��  
   GPIO_ResetBits(GPIOA,SPI_CS);

   //send 6 Byte Command to MMC/SD-Card
   for (i=0;i<0x06;i++) 
   { 
      Write_Byte_MMC(*CMD++);
   }
   i = 0;
   //get 16 bit response
   Read_Byte_MMC(); //read the first byte,ignore it. 
   do 
   {  //Only last 8 bit is used here.Read it out. 
      tmp = Read_Byte_MMC();
      i++;
   }
   while((tmp==0xff)&&(i<100)); 
   return(tmp);
}


/******************************************************************
 - ������������λSD�����õ�CMD0��ʹ��SD���л���SPIģʽ
 - ����ģ�飺SD��ģ��
 - �������ԣ��ڲ�
 - ����˵������
 - ����˵�������óɹ�������0x00�����򷵻�INIT_CMD0_ERROR (sd.h���ж���)
 ******************************************************************/
unsigned char MMC_Reset(void)//SD����λ������SPIģʽ��ʹ��CMD0��0�����
{
 	uchar time,temp,i;
 	uchar pcmd[] = {0x40,0x00,0x00,0x00,0x00,0x95}; 
	
   //SD_SCL=1;
   //SD_SO =1;
   //SD_CS=1; 
   GPIO_SetBits(GPIOA,SPI_CS);

	Init_Flag=1; //Set the init flag

 	for(i=0;i<0x0f;i++) //��ʼʱ������Ҫ��������74��ʱ���źţ����Ǳ���ģ�����
 	{
 		 Write_Byte_MMC(0xff); //120 clk
 	}


 	//SD_CS=0;
	GPIO_ResetBits(GPIOA,SPI_CS);	 
 	time=0;
 	do
 	{ 
		temp=Write_Command_MMC(pcmd);//д��CMD0
		time++;
		if(time>=TRY_TIME) 
		{ 
			return(INIT_CMD0_ERROR);//CMD0д��ʧ��
		}
	}while(temp!=0x01);

	//SD_CS=1;
	GPIO_SetBits(GPIOA,SPI_CS);

//	SD_spi_write(0xff); //����SD���Ĳ���ʱ�������ﲹ8��ʱ�� 
	return 0;//����0,˵����λ�����ɹ�
}

/******************************************************************
 - ������������ʼ��SD����ʹ��CMD1
 - ����ģ�飺SD��ģ��
 - �������ԣ��ڲ�
 - ����˵������
 - ����˵�������óɹ�������0x00�����򷵻�INIT_CMD1_ERROR (sd.h���ж���)
 ******************************************************************/
unsigned char MMC_Init(void)	//��ʼ����ʹ��CMD1��1�����
{  
	uchar time,temp;
	uchar pcmd[] = {0x41,0x00,0x00,0x00,0x00,0xff};

	//SD_CS=0;
	GPIO_ResetBits(GPIOA,SPI_CS);	 

	time=0;
	do
	{ 
		temp=Write_Command_MMC(pcmd);
		time++;
		if(time==TRY_TIME) 
		{ 
			return(INIT_CMD1_ERROR);//CMD1д��ʧ��
		}
	}while(temp!=0);
   		 
	Init_Flag=0; //��ʼ����ϣ���is_init����Ϊ0,Ϊ������Ժ�����ݴ����ٶ�
//	#ifdef STC_SPI
//	SPCTL = 0xDC;	//����������SS,CPU_CLK/4 
//	#endif
   
	//SD_CS=1;  //��װSD����Ƭѡ 
	GPIO_SetBits(GPIOA,SPI_CS);

	Write_Byte_MMC(0xff); //����SD���Ĳ���ʱ�������ﲹ8��ʱ��

	return(0); //����0,˵����ʼ�������ɹ�
}
  

//************************************************************
//Routine for reading data Registers of MMC/SD-Card
//Return 0 if no Error.

//************************************************************
unsigned char MMC_Read_Block(uchar *CMD,uchar *Buffer,uint Bytes)
{  
   unsigned int i; 
   unsigned  char temp;
    
   //Send Command CMD to MMC/SD-Card
   i = 0;
   do
   {  //Retry 100 times to send command.
      temp=Write_Command_MMC(CMD);
      i++;
      if(i == 100) 
      {
        return(READ_BLOCK_ERROR); //block write Error!
      }
   }
   while(temp!=0); 
   			
   //Read Start Byte form MMC/SD-Card (FEh/Start Byte)
   while (Read_Byte_MMC()!=0xfe) ;
	
   //Write blocks(normal 512Bytes) to MMC/SD-Card
   for (i=0;i<Bytes;i++)
   {
      *Buffer++ = Read_Byte_MMC();
   }
   
   Read_Byte_MMC();//CRC - Byte 
   Read_Byte_MMC();//CRC - Byte
	
   //SPI_CS=1;
   GPIO_SetBits(GPIOA , SPI_CS);
  
   return(0);
}

//************************************************************
//Routine for reading CSD Registers from MMC/SD-Card (16Bytes)
//Return 0 if no Error.
//Command for reading CSD Registers
//************************************************************
unsigned char Read_CSD_MMC(unsigned char *Buffer)
{	   
   unsigned char temp;
   CMD[0]=0X49;		//CMD9:��CSD�Ĵ���.
   CMD[5]=0XFF;

   temp=MMC_Read_Block(CMD,Buffer,16); //read 16 bytes
   return(temp);
}

//************************************************************
//Routine for reading CID Registers from MMC/SD-Card (16Bytes) 
//Return 0 if no Error.
//Command for reading CID Registers
//************************************************************
unsigned char Read_CID_MMC(unsigned char *Buffer)
{
   unsigned char temp;
   CMD[0]=0X4A;	   //CMD10:��CID�Ĵ���.
   CMD[5]=0XFF;

   temp=MMC_Read_Block(CMD,Buffer,16); //read 16 bytes
   return(temp);
}




/****************************************************************************
 - ������������ȡaddr������512���ֽڵ�bufferָ������ݻ�����
 - ����ģ�飺SD��ģ��
 - �������ԣ��ڲ�
 - ����˵����addr:������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻�READ_BLOCK_ERROR (sd.h���ж���)
 - ע��SD����ʼ���ɹ��󣬶�д����ʱ��������SPI�ٶ������������Ч��
 ****************************************************************************/
unsigned char SD_Read_Sector(unsigned long addr,unsigned char *buffer)//��SD����ָ�������ж���512���ֽڣ�ʹ��CMD17��17�����
{
	uint j;
	uchar time,temp;
	uchar pcmd[]={0x51,0x00,0x00,0x00,0x00,0xFF}; //CMD17���ֽ�����
   
	addr<<=9; //sector = sector * 512	   �����ַ��������ַ��תΪ�ֽڵ�ַ

	pcmd[1]=((addr&0xFF000000)>>24);
	pcmd[2]=((addr&0x00FF0000)>>16);
	pcmd[3]=((addr&0x0000FF00)>>8);

	//SD_CS=0;//��Ƭѡ 
	GPIO_ResetBits(GPIOA,SPI_CS);
	time=0;
	do
	{  
		temp=Write_Command_MMC(pcmd); //д��CMD17
		time++;
		if(time==TRY_TIME) 
		{
			return(READ_BLOCK_ERROR); //����ʧ��
		}
	}while(temp!=0); 
   			
 	while (Read_Byte_MMC() != 0xfe); //һֱ����������0xfeʱ��˵���������512�ֽڵ�������

	for(j=0;j<512;j++)	 //������д�뵽���ݻ�������
	{	
		buffer[j]=Read_Byte_MMC();
	}

	Read_Byte_MMC();
	Read_Byte_MMC();//��ȡ�����ֽڵ�CRCУ���룬���ù�������
	//SD_CS=1;  //SD���ر�Ƭѡ
	GPIO_SetBits(GPIOA,SPI_CS); 


	Write_Byte_MMC(0xFF);//����SD���Ĳ���ʱ�������ﲹ8��ʱ��
	return 0;
}
//---------------------------------------------------SD������--------------------------------

