#ifndef __TEXT_H__
#define __TEXT_H__	 
					    	    
void Get_HzMat(unsigned char *code,unsigned char *mat,uint8_t size);//得到汉字的点阵码
void Show_Font(uint8_t x,uint8_t y,uint8_t *font,uint8_t size,uint8_t mode);//在指定位置显示一个汉字
void Show_Str(uint8_t x,uint8_t y,uint8_t*str,uint8_t size,uint8_t mode);//在指定位置显示一个字符串 
void Show_Str_Mid(uint8_t x,uint16_t y,uint8_t*str,uint8_t size,uint8_t len);
void Show_PTStr(uint8_t x,uint8_t y,uint8_t*str,uint8_t size,uint8_t mode,uint8_t len,uint16_t offx);//显示部分字符
void Str_Scroll(uint8_t x,uint8_t y,uint8_t*str,uint8_t size,uint8_t mode,uint8_t len,uint8_t start);//滚动字符串
void my_stradd(uint8_t*str1,uint8_t*str2);//将str2与str1相加,结果保存在str1


#endif
