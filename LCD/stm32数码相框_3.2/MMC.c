
#include "MMC.h"

unsigned char  Init_Flag;    //Set it to 1 when Init is processing.
unsigned char  card_type;  

void NOP(void);

unsigned char CMD[] = {0x40,0x00,0x00,0x00,0x00,0x95};//CMD0

/**********************************************************

 延时子函数

**********************************************************/
void delay(unsigned int time)
{
 while(time--);
}

/**********************************************************

 us延时子程序  (4.34us)

**********************************************************/
void delayNOP()
{
  NOP();
  NOP();
  NOP();
  NOP();
}

/**********************************************************

 延时程序 

**********************************************************/
void NOP()
{ vu16 i;
  for(i=0; i<10; i++); 
}

/************************************************************

 卡端口初始化

************************************************************/
void MMC_Port_Init()
{
//   SPI_CLK=1;
 GPIO_SetBits(GPIOA,SPI_CLK);    //置位LCD_RS
//   SPI_DO =1;
 GPIO_SetBits(GPIOA,SPI_DO);
//   SPI_CS=1;  
 GPIO_SetBits(GPIOA,SPI_CS);           
}

/******************************************************************
 - 功能描述：IO模拟SPI，发送一个字节
 - 隶属模块：SD卡模块
 - 函数属性：内部
 - 参数说明：x是要发送的字节
 - 返回说明：无返回
 - 注：      其中is_init为1时，写的速度放慢，初始化SD卡SPI速度不能太高
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
 - 功能描述：IO模拟SPI，读取一个字节
 - 隶属模块：SD卡模块
 - 函数属性：内部
 - 参数说明：无
 - 返回说明：返回读到的字节
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
 - 功能描述：向SD卡写命令
 - 隶属模块：SD卡模块
 - 函数属性：内部
 - 参数说明：SD卡的命令是6个字节，pcmd是指向命令字节序列的指针
 - 返回说明：命令写入后，SD卡的回应值，调用不成功，将返回0xff
 ******************************************************************/
unsigned char Write_Command_MMC(unsigned char *CMD)
{
   unsigned char tmp;
   unsigned char i;


   GPIO_SetBits(GPIOA,SPI_CS);
   Write_Byte_MMC(0xFF); //提高兼容性，如果没有这里，有些SD卡可能不支持  
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
 - 功能描述：复位SD卡，用到CMD0，使用SD卡切换到SPI模式
 - 隶属模块：SD卡模块
 - 函数属性：内部
 - 参数说明：无
 - 返回说明：调用成功，返回0x00，否则返回INIT_CMD0_ERROR (sd.h中有定义)
 ******************************************************************/
unsigned char MMC_Reset(void)//SD卡复位，进入SPI模式，使用CMD0（0号命令）
{
 	uchar time,temp,i;
 	uchar pcmd[] = {0x40,0x00,0x00,0x00,0x00,0x95}; 
	
   //SD_SCL=1;
   //SD_SO =1;
   //SD_CS=1; 
   GPIO_SetBits(GPIOA,SPI_CS);

	Init_Flag=1; //Set the init flag

 	for(i=0;i<0x0f;i++) //初始时，首先要发送最少74个时钟信号，这是必须的！！！
 	{
 		 Write_Byte_MMC(0xff); //120 clk
 	}


 	//SD_CS=0;
	GPIO_ResetBits(GPIOA,SPI_CS);	 
 	time=0;
 	do
 	{ 
		temp=Write_Command_MMC(pcmd);//写入CMD0
		time++;
		if(time>=TRY_TIME) 
		{ 
			return(INIT_CMD0_ERROR);//CMD0写入失败
		}
	}while(temp!=0x01);

	//SD_CS=1;
	GPIO_SetBits(GPIOA,SPI_CS);

//	SD_spi_write(0xff); //按照SD卡的操作时序在这里补8个时钟 
	return 0;//返回0,说明复位操作成功
}

/******************************************************************
 - 功能描述：初始化SD卡，使用CMD1
 - 隶属模块：SD卡模块
 - 函数属性：内部
 - 参数说明：无
 - 返回说明：调用成功，返回0x00，否则返回INIT_CMD1_ERROR (sd.h中有定义)
 ******************************************************************/
unsigned char MMC_Init(void)	//初始化，使用CMD1（1号命令）
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
			return(INIT_CMD1_ERROR);//CMD1写入失败
		}
	}while(temp!=0);
   		 
	Init_Flag=0; //初始化完毕，将is_init设置为0,为了提高以后的数据传输速度
//	#ifdef STC_SPI
//	SPCTL = 0xDC;	//主机，忽略SS,CPU_CLK/4 
//	#endif
   
	//SD_CS=1;  //关装SD卡的片选 
	GPIO_SetBits(GPIOA,SPI_CS);

	Write_Byte_MMC(0xff); //按照SD卡的操作时序在这里补8个时钟

	return(0); //返回0,说明初始化操作成功
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
   CMD[0]=0X49;		//CMD9:读CSD寄存器.
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
   CMD[0]=0X4A;	   //CMD10:读CID寄存器.
   CMD[5]=0XFF;

   temp=MMC_Read_Block(CMD,Buffer,16); //read 16 bytes
   return(temp);
}




/****************************************************************************
 - 功能描述：读取addr扇区的512个字节到buffer指向的数据缓冲区
 - 隶属模块：SD卡模块
 - 函数属性：内部
 - 参数说明：addr:扇区地址
             buffer:指向数据缓冲区的指针
 - 返回说明：调用成功，返回0x00，否则返回READ_BLOCK_ERROR (sd.h中有定义)
 - 注：SD卡初始化成功后，读写扇区时，尽量将SPI速度提上来，提高效率
 ****************************************************************************/
unsigned char SD_Read_Sector(unsigned long addr,unsigned char *buffer)//从SD卡的指定扇区中读出512个字节，使用CMD17（17号命令）
{
	uint j;
	uchar time,temp;
	uchar pcmd[]={0x51,0x00,0x00,0x00,0x00,0xFF}; //CMD17的字节序列
   
	addr<<=9; //sector = sector * 512	   将块地址（扇区地址）转为字节地址

	pcmd[1]=((addr&0xFF000000)>>24);
	pcmd[2]=((addr&0x00FF0000)>>16);
	pcmd[3]=((addr&0x0000FF00)>>8);

	//SD_CS=0;//打开片选 
	GPIO_ResetBits(GPIOA,SPI_CS);
	time=0;
	do
	{  
		temp=Write_Command_MMC(pcmd); //写入CMD17
		time++;
		if(time==TRY_TIME) 
		{
			return(READ_BLOCK_ERROR); //读块失败
		}
	}while(temp!=0); 
   			
 	while (Read_Byte_MMC() != 0xfe); //一直读，当读到0xfe时，说明后面的是512字节的数据了

	for(j=0;j<512;j++)	 //将数据写入到数据缓冲区中
	{	
		buffer[j]=Read_Byte_MMC();
	}

	Read_Byte_MMC();
	Read_Byte_MMC();//读取两个字节的CRC校验码，不用关心它们
	//SD_CS=1;  //SD卡关闭片选
	GPIO_SetBits(GPIOA,SPI_CS); 


	Write_Byte_MMC(0xFF);//按照SD卡的操作时序在这里补8个时钟
	return 0;
}
//---------------------------------------------------SD卡驱动--------------------------------

