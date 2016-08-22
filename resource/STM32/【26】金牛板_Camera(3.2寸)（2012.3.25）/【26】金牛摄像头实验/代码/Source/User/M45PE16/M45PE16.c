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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOC,&GPIO_InitStructure); 

	M45PE_CS = 1;                            //关闭器件   
}
//读取M45P的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
uint8_t M45PE_ReadSR(void)   
{  
	uint8_t byte=0;   
	M45PE_CS=0;                            //使能器件   
	SPI_ReadWriteByte(M45P_ReadStatusReg);    //发送读取状态寄存器命令    
	byte=SPI_ReadWriteByte(0Xff);             //读取一个字节  
	M45PE_CS=1;                            //取消片选     
	return byte;   
}   
//M45P写使能	
//将WEL置位   
void M45PE_Write_Enable(void)   
{
	M45PE_CS=0;                            //使能器件   
    SPI_ReadWriteByte(M45P_WriteEnable);      //发送写使能  
	M45PE_CS=1;                            //取消片选     	      
} 
//M45P写禁止	
//将WEL清零  
void M45PE_Write_Disable(void)   
{  
	M45PE_CS=0;                            //使能器件   
    SPI_ReadWriteByte(M45P_WriteDisable);     //发送写禁止指令    
	M45PE_CS=1;                            //取消片选     	      
} 			    
//读取芯片ID M45P16的ID:0X204015
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
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void M45PE_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;    												    
	M45PE_CS=0;                            //使能器件   
    SPI_ReadWriteByte(M45P_ReadData);         //发送读取命令   
    SPI_ReadWriteByte((uint8_t)((ReadAddr)>>16));  //发送24bit地址    
    SPI_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    SPI_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI_ReadWriteByte(0XFF);   //循环读数  
    }
	M45PE_CS=1;                            //取消片选     	      
}  
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void M45PE_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    M45PE_Write_Enable();                  //SET WEL 
	M45PE_CS=0;                            //使能器件   
    SPI_ReadWriteByte(M45P_PageProgram);      //发送写页命令   
    SPI_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //发送24bit地址    
    SPI_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    SPI_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI_ReadWriteByte(pBuffer[i]);//循环写数  
	M45PE_CS=1;                            //取消片选 
	M45PE_Wait_Busy();					   //等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void M45PE_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		M45PE_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)  		   
uint8_t M45PE_BUF[4096];
void M45PE_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    

	secpos=WriteAddr/4096;//扇区地址 0~511 for M45P16
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		M45PE_Read(M45PE_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(M45PE_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			M45PE_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				M45PE_BUF[i+secoff]=pBuffer[i];	  
			}
			M45PE_Write_NoCheck(M45PE_BUF,secpos*4096,4096);//写入整个扇区  

		}else M45PE_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
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
  M45PE_CS=0;                       //使能器件   
  /* Send Sector Erase instruction */
  SPI_ReadWriteByte(M45P_PageErase);
  /* Send SectorAddr high nibble address byte */
  SPI_ReadWriteByte((PageAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPI_ReadWriteByte((PageAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPI_ReadWriteByte(PageAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  M45PE_CS=1;                            //使能器件   

  /* Wait the end of Flash writing */
  M45PE_Wait_Busy();
}
//擦除一个扇区
//Dst_Addr:扇区地址 0~511 for M45P16
//擦除一个山区的最少时间:150ms
void M45PE_Erase_Sector(uint32_t Dst_Addr)   
{   
	Dst_Addr*=4096;
    M45PE_Write_Enable();                  //SET WEL 	 
  	M45PE_CS=0;                            //使能器件   
    SPI_ReadWriteByte(M45P_SectorErase);      //发送扇区擦除指令 
    SPI_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //发送24bit地址    
    SPI_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    SPI_ReadWriteByte((uint8_t)Dst_Addr);  
	M45PE_CS=1;                            //取消片选     	      
    M45PE_Wait_Busy();   				   //等待擦除完成
}  
//等待空闲
void M45PE_Wait_Busy(void)   
{
  	//M45PE_CS=0;                            //使能器件   
   
	while ((M45PE_ReadSR()&0x01)!=0);   // 等待BUSY位清空

	//M45PE_CS=1;                            //取消片选     	      
}  
//进入掉电模式
void M45PE_PowerDown()   
{ 
	uint8_t i;
  	M45PE_CS=0;                            //使能器件   
    SPI_ReadWriteByte(M45P_PowerDown);        //发送掉电命令  
	M45PE_CS=1;                            //取消片选     	      
    for(i=5;i>0;i--);                               //等待TPD  
}   
//唤醒
void M45PE_WAKEUP()   
{  
	uint8_t i;
  	M45PE_CS=0;                            //使能器件   
    SPI_ReadWriteByte(M45P_ReleasePowerDown);   //  send M45PE_PowerDown command 0xAB    
	M45PE_CS=1;                            //取消片选     	      
    for(i=5;i>0;i--);                               //等待TRES1
}   

