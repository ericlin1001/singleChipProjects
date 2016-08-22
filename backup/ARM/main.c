/* Includes ------------------------------------------------------------------*/
#include "initConfig.h"
#include "entry.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void GPIO_initConfig(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);  //¹Ø±Õµ÷ÊÔ ¶Ë¿ÚÖØÐÂÓ³Éä  Ê¹ÓÃ·ÂÕæÆ÷µ÷ÊÔÊ±£¬²»ÄÜÓÃ´ËÓï

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All; //select all pin
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //output push-pull 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //I/O speed =50Hz
   GPIO_Init(GPIOA, &GPIO_InitStructure);  //set GPIOA
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All ;  //select all pin
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //input floating 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //I/O speed =50Hz
  GPIO_Init(GPIOB, &GPIO_InitStructure);  //set GPIOB
}

int main(void)
{
#ifdef DEBUG
  debug();
#endif
  Basic_initConfig();
  GPIO_initConfig();
  return entry();
}

#ifdef  DEBUG
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
