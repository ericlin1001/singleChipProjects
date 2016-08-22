#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
//中文汉字支持程序 V1.1					   

#define EN_UPDATE_FONT //使能字体更新,通过关闭这里实现禁止字库更新

 
extern uint32_t FONT16ADDR ;
extern uint32_t FONT12ADDR ;
extern uint32_t UNI2GBKADDR;

u8 Update_Font(void);//更新字库   
u8 Font_Init(void);//初始化字库
#endif





















