#ifndef __TEXT_H__
#define __TEXT_H__	 
					    	    
void Get_HzMat(unsigned char *code,unsigned char *mat,uint8_t size);//�õ����ֵĵ�����
void Show_Font(uint8_t x,uint8_t y,uint8_t *font,uint8_t size,uint8_t mode);//��ָ��λ����ʾһ������
void Show_Str(uint8_t x,uint8_t y,uint8_t*str,uint8_t size,uint8_t mode);//��ָ��λ����ʾһ���ַ��� 
void Show_Str_Mid(uint8_t x,uint16_t y,uint8_t*str,uint8_t size,uint8_t len);
void Show_PTStr(uint8_t x,uint8_t y,uint8_t*str,uint8_t size,uint8_t mode,uint8_t len,uint16_t offx);//��ʾ�����ַ�
void Str_Scroll(uint8_t x,uint8_t y,uint8_t*str,uint8_t size,uint8_t mode,uint8_t len,uint8_t start);//�����ַ���
void my_stradd(uint8_t*str1,uint8_t*str2);//��str2��str1���,���������str1


#endif
