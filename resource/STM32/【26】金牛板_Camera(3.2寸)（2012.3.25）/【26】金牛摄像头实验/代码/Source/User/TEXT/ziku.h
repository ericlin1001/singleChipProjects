#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
//���ĺ���֧�ֳ��� V1.1					   

#define EN_UPDATE_FONT //ʹ���������,ͨ���ر�����ʵ�ֽ�ֹ�ֿ����

 
extern uint32_t FONT16ADDR ;
extern uint32_t FONT12ADDR ;
extern uint32_t UNI2GBKADDR;

u8 Update_Font(void);//�����ֿ�   
u8 Font_Init(void);//��ʼ���ֿ�
#endif





















