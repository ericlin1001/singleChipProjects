/**************************************************************
** 	火牛开发板
**	功能介绍： ADC转换代码
**  版本：V1.0  
**	论坛：www.openmcu.com
**	淘宝：http://shop36995246.taobao.com/   
**  技术支持群：121939788 
***************************************************************/ 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "adc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)	//ADC1的数据寄存器地址

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
vu16 ADCConvertedValue;

uint8_t AsciiBuff[5];
/* Private function prototypes -----------------------------------------------*/
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
/*****************************************************************************
** 函数名称: adc_Init
** 功能描述: ADC初始化
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void adc_Init()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 使能 MDA 重要！！！
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);// 使能 ADC1 重要！！！

	/*DMA1 channel1 configuration ----------------------------------------------*/
  	DMA_DeInit(DMA1_Channel1);			   //将DMA的通道1寄存器重设为缺省值
  	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	  //定义DMA外设基地址为ADC1
  	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue; //定义DMA内存基地址
  	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	  //外设作为数据传输的来源
  	DMA_InitStructure.DMA_BufferSize = 1;	  //定义指定DMA通道的DMA缓存的大小
  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;		//内存地址寄存器不变
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//数据宽度为16位
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//数据宽度为16位
  	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	  //工作在循环缓存模式
  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMA通道1拥有高优先级
  	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//DMA通道1没有设置为内存到内存传输
  	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	 
  
  	/* Enable DMA1 channel1 */
  	DMA_Cmd(DMA1_Channel1, ENABLE);			 
     
  	/* ADC1 configuration ------------------------------------------------------*/
  	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	 //独立工作模式
  	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		  //扫描模式
  	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	   //连续转换模式
  	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//软件控制转换
  	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	 //右对齐
  	ADC_InitStructure.ADC_NbrOfChannel = 1;
  	ADC_Init(ADC1, &ADC_InitStructure);

  	/* ADC1 regular channel10 configuration */ 
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_55Cycles5); //转换时间为17.1US

  	/* Enable ADC1 DMA */
  	ADC_DMACmd(ADC1, ENABLE);
  
  	/* Enable ADC1 */
  	ADC_Cmd(ADC1, ENABLE);

  	/* Enable ADC1 reset calibaration register */   
  	ADC_ResetCalibration(ADC1);

  	/* Check the end of ADC1 reset calibration register */
  	while(ADC_GetResetCalibrationStatus(ADC1));

  	/* Start ADC1 calibaration */
  	ADC_StartCalibration(ADC1);
  	/* Check the end of ADC1 calibration */
  	while(ADC_GetCalibrationStatus(ADC1));
     
  	/* Start ADC1 Software Conversion */ 
  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
/*****************************************************************************
** 函数名称: HexToASCII
** 功能描述: 将数值转化成字符
				0x30 代表字符0 比如1234 转化为 ”1234“
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void HexToASCII(uint16_t data)
{
	AsciiBuff[0] = data/1000%10 + 0x30;
	AsciiBuff[1] = data/100%10 + 0x30;
	AsciiBuff[2] = data/10%10 + 0x30;
	AsciiBuff[3] = data%10 + 0x30;
	AsciiBuff[4] = 0;
}
/*****************************************************************************
** 函数名称: HexToASCII
** 功能描述: 将数值转化成字符
				0x30 代表字符0 比如1234 转化为 ”1234“
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
void Temp_Init()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 使能 MDA 重要！！！
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);// 使能 ADC1 重要！！！

  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* Enable DMA1 channel 1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
     
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel 14 configuration */ 
  /* 内部温度传感器 Channel 16 */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_55Cycles5);

  /* Enable the temperature sensor and vref internal channel */
  ADC_TempSensorVrefintCmd(ENABLE);	    //内部温度传感器  添加这一句 

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */   //使用之前一定要校准
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
/*****************************************************************************
** 函数名称: ADC_Filter
** 功能描述: ADC转换采样值
				采样10后区平均值 结果更准确
** 作  　者: Dream
** 日　  期: 2010年12月06日
*****************************************************************************/
uint16_t ADC_Filter(void)
{
	uint16_t result=0;
	uint32_t i,j;
	for(i=10;i>0;i--)
	{
		for(j=0;j<5000;j++); 

		result += ADCConvertedValue;
	}

	return result/10;
}
/*********************************************************************************************************
** End of File
*********************************************************************************************************/
