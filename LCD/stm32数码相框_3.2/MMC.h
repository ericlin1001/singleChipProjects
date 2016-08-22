/**********************************************************

file:  MMC.h
**********************************************************/
#include "stm32f10x_lib.h"
#define uchar unsigned char
#define uint  unsigned int
#define ulong  unsigned long

#define SPI_CS    GPIO_Pin_12    //=P3^4;
#define SPI_DI    GPIO_Pin_13    //=P3^5;
#define SPI_CLK   GPIO_Pin_14    //=P3^6;
#define SPI_DO    GPIO_Pin_15    //=P3^7;
#define T_CS_PIN  GPIO_Pin_7	 //T_CS  引脚定义 P2^0
#define T_CS(x)   x ? GPIO_SetBits(GPIOB, T_CS_PIN) :   GPIO_ResetBits(GPIOB, T_CS_PIN)


#define TRY_TIME 200   //向SD卡写入命令之后，读取SD卡的回应次数，即读TRY_TIME次，如果在TRY_TIME次中读不到回应，产生超时错误，命令写入失败

// Error define
#define INIT_CMD0_ERROR     0x01
#define INIT_CMD1_ERROR		0x02
#define WRITE_BLOCK_ERROR	0x03
#define READ_BLOCK_ERROR   	0x04 



extern unsigned char  Init_Flag;    
extern unsigned char card_type;  


extern  void delay(unsigned int time);
extern  void delayNOP(void);
extern  void MMC_Port_Init(void);
extern  void Write_Byte_MMC(uchar value);
extern  unsigned char Read_Byte_MMC(void);
extern  unsigned char Write_Command_MMC(unsigned char *CMD);
extern  unsigned char MMC_Init(void);
extern  unsigned char MMC_Read_Block(uchar *CMD,uchar *Buffer,uint Bytes);
extern  unsigned char Read_CSD_MMC(unsigned char *Buffer);
extern  unsigned char Read_CID_MMC(unsigned char *Buffer);
extern  unsigned char SD_Read_Sector(unsigned long addr,unsigned char *buffer);


extern  unsigned char MMC_Reset(void);
extern  unsigned char MMC_Init(void);

