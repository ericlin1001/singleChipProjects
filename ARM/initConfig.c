#include "initConfig.h"
/*********private decleare*************/
static void RCC_initConfig(void);
static void NVIC_initConfig(void);

/*********implement*************/
void Basic_initConfig(void){
RCC_initConfig();
NVIC_initConfig();
}

void RCC_initConfig(void)
{   
ErrorStatus HSEStartUpStatus;

//reset RCC to default reset value.
  RCC_DeInit();

  //open high speed externl oscillator, HSE(HCLK)=8Hz
  RCC_HSEConfig(RCC_HSE_ON);

   //wait for HSE ready.
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)   
  {								    
    //opne flash prefetch function
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    //flash delay for 2 T
    FLASH_SetLatency(FLASH_Latency_2);
 	
  //AHB(HCLK)=SYSCLK
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  
  
   //APB2(PCLK2)=AHB(HCLK)
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    //APB1(PCLK1)=AHB(HCLK)*1/2
    RCC_PCLK1Config(RCC_HCLK_Div2);

     //PLL=AHB(HCLK)/1*9=8Hz*9=72Hz
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

    //open PLL
    RCC_PLLCmd(ENABLE);

   //wait till PLL ready.
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

  //SYSCLK=PLL=72Hz
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

   //check whether PLL is SYSCLK.
    while(RCC_GetSYSCLKSource() != 0x08);
  }
}

/*******************************************************************************
*                             NVICÅäÖÃº¯Êý
*******************************************************************************/
void NVIC_initConfig(void)
{
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
}
