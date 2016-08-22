#include "stm32f10x.h"
#include "text.h"
#include "ziku.h"
#include "ili932x_conf.h"
#include "M45PE16.h"

//code �ַ�ָ�뿪ʼ
//���ֿ��в��ҳ���ģ
//code �ַ����Ŀ�ʼ��ַ,ascii��
//mat  ���ݴ�ŵ�ַ size*2 bytes��С

void Get_HzMat(unsigned char *code,unsigned char *mat,uint8_t size)
{
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//�� ���ú���
	{   		    
	    for(i=0;i<(size*2);i++)*mat++=0x00;//�������
	    return; //��������
	}          
	if(ql<0x7f)ql-=0x40;//ע��!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*(size*2);//�õ��ֿ��е��ֽ�ƫ����  		  

	if(size==16)M45PE_Read(mat,foffset+FONT16ADDR,32);
	else M45PE_Read(mat,foffset+FONT12ADDR,24);     
}  
//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:����GBK��
//size:�����С
//mode:0,������ʾ,1,������ʾ	 
//����ԭ��@HYW
//CHECK:09/10/30
void Show_Font(uint8_t x,uint8_t y,uint8_t *font,uint8_t size,uint8_t mode)
{
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t dzk[32];
	uint16_t tempcolor;

	Get_HzMat(font,dzk,size);//�õ���Ӧ��С�ĵ�������
	if(mode==0)//������ʾ
	{	 
	    for(t=0;t<size*2;t++)
	    {   												   
		    temp=dzk[t];//�õ�12����                          
	        for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)LCD_DrawPoint(x,y);
	 			else 
				{
					tempcolor=POINT_COLOR;
					POINT_COLOR=BACK_COLOR;
					LCD_DrawPoint(x,y);
					POINT_COLOR=tempcolor;//��ԭ
				}
				temp<<=1;
				y++;
				if((y-y0)==size)
				{
					y=y0;
					x++;
					break;
				}
			}  	 
    	} 
	}else//������ʾ
	{
	    for(t=0;t<size*2;t++)
	    {   												   
		    temp=dzk[t];//�õ�12����                          
	        for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)LCD_DrawPoint(x,y);   
				temp<<=1;
				y++;
				if((y-y0)==size)
				{
					y=y0;
					x++;
					break;
				}
			}  	 
    	} 
	}    
}
//��ָ��λ�ÿ�ʼ��ʾһ���ַ���	    
//֧���Զ�����
//(x,y):��ʼ����
//str  :�ַ���
//size :�����С
//mode:0,�ǵ��ӷ�ʽ;1,���ӷ�ʽ    
//����ԭ��@HYW
//CHECK:09/10/30			   
void Show_Str(uint8_t x,uint8_t y,uint8_t*str,uint8_t size,uint8_t mode)
{												  	  
    uint8_t bHz=0;     //�ַ���������  	    				    				  	  
    while(*str!=0)//����δ����
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//���� 
	        else              //�ַ�
	        {      
                if(x>(LCD_W-size/2))//����
				{				   
					y+=size;
					x=0;	   
				}							    
		        if(y>(LCD_H-size))break;//Խ�緵��      
		        if(*str==13)//���з���
		        {         
		            y+=size;
					x=0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,mode);//��Ч����д�� 
				str++; 
		        x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
	        }
        }else//���� 
        {     
            bHz=0;//�к��ֿ�    
            if(x>(LCD_W-size))//����
			{	    
				y+=size;
				x=0;		  
			}
	        if(y>(LCD_H-size))break;//Խ�緵��  						     
	        Show_Font(x,y,str,size,mode); //��ʾ�������,������ʾ 
	        str+=2; 
	        x+=size;//��һ������ƫ��	    
        }						 
    }   
}  			 		  
//�õ��ַ����ĳ���
//����ԭ��@HYW
//CHECK OK 091118 
uint16_t my_strlen(uint8_t*str)
{
	uint16_t len=0;
	while(*str!='\0')
	{
		str++;
		len++;
	}
	return len;
}
//��str1�������str2
//����ԭ��@HYW
//CHECK OK 091118 
void my_stradd(uint8_t*str1,uint8_t*str2)
{				  
	while(*str1!='\0')str1++;
	while(*str2!='\0')
	{
		*str1=*str2;
		str2++;
		str1++;
	}
	*str1='\0';//���������  		 
}  
//��ָ����ȵ��м���ʾ�ַ���
//����ַ����ȳ�����len,����Show_Str��ʾ
//len:ָ��Ҫ��ʾ�Ŀ��
//���12*12����!!!
void Show_Str_Mid(uint8_t x,uint16_t y,uint8_t*str,uint8_t size,uint8_t len)
{
	uint16_t strlenth=0;
   	strlenth=my_strlen(str);
	strlenth*=size/2;
	if(strlenth>len)Show_Str(x,y,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	    Show_Str(strlenth+x,y,str,size,1);
	}
}   
