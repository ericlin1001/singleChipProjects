/**************************************************************
** 	火牛开发板
**	LCD驱动代码
**  版本：V1.0  
**	论坛：www.openmcu.com
**	淘宝：http://shop36995246.taobao.com/   
**  技术支持群：121939788 
***************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "hardware_conf.h"
#include "ili932x_conf.h"
#include "Font_Lib.h"	 //字库文件库
#include  "hz16.h"
#include "ov7670.h"	

static uint16_t DeviceCode;

uint16_t POINT_COLOR = BLUE,BACK_COLOR = WHITE;  /* 分别设置点的颜色和底色	*/

/*****************************************************************************
** 函数名称: LCD_Write_Reg
** 功能描述: 写指令及数据
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_Dat)
{
	Write_Cmd(LCD_Reg);
	Write_Dat(LCD_Dat);
}
/*****************************************************************************
** 函数名称: Write_Cmd
** 功能描述: 写指令
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void Write_Cmd(uint16_t LCD_Reg)
{
	LCD_CS = 0;
	LCD_RS = 0;
	GPIOE->ODR = LCD_Reg;
	LCD_WR = 0;
	LCD_WR = 1;	
	LCD_CS = 1;
}
/*****************************************************************************
** 函数名称: Write_Dat
** 功能描述: 写数据
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void Write_Dat(uint16_t LCD_Dat)
{
	LCD_CS = 0;
	LCD_RS = 1;
	GPIOE->ODR = LCD_Dat;
	LCD_WR = 0;
	LCD_WR = 1;	
	LCD_CS = 1;
}
/*****************************************************************************
** 函数名称: LCD_ReadReg
** 功能描述: 读指令
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	uint16_t temp;

	Write_Cmd(LCD_Reg);  //写入要读的寄存器号  
	GPIOE->CRH = (GPIOE->CRH & 0x00000000) | 0x44444444;  //将端口高8位配置成输入
	GPIOE->CRL = (GPIOE->CRL & 0x00000000) | 0x44444444;  //将端口低8位配置成输入
	LCD_CS = 0;
	LCD_RS = 1;
	LCD_RD = 0;
	temp = GPIOE->IDR&0xFFFF; 	//读取数据(读寄存器时,并不需要读2次)
	LCD_RD = 1;
	LCD_CS = 1;
	GPIOE->CRH = (GPIOE->CRH & 0x00000000) | 0x33333333;  //释放端口高8位为输出
	GPIOE->CRL = (GPIOE->CRL & 0x00000000) | 0x33333333;  //释放端口低8位为输出

	return temp;   
}
/*****************************************************************************
** 函数名称: LCD_ReadDat
** 功能描述: 读数据
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
uint16_t LCD_ReadDat()
{
	uint16_t temp;

	GPIOE->CRH = (GPIOE->CRH & 0x00000000) | 0x44444444;  //将端口高8位配置成输入
	GPIOE->CRL = (GPIOE->CRL & 0x00000000) | 0x44444444;  //将端口低8位配置成输入
	LCD_CS = 0;
	LCD_RS = 1;
	LCD_RD = 0;
	temp = GPIOE->IDR; 	//读取数据(读寄存器时,并不需要读2次)
	//temp = GPIOE->IDR;
	LCD_RD = 1;
	LCD_CS = 1;
	GPIOE->CRH = (GPIOE->CRH & 0x00000000) | 0x33333333;  //释放端口高8位为输出
	GPIOE->CRL = (GPIOE->CRL & 0x00000000) | 0x33333333;  //释放端口低8位为输出

	return temp;   
}
/*****************************************************************************
** 函数名称: LCD_Configuration
** 功能描述: LCD_IO口配置
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC 
							| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);
	
	/* 配置控制IO 连接到PD12.PD13.PD14.PD15 *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 推挽输出方式
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 输出IO口最大最速为50MHZ
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 推挽输出方式
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 输出IO口最大最速为50MHZ
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* 配置数据IO 连接到GPIOE *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 推挽输出方式
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 输出IO口最大最速为50MHZ
  	GPIO_Init(GPIOE, &GPIO_InitStructure);

}
/*****************************************************************************
** 函数名称: LCD_Init
** 功能描述: LCD初始化
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_Init(void)
{
	LCD_Configuration();
	LCD_WriteReg(0x0000,0x0001);
	LCD_Delay(5); // LCD_Delay 50 ms 
	DeviceCode = LCD_ReadReg(0x0000);   
	//printf("ID:%d\n",DeviceCode); 
//	DeviceCode = 0x9325;
	if(DeviceCode==0x9325||DeviceCode==0x9328)//ILI9325
	{
		LCD_WriteReg(0x00E3,0x3008);        
		LCD_WriteReg(0x00E7,0x0012);
		LCD_WriteReg(0x00Ef,0x1231); 	// Set the internal timing;    
        LCD_WriteReg(0x00,0x0001);
        LCD_WriteReg(0x01, 0x0000); // set SS and SM bit
        LCD_WriteReg(0x02, 0x0700); // set 1 line inversion
        //LCD_WriteReg(0x03, 0x10B0); // set GRAM write direction and BGR=1.
#if   ID_AM==000       
	    LCD_WriteReg(0x0003,0x1000);//屏幕旋转控制 TFM=0,TRI=0,SWAP=1,16 bits system interface  swap RGB to BRG，此处ORG和HWM 为0
#elif ID_AM==001        
	    LCD_WriteReg(0x0003,0x1008);      
#elif ID_AM==010  
	    LCD_WriteReg(0x0003,0x1010);        
#elif ID_AM==011
	    LCD_WriteReg(0x0003,0x1018);
#elif ID_AM==100  
	    LCD_WriteReg(0x0003,0x1020);      
#elif ID_AM==101  
	    LCD_WriteReg(0x0003,0x1028);      
#elif ID_AM==110  
	    LCD_WriteReg(0x0003,0x1030);      
#elif ID_AM==111  
	    LCD_WriteReg(0x0003,0x1038);
#endif    
        LCD_WriteReg(0x04, 0x0000); // Resize register
		LCD_WriteReg(0x08, 0x0404); // set the back porch and front porch
        LCD_WriteReg(0x09, 0x0000); // set non-display area refresh cycle ISC[3:0]
        LCD_WriteReg(0x0A, 0x0000); // FMARK function
        // LCD_WriteReg(0x0C, 0x0000); // RGB interface setting
        // LCD_WriteReg(0x0D, 0x0000); // Frame marker Position
        // LCD_WriteReg(0x0F, 0x0000); // RGB interface polarity
//Power On sequence //
        LCD_WriteReg(0x10, 0x0080); // SAP, BT[3:0], AP, DSTB, SLP, STB
        LCD_WriteReg(0x11, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
        LCD_WriteReg(0x12, 0x0000); // VREG1OUT voltage
        LCD_WriteReg(0x13, 0x0000); // VDV[4:0] for VCOM amplitude
	 	LCD_WriteReg(0x07, 0x0001); // 
		 
        DelayNS(10000); // Dis-charge capacitor power voltage
         
        LCD_WriteReg(0x10, 0x1590); // SAP, BT[3:0], AP, DSTB, SLP, STB
        LCD_WriteReg(0x11, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
        DelayNS(10000); // Delay 50ms
        
        LCD_WriteReg(0x12, 0x001F); // Internal reference voltage= Vci;
        DelayNS(10000); // Delay 50ms
        LCD_WriteReg(0x13, 0x1500); // Set VDV[4:0] for VCOM amplitude
        LCD_WriteReg(0x29, 0x0022); // Set VCM[5:0] for VCOMH
        LCD_WriteReg(0x2B, 0x000f); // Set Frame Rate
        DelayNS(10000);// Delay 580ms
         
#if   ID_AM==000         
	    LCD_WriteReg(0x0020,0x00ef);//GRAM水平起始位置
	    LCD_WriteReg(0x0021,0x013f);      
#elif ID_AM==001
	    LCD_WriteReg(0x0020,0x00ef);
	    LCD_WriteReg(0x0021,0x013f);      
#elif ID_AM==010
	    LCD_WriteReg(0x0020,0x0000);
	    LCD_WriteReg(0x0021,0x013f);      
#elif ID_AM==011
	    LCD_WriteReg(0x0020,0x0000);
	    LCD_WriteReg(0x0021,0x013f);       
#elif ID_AM==100
	    LCD_WriteReg(0x0020,0x00ef);
	    LCD_WriteReg(0x0021,0x0000);      
#elif ID_AM==101  
	    LCD_WriteReg(0x0020,0x00ef);
	    LCD_WriteReg(0x0021,0x0000);      
#elif ID_AM==110
	    LCD_WriteReg(0x0020,0x0000);
	    LCD_WriteReg(0x0021,0x0000);      
#elif ID_AM==111
	    LCD_WriteReg(0x0020,0x0000);
	    LCD_WriteReg(0x0021,0x0000);         
#endif  
// ----------- Adjust the Gamma Curve ----------//
		LCD_WriteReg(0x0030,0x0101);
		LCD_WriteReg(0x0031,0x0707);
		LCD_WriteReg(0x0032,0x0505);
		LCD_WriteReg(0x0035,0x0407);
		LCD_WriteReg(0x0036,0x0102);
		LCD_WriteReg(0x0037,0x0102);
		LCD_WriteReg(0x0038,0x0202);
		LCD_WriteReg(0x0039,0x0106);
		LCD_WriteReg(0x003C,0x0202);
		LCD_WriteReg(0x003D,0x0706);
//------------------ Set GRAM area ---------------//
        LCD_WriteReg(0x0050, 0x0000); // Horizontal GRAM Start Address
        LCD_WriteReg(0x0051, 0x00EF); // Horizontal GRAM End Address
        LCD_WriteReg(0x0052, 0x0000); // Vertical GRAM Start Address
        LCD_WriteReg(0x0053, 0x013F); // Vertical GRAM Start Address
        LCD_WriteReg(0x0060, 0xA700); // Gate Scan Line
        LCD_WriteReg(0x0061, 0x0001); // NDL,VLE, REV
        LCD_WriteReg(0x006A, 0x0000); // set scrolling line
//-------------- Partial Display Control ---------//
        LCD_WriteReg(0x80, 0x0000);
        LCD_WriteReg(0x81, 0x0000);
        LCD_WriteReg(0x82, 0x0000);
        LCD_WriteReg(0x83, 0x0000);
        LCD_WriteReg(0x84, 0x0000);
        LCD_WriteReg(0x85, 0x0000);
//-------------- Panel Control -------------------//
        LCD_WriteReg(0x90, 0x0010);
        LCD_WriteReg(0x92, 0x0000);
        LCD_WriteReg(0x93, 0x0003);
        LCD_WriteReg(0x95, 0x0110);
        LCD_WriteReg(0x97, 0x0000);
        LCD_WriteReg(0x98, 0x0000);
        LCD_WriteReg(0x07, 0x0173); // 262K color and display ON   0133
        DelayNS(10000); // Delay 5ms
	}
	else if(DeviceCode==0x9320||DeviceCode==0x9300)
	{
		LCD_WriteReg(0x00,0x0000);
		LCD_WriteReg(0x01,0x0000);	//Driver Output Contral.
		LCD_WriteReg(0x02,0x0700);	//LCD Driver Waveform Contral.
		LCD_WriteReg(0x03,0x1008);//Entry Mode Set.
	
		LCD_WriteReg(0x04,0x0000);	//Scalling Contral.
		LCD_WriteReg(0x08,0x0202);	//Display Contral 2.(0x0207)
		LCD_WriteReg(0x09,0x0000);	//Display Contral 3.(0x0000)
		LCD_WriteReg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		LCD_WriteReg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		LCD_WriteReg(0x0d,0x0000);	//Frame Maker Position.
		LCD_WriteReg(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
		LCD_Delay(10); 
		LCD_WriteReg(0x07,0x0101);	//Display Contral.
		LCD_Delay(10); 								  
		LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		LCD_WriteReg(0x11,0x0007);								//Power Control 2.(0x0001)
		LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
		LCD_WriteReg(0x13,0x0b00);								//Power Control 4.
		LCD_WriteReg(0x29,0x0000);								//Power Control 7.
	
		LCD_WriteReg(0x2b,(1<<14)|(1<<4));	    
		LCD_WriteReg(0x50,0);	//Set X Star
		//水平GRAM终止位置Set X End.
		LCD_WriteReg(0x51,239);	//Set Y Star
		LCD_WriteReg(0x52,0);	//Set Y End.t.
		LCD_WriteReg(0x53,319);	//
	
		LCD_WriteReg(0x60,0x2700);	//Driver Output Control.
		LCD_WriteReg(0x61,0x0001);	//Driver Output Control.
		LCD_WriteReg(0x6a,0x0000);	//Vertical Srcoll Control.
	
		LCD_WriteReg(0x80,0x0000);	//Display Position? Partial Display 1.
		LCD_WriteReg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		LCD_WriteReg(0x82,0x0000);	//RAM Address End-Partial Display 1.
		LCD_WriteReg(0x83,0x0000);	//Displsy Position? Partial Display 2.
		LCD_WriteReg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		LCD_WriteReg(0x85,0x0000);	//RAM Address End? Partial Display 2.
	
		LCD_WriteReg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		LCD_WriteReg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		LCD_WriteReg(0x93,0x0001);	//Panel Interface Contral 3.
		LCD_WriteReg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		LCD_WriteReg(0x97,(0<<8));	//
		LCD_WriteReg(0x98,0x0000);	//Frame Cycle Contral.	   
		LCD_WriteReg(0x07,0x0173);	//(0x0173)
		LCD_Delay(10);
	}
	else if(DeviceCode==0x8999)	   //对应的驱动IC为SSD1289
	{
		//************* Start Initial Sequence **********//
		LCD_WriteReg(0x00, 0x0001); // Start internal OSC.
		LCD_WriteReg(0x01, 0x7B3F); // Driver output control, RL=0;REV=1;GD=1;BGR=0;SM=0;TB=1
		LCD_WriteReg(0x02, 0x0600); // set 1 line inversion
		//************* Power control setup ************/
		LCD_WriteReg(0x0C, 0x0007); // Adjust VCIX2 output voltage
		LCD_WriteReg(0x0D, 0x0006); // Set amplitude magnification of VLCD63
		LCD_WriteReg(0x0E, 0x3200); // Set alternating amplitude of VCOM
		LCD_WriteReg(0x1E, 0x00BB); // Set VcomH voltage
		LCD_WriteReg(0x03, 0x6A64); // Step-up factor/cycle setting
		//************ RAM position control **********/
		LCD_WriteReg(0x0F, 0x0000); // Gate scan position start at G0.
		LCD_WriteReg(0x44, 0xEF00); // Horizontal RAM address position
		LCD_WriteReg(0x45, 0x0000); // Vertical RAM address start position
		LCD_WriteReg(0x46, 0x013F); // Vertical RAM address end position
		// ----------- Adjust the Gamma Curve ----------//
		LCD_WriteReg(0x30, 0x0000);
		LCD_WriteReg(0x31, 0x0706);
		LCD_WriteReg(0x32, 0x0206);
		LCD_WriteReg(0x33, 0x0300);
		LCD_WriteReg(0x34, 0x0002);
		LCD_WriteReg(0x35, 0x0000);
		LCD_WriteReg(0x36, 0x0707);
		LCD_WriteReg(0x37, 0x0200);
		LCD_WriteReg(0x3A, 0x0908);
		LCD_WriteReg(0x3B, 0x0F0D);
		//************* Special command **************/
		LCD_WriteReg(0x28, 0x0006); // Enable test command
		LCD_WriteReg(0x2F, 0x12EB); // RAM speed tuning
		LCD_WriteReg(0x26, 0x7000); // Internal Bandgap strength
		LCD_WriteReg(0x20, 0xB0E3); // Internal Vcom strength
		LCD_WriteReg(0x27, 0x0044); // Internal Vcomh/VcomL timing
		LCD_WriteReg(0x2E, 0x7E45); // VCOM charge sharing time  
		//************* Turn On display ******************/
		LCD_WriteReg(0x10, 0x0000); // Sleep mode off.
		LCD_Delay(8); // Wait 30mS
		LCD_WriteReg(0x11, 0x6848); // Entry mode setup. 262K type B, take care on the data bus with 16it only
		LCD_WriteReg(0x07, 0x0033); // Display ON	*/
	}	
	LCD_Clear(BACK_COLOR);	
}
/*****************************************************************************
** 函数名称: LCD_DrawPoint
** 功能描述: 写一个点
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_SetCursor(x,y);//设置光标位置 
	Write_Cmd(R34);//开始写入GRAM
	Write_Dat(POINT_COLOR); 	
}
/*****************************************************************************
** 函数名称: LCD_WriteRAM_Prepare
** 功能描述: 些准备
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_WriteRAM_Prepare()
{
	Write_Cmd(R34);	
}
/*****************************************************************************
** 函数名称: LCD_SetCursor
** 功能描述: 设置光标函数
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
 	if(DeviceCode==0x8999||DeviceCode==0x9919)
	{
		LCD_WriteReg(0x004E, 239-Ypos);
		LCD_WriteReg(0X004F, 319-Xpos);
	}
	else 
	{
	#if   ID_AM==000       
		LCD_WriteReg(R32, Xpos);
		LCD_WriteReg(R33, Ypos);
	
	#elif ID_AM==001  
		LCD_WriteReg(R32, 239-Xpos);
		LCD_WriteReg(R33, 319-Ypos);
	#endif 
	}
	
} 
/*****************************************************************************
** 函数名称: LCD_SetDisplayWindow
** 功能描述: 设置窗口函数 (将Width和Height调换一下，前提是)
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width)
{
	//设置窗口	
 	if(DeviceCode==0x8999||DeviceCode==0x9919)
	{
	  	LCD_WriteReg(0x44, Xpos|(Height<<8));	 //水平方向GRAM起始地址(低字节为起始地址，高字节为结束地址)
	  	LCD_WriteReg(0x45, Ypos);		  				//垂直方向GRAM起始地址
	  	LCD_WriteReg(0x46, Width);  				//垂直方向GRAM结束地址

		LCD_WriteReg(0x4E, Xpos);
		LCD_WriteReg(0X4F, Ypos);
	}
	else
	{
	  	LCD_WriteReg(R80, Xpos);	   	   	//水平方向GRAM起始地址
	  	LCD_WriteReg(R81, Height); 	//水平方向GRAM结束地址 
	  	LCD_WriteReg(R82, Ypos);		  	//垂直方向GRAM起始地址
	  	LCD_WriteReg(R83, Width);  	//垂直方向GRAM结束地址

		LCD_WriteReg(0x20, Xpos);
		LCD_WriteReg(0X21, Ypos);
	}

}
/*****************************************************************************
** 函数名称: LCD_ShowString
** 功能描述: 显示字符串函数
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
void LCD_ShowString(uint8_t x,uint16_t y,__I uint8_t *p)
{         
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;LCD_Clear(WHITE);}
        LCD_ShowChar(x,y,*p,16,0);
        x+=8;
        p++;
    }  
}
/*****************************************************************************
** 函数名称: LCD_ShowChar
** 功能描述: 显示一个字符函数
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_ShowChar(uint8_t x,uint16_t y,uint8_t chars,uint8_t size,uint8_t mode)
{
	uint8_t temp;
    uint8_t pos,t;      
    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY) return;	    
											
	LCD_SetDisplayWindow(x,y,(size/2-1),size-1);  //设置窗口

	LCD_WriteRAM_Prepare();        //开始写入GRAM	   
	if(!mode) 						//非叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=ASCII_1206[chars-0x20][pos];//调用1206字体
			else temp=ASCII_1608[chars-0x20][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {     
		        //if(temp&0x01)            	 			//先传低位，取模有关系
		        if((temp<<t)&0x80)						//先传高位
				{
					Write_Dat(RED);
				}
				else 
				{
					Write_Dat(WHITE);	        
		        }
				//temp>>=1; 	   							//如果先传低位，去掉屏蔽线
		    }
		}	
	}
	else//叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=ASCII_1206[chars-0x20][pos];	//调用1206字体
			else temp=ASCII_1608[chars-0x20][pos];		 	//调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if((temp<<t)&0x80)LCD_DrawPoint(x+t,y+pos);	//画一个点     
		        //temp>>=1; 								//如果先传低位，去掉屏蔽线
		    }
		}
	}	    
	/* 恢复窗体大小	*/
	LCD_WriteReg(R80, 0x0000); //水平方向GRAM起始地址
	LCD_WriteReg(R81, 0x00EF); //水平方向GRAM结束地址
	LCD_WriteReg(R82, 0x0000); //垂直方向GRAM起始地址
	LCD_WriteReg(R83, 0x013F); //垂直方向GRAM结束地址	
}
/*****************************************************************************
** 函数名称: LCD_Clear
** 功能描述: 清屏幕函数
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint32_t index=0;      
	LCD_SetCursor(0,0); //设置光标位置 
	LCD_WriteRAM_Prepare();     //开始写入GRAM	 	  
	for(index=0;index<76800;index++)
	{
		Write_Dat(Color);    
	}
}
/*****************************************************************************
** 函数名称: WriteString
** 功能描述: 在指定位置开始显示一个字符串和一串汉字
				支持自动换行
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/   			   
void WriteString(uint16_t x0, uint16_t y0,uint8_t *pcStr, uint16_t color)
{
	uint16_t usIndex;
    uint16_t usWidth = 0;
    FNT_GB16 *ptGb16 = 0;
    
    ptGb16 = (FNT_GB16 *)GBHZ_16;  
	while(1)
    {
        if(*pcStr == 0) 
		{
            break;                                     /* 字符串结束            */
        }      
        x0 = x0 + (usWidth);                           /* 调节字符串显示松紧度         */
        if(*pcStr > 0x80)                              /* 判断为汉字                   */
        {
		    if((x0 + 16) > LCD_W)                      /* 检查剩余空间是否足够         */
            {
			    x0 = 0;
                y0 = y0 + 16;                          /* 改变显示坐标                 */
                if(y0 > LCD_H)                         /* 纵坐标超出                   */
                {
				    y0 = 0;
                }
            }
            usIndex = findHzIndex(pcStr);
            usWidth = WriteOneHzChar((uint8_t *)&(ptGb16[usIndex].Msk[0]), x0, y0, color);
                                                       /* 显示字符                     */
            pcStr += 2;
        }
		else 
		{                                               /* 判断为非汉字 */
            if (*pcStr == '\r')                         /* 换行 */
            { 
			    y0 = y0 + 16;                           /* 改变显示坐标 */
                if(y0 > LCD_H)                          /* 纵坐标超出 */
                {
				    y0 = 0;
                }
                pcStr++;
                usWidth = 0;
                continue;
            } 
			else if (*pcStr == '\n')                    /* 对齐到起点 */
            {
			    x0 = 0;
                pcStr++;
                usWidth = 0;
                continue;
            } 
			else 
			{
                if((x0 + 8) > LCD_W)                     /* 检查剩余空间是否足够 */
                {
				    x0 = 0;
                    y0 = y0 + 16;                        /* 改变显示坐标 */
                    if(y0 > LCD_H)                       /* 纵坐标超出 */
                    { 
					    y0 = 0;
                    }
                }
                usWidth = WriteOneASCII((uint8_t *)&ASCII_1608_CU[(*pcStr - 0x20)][0], x0, y0, color);
                                                         /* ASCII码表21H的值对应区位码3区*/
                pcStr += 1;
            }
		}
	}												  	  
}
/*****************************************************************************
** 函数名称: WriteOneHzChar
** 功能描述: 显示一个指定大小的汉字
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
uint16_t WriteOneHzChar(uint8_t *pucMsk,
                               uint16_t x0,
                               uint16_t y0,
                               uint16_t color)
{
    uint16_t i,j;
    uint16_t mod[16];                                      /* 当前字模 */
    uint16_t *pusMsk;                                      /* 当前字库地址 */
    uint16_t y;
    
    pusMsk = (uint16_t *)pucMsk;
    for(i=0; i<16; i++)                                    /* 保存当前汉字点阵式字模 */
    {
        mod[i] = *pusMsk++;                                /* 取得当前字模，半字对齐访问*/
        mod[i] = ((mod[i] & 0xff00) >> 8) | ((mod[i] & 0x00ff) << 8);/* 字模交换高低字节（为了显示   */
                                                           /* 需要） */
    }
    y = y0;
    for(i=0; i<16; i++)                                    /* 16行 */
    { 
	    #ifdef __DISPLAY_BUFFER                            /* 使用显存显示  */
        for(j=0; j<16; j++)                                /* 16列 */
        {
		    if((mod[i] << j)& 0x8000)                      /* 显示字模 */
            {
			    DispBuf[240*(y0+i) + x0+j] = color;
            }
        }
        #else                                              /* 直接显示 */

 	if(DeviceCode==0x8999||DeviceCode==0x9919)
	{
		LCD_SetCursor(x0,y);
	}
	else
	{
		#if   ID_AM==000               
		        LCD_SetCursor(x0, y);                              /* 设置写数据地址指针 */
		
		#elif ID_AM==001 
		        LCD_SetCursor(y, x0);                              /* 设置写数据地址指针 */
		#endif
	}
		LCD_WriteRAM_Prepare();        					   /*开始写入GRAM	*/   
        for(j=0; j<16; j++)                                /* 16列                         */
        {
		    if((mod[i] << j) & 0x8000)                     /* 显示字模  */
            { 
			    Write_Dat(color);
            } 
			else 
			{
                Write_Dat(BACK_COLOR);                     /* 用读方式跳过写空白点的像素*/
            	//LCD_ReadDat();
			}
        }
        y++;
        #endif
    }
    return (16);                                          /* 返回16位列宽 */
}
/*****************************************************************************
** 函数名称: WriteOneASCII
** 功能描述: 显示一个指定大小的字符
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
uint16_t WriteOneASCII(uint8_t *pucMsk,
                              uint16_t x0,
                              uint16_t y0,
                              uint16_t color)
{
    uint16_t i,j;
    uint16_t y;
    uint8_t ucChar;
    
    y = y0;
    for(i=0; i<16; i++) 						    /* 16行 */
	{                               
        ucChar = *pucMsk++;
 		if(DeviceCode==0x8999||DeviceCode==0x9919)
		{
			LCD_SetCursor(x0,y);  
		}
		else
		{
			#if   ID_AM==000               
			        LCD_SetCursor(x0, y);                              /* 设置写数据地址指针 */
			
			#elif ID_AM==001 
			        LCD_SetCursor(y, x0);                              /* 设置写数据地址指针 */
			#endif
		}
		LCD_WriteRAM_Prepare();        					  			  /* 开始写入GRAM */
	    for(j=0; j<8; j++) 								    		  /* 8列 */
		{                             
	    	if((ucChar<<j)&0x80)Write_Dat(color);     	  			  /* 显示字模 */      
	        else Write_Dat(BACK_COLOR);   
	    }
	    y++;
  	}
    return (8);                                           			  /* 返回16位列宽 */
}
/*****************************************************************************
** 函数名称: Num_power
** 功能描述: M的N次方
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
uint32_t Num_power(uint8_t m,uint8_t n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}
/*****************************************************************************
** 函数名称: LCD_ShowNum
** 功能描述: 在指定位置显示一串数字
				num:数值(0~4294967295);
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/	 
void LCD_ShowNum(uint8_t x,uint16_t y,u32 num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/Num_power(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
}
/*****************************************************************************
** 函数名称: LCD_WriteBMP
** 功能描述: 在指定的位置显示一幅图片
				Xpos和Ypos为图片显示地址，Height和Width 为图片的宽度和高度
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_WriteBMP(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width, uint8_t *bitmap)
{
  	uint32_t index;
  	uint32_t size = Height * Width;
  	uint16_t *bitmap_ptr = (uint16_t *)bitmap;

  	LCD_SetDisplayWindow(Xpos, Ypos, Xpos+Width-1, Ypos+Height-1);

  	LCD_WriteRAM_Prepare();

  	for(index = 0; index < size; index++)
  	{
    	Write_Dat(*bitmap_ptr++);
  	}
	//恢复窗体大小	 
 	if(DeviceCode==0x8999||DeviceCode==0x9919)
	{
	  	LCD_WriteReg(0x44, 0|239<<8);	 //水平方向GRAM起始地址(低字节为起始地址，高字节为结束地址)
	  	LCD_WriteReg(0x45, 0);		  	//垂直方向GRAM起始地址
	  	LCD_WriteReg(0x46, 319);  	//垂直方向GRAM结束地址
	}
	else
	{
		LCD_WriteReg(R80,0);           //水平方向GRAM起始地址
		LCD_WriteReg(R81,239);		   //水平方向GRAM结束地址
		LCD_WriteReg(R82,0);           //垂直方向GRAM起始地址
		LCD_WriteReg(R83,319);        //垂直方向GRAM结束地址	
	}
}
/*****************************************************************************
** 函数名称: LCD_DrawLine
** 功能描述: 在指定的两侧位置画一条线
				x1,y1:起点坐标度  x2,y2:终点坐标 
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; 				//计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; 		//设置单步方向 
	else if(delta_x==0)incx=0;	//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;	//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )	//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
/*****************************************************************************
** 函数名称: LCD_DrawLine
** 功能描述: 在指定位置画一个指定大小的圆
				(x,y):中心点 	 r    :半径
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void Draw_Circle(uint8_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 
/*****************************************************************************
** 函数名称: LCD_Fill
** 功能描述: 在指定区域内填充指定颜色
				区域大小:  (xend-xsta)*(yend-ysta) 
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_Fill(uint8_t xsta,uint16_t ysta,uint8_t xend,uint16_t yend,uint16_t color)
{                    
    uint32_t n;
	//设置窗口										
	LCD_WriteReg(R80, xsta); //水平方向GRAM起始地址
	LCD_WriteReg(R81, xend); //水平方向GRAM结束地址
	LCD_WriteReg(R82, ysta); //垂直方向GRAM起始地址
	LCD_WriteReg(R83, yend); //垂直方向GRAM结束地址	
	LCD_SetCursor(xsta,ysta);//设置光标位置  
	LCD_WriteRAM_Prepare();  //开始写入GRAM	 	   	   
	n=(u32)(yend-ysta+1)*(xend-xsta+1);    
	while(n--){Write_Dat(color);}//显示所填充的颜色. 
	//恢复设置
	LCD_WriteReg(R80, 0x0000); //水平方向GRAM起始地址
	LCD_WriteReg(R81, 0x00EF); //水平方向GRAM结束地址
	LCD_WriteReg(R82, 0x0000); //垂直方向GRAM起始地址
	LCD_WriteReg(R83, 0x013F); //垂直方向GRAM结束地址	    
}
/*****************************************************************************
** 函数名称: LCD_Delay
** 功能描述: 用于LCD驱动延时
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void LCD_Delay (uint32_t nCount)
{
	__IO uint16_t i;	 	
	for (i=0;i<nCount*100;i++);
}
/*********************************************************************************************************
** End of File
*********************************************************************************************************/
