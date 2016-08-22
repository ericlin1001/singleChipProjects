/**************************************************************
** 	火牛开发板
**	LCD驱动代码
**  版本：V1.0  
**	论坛：www.openmcu.com
**	淘宝：http://shop36995246.taobao.com/   
**  技术支持群：121939788 
***************************************************************/
#include "stm32f10x.h"
#include "hardware_conf.h"
#include "ov7670.h"	
#include "sccb.h"

/*******************************************************************************
* 函数名称 : void EXTI_Configuration(void)
* 函数描述: 中断线配置
* 输入参数：无
* 输出参数：无
* 返回值：无
*******************************************************************************/
void EXTI_Configuration(void)
{
  	EXTI_InitTypeDef EXTI_InitStructure;
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
  	/* EXTI Line15 下降沿产生中断 */
  	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
  	EXTI_GenerateSWInterrupt(GPIO_PinSource12);
}
/*****************************************************************************
** 函数名称: Touch_Configuration
** 功能描述: 触摸屏IO配置
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void Camera_Configuration(void)
{
  	EXTI_Configuration();		//摄像头帧频中断引脚配置
  	FIFO_GPIO_Configuration();  //AL422缓存管脚配置
}
/*****************************************************
函数名：OV7670_GPIO_Init
功能：OV7670GPIO初始化
输入：无
输出：无
返回值：无
*****************************************************/
void OV7670_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  OV7670_VSYNC_BIT; //VSYNC
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*****************************************************
函数名：FIFO_GPIO_Configuration
功能：FIFO_GPIO初始化
输入：无
输出：无
返回值：无
*****************************************************/
void FIFO_GPIO_Configuration(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
                                | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2
                                |GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; //DATA
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*****************************************************
函数名：XCLK_init_ON
功能：提供7660时钟
输入：无
输出：无
返回值：无
*****************************************************/
void XCLK_init_ON(void)
{
  
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = OV7670_XCLK_BIT; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    RCC_MCOConfig(RCC_MCO_HSE);		//是能外部晶振作为OV7670时钟
}
/*****************************************************
函数名：XCLK_init_OFF
功能：关闭7660时钟
输入：无
输出：无
返回值：无
*****************************************************/
void XCLK_init_OFF(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = OV7670_XCLK_BIT; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//功能：写OV7660寄存器
//返回：1-成功	0-失败
u8 wrOV7670Reg(u8 regID, u8 regDat)
{
	startSCCB();
	if(SCCBwriteByte(0x42)==0)
	{
		stopSCCB();
		return(0);
	}
	DelaySCCB(100);	
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	DelaySCCB(100);
  	if(0==SCCBwriteByte(regDat))
	{
		stopSCCB();
		return(0);
	}
  	stopSCCB();
	
  	return(1);
}
////////////////////////////
//功能：读OV7660寄存器
//返回：1-成功	0-失败
u8 rdOV7660Reg(u8 regID, u8 *regDat)
{
	//通过写操作设置寄存器地址
	startSCCB();
	if(0==SCCBwriteByte(0x42))
	{
		stopSCCB();
		return(0);
	}
	DelaySCCB(100);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	stopSCCB();
	
	DelaySCCB(100);
	
	//设置寄存器地址后，才是读
	startSCCB();
	if(0==SCCBwriteByte(0x43))
	{
		stopSCCB();
		return(0);
	}
	DelaySCCB(100);
  	*regDat=SCCBreadByte();
  	noAck();
  	stopSCCB();
  	return(1);
}


//(140,16,640,480) is good for VGA
//(272,16,320,240) is good for QVGA
/* config_OV7660_window */
void OV7660_config_window(u16 startx,u16 starty,u16 width, u16 height)
{
	u16 endx=(startx+width);
	u16 endy=(starty+height*4);// "v*2"必须
	u8 temp_reg1, temp_reg2;
	u8 state,temp;
	
	state = rdOV7660Reg(0x03, &temp_reg1 );
	temp_reg1 &= 0xC0;
	state = rdOV7660Reg(0x32, &temp_reg2 );
	temp_reg2 &= 0xC0;
	
	// Horizontal
	temp = temp_reg2|((endx&0x7)<<3)|(startx&0x7);
	state = wrOV7670Reg(0x32, temp );
	temp = (startx&0x7F8)>>3;
	state = wrOV7670Reg(0x17, temp );
	temp = (endx&0x7F8)>>3;
	state = wrOV7670Reg(0x18, temp );
	
	// Vertical
	temp = temp_reg1|((endy&0x7)<<3)|(starty&0x7);
	state = wrOV7670Reg(0x03, temp );
	temp = (starty&0x7F8)>>3;
	state = wrOV7670Reg(0x19, temp );
	temp = (endy&0x7F8)>>3;
	state = wrOV7670Reg(0x1A, temp );
	if(state==0){}
}

void my_delay_ms(u16 time)//delay some time
{
	u16 i;
	for(;time!=0;time--)
	{
		for(i=0;i<10000;i++);
	}
}
/*****************************************************
函数名：OV7660_init
功能：OV7670初始化
输入：无
输出：无
返回值：返回1成功，返回0失败
*****************************************************/
u8 OV7660_init(void)
{
	u8  i , temp;       
	u8 ovidmsb=0;
	u8 ovidlsb=0;
	SCCB_GPIO_Config();//SCCB总线端口配置
	OV7670_GPIO_Init();
	XCLK_init_ON();	   //给OV7670提供时钟
	temp=0x80;		 //复位值
	if(0==wrOV7670Reg(0x12, temp)) //Reset SCCB
	{
	  return 0;
	}
	rdOV7660Reg(0x12, &ovidmsb);	   //这一步要加上,不加的话返回值好像老为0

	rdOV7660Reg(0x0a, &ovidmsb);	  //读产品高位序列号
	if(ovidmsb !=0x76)
	{
		return 0;
	} 
    //rdOV7660Reg(0x12, &ovidlsb);	  //这一步要加上,不加的话返回值好像老为0

	rdOV7660Reg(0x0b, &ovidlsb );		  //读产品低位识别号
	if(ovidlsb !=0x73)
	{
		return 0;
	} 
	for(i=0;i<CHANGE_REG_NUM;i++)
	{
		if( 0 == wrOV7670Reg(change_reg[i][0],change_reg[i][1]))
		{
			return 0;
		}
	}
	my_delay_ms(200);	
	
	return 0x01; //ok
}
void set_qvga(void)   
{   
    while(1!= wrOV7670Reg(0x12, 0x14));
   // wrOV7670Reg(0x40, 0xd0);     
    wrOV7670Reg(0x17, 0x16);   
    wrOV7670Reg(0x18, 0x04);   
    wrOV7670Reg(0x19, 0x02);   
    wrOV7670Reg(0x1A, 0x7B); 
    wrOV7670Reg(0x32, 0x40);
     
    wrOV7670Reg(0x03, 0x0a); 
  //  wrOV7670Reg(0x3E, 0x1A); 
    
    wrOV7670Reg(0x70, 0x3A); 
    wrOV7670Reg(0x71, 0x35);  
    wrOV7670Reg(0x72, 0x11);   
    wrOV7670Reg(0x73, 0xF9); 
    wrOV7670Reg(0xA2, 0x02);
    
}
/**************************************************************************************
* 名    称: void Delay(u16 time) 
* 功    能: 短暂延时函数
* 参    数: 无
* 返 回 值: 无
*
* 修改历史:
*   版本    日期     作者     改动内容和原因
*   ----------------------------------------------------
*   1.0   2009.3.23 
**************************************************************************************/
void DelayNS(u16 time)                
{
    while(time--!=0); 
}
