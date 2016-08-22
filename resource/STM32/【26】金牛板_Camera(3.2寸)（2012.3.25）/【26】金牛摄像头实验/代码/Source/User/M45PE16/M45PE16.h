#ifndef __M45PE16_H
#define __M45PE16_H			    

#define	M45PE_CS PCout(7)  //选中FLASH					 
////////////////////////////////////////////////////////////////////////////
//M45PE16读写
//指令表
#define M45P_WriteEnable		0x06 
#define M45P_WriteDisable		0x04 
#define M45P_ReadStatusReg		0x05 
#define M45P_ReadData			0x03 
#define M45P_FastReadData		0x0B 
#define M45P_PageProgram		0x02 
#define M45P_PageErase			0xDB 
#define M45P_SectorErase		0xD8   //0x20 
#define M45P_PowerDown			0xB9 
#define M45P_ReleasePowerDown	0xAB 
#define M45P_JedecDeviceID		0x9F 

void M45PE_Init(void);
uint32_t  M45PE_ReadID(void);  	    //读取FLASH ID
uint8_t	 M45PE_ReadSR(void);        //读取状态寄存器 
void M45PE_Write_SR(uint8_t sr);  	//写状态寄存器
void M45PE_Write_Enable(void);  //写使能 
void M45PE_Write_Disable(void);	//写保护
void M45PE_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   //读取flash
void M45PE_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//写入flash
void M45PE_PageErase(uint32_t PageAddr);
void M45PE_Erase_Sector(uint32_t Dst_Addr);//扇区擦除
void M45PE_Wait_Busy(void);           //等待空闲
void M45PE_PowerDown(void);           //进入掉电模式
void M45PE_WAKEUP(void);			  //唤醒
#endif

