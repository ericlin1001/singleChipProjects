#include "stm32f10x.h"
#include "ziku.h"
#include "ili932x_conf.h"
#include "M45PE16.h"
#include "fat.h"

extern void Delay(__IO uint32_t nTime);

//�������ֶ�һ����Ҫ�Ҹ�!!!
//�ֽ�0~3,  ��¼UNI2GBKADDR;�ֽ�4~7  ,��¼UNI2GBKADDR�Ĵ�С
//�ֽ�8~11, ��¼FONT16ADDR ;�ֽ�12~15,��¼FONT16ADDR�Ĵ�С
//�ֽ�16~19,��¼FONT12ADDR ;�ֽ�20~23,��¼FONT12ADDR�Ĵ�С		    
//�ֽ�24,��������ֿ��Ƿ���ڵı�־λ,0XAA,��ʾ�����ֿ�;����ֵ,��ʾ�ֿⲻ����!

//ϵͳ�ļ���		  
const unsigned char *folder[2]=
{
"SYSTEM",
"FONT",	  
};	 
//ϵͳ�ļ�����
const unsigned char *sysfile[3]=
{			  
"GBK16.FON",
"GBK12.FON",
"UNI2GBK.SYS",	  
};
//�ֽ�0~3,  ��¼UNI2GBKADDR;�ֽ�4~7  ,��¼UNI2GBKADDR�Ĵ�С
//�ֽ�8~11, ��¼FONT16ADDR ;�ֽ�12~15,��¼FONT16ADDR�Ĵ�С
//�ֽ�16~19,��¼FONT12ADDR ;�ֽ�20~23,��¼FONT12ADDR�Ĵ�С
//������,��100K����ʼ
//100K,���UNICODE2GBK��ת����	

uint32_t FONT16ADDR ;//16�����ŵĵ�ַ
uint32_t FONT12ADDR ;//12�����ŵĵ�ַ
uint32_t UNI2GBKADDR;//UNICODE TO GBK ���ŵ�ַ	  	 

//���������ļ�
//����ֵ:0,���³ɹ�;
//		 ����,�������.
//����ԭ��@ALIENTEK
//V1.1
#ifdef EN_UPDATE_FONT
static uint8_t temp[512];  //��ʱ�ռ�
uint8_t Update_Font(void)
{
	uint32_t fcluster=0;	   
	uint32_t i;
	//uint8_t temp[512];  //��ʱ�ռ�	 �����ﶨ��,���ڴ����
	uint32_t tempsys[2];  //��ʱ��¼�ļ���ʼλ�ú��ļ���С
	float prog;
	uint8_t t;		 
	FileInfoStruct FileTemp;//��ʱ�ļ���   				    	 
	//�õ���Ŀ¼�Ĵغ�
	if(FAT32_Enable)fcluster=FirstDirClust;
	else fcluster=0;			   
	FileTemp=F_Search(fcluster,(unsigned char *)folder[0],T_FILE);//����system�ļ���	  
	if(FileTemp.F_StartCluster==0)return 1;						  //ϵͳ�ļ��ж�ʧ			  
	{	 										 
		//�Ȳ�������
		FileTemp=F_Search(FileTemp.F_StartCluster,(unsigned char *)folder[1],T_FILE);//��system�ļ����²���FONT�ļ���
		if(FileTemp.F_StartCluster==0)return 2;//�����ļ��ж�ʧ	

		fcluster=FileTemp.F_StartCluster;      //�����ļ��дغ�	    
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[2],T_SYS);//��system�ļ����²���SYS�ļ�
		if(FileTemp.F_StartCluster==0)return 3;//FONT12�����ļ���ʧ	 
		LCD_ShowString(20,90,"Write UNI2GBK to FLASH...");		
		LCD_ShowString(108,110,"%");		
		F_Open(&FileTemp);//�򿪸��ļ�
		i=0;	  
		while(F_Read(&FileTemp,temp))//�ɹ�����512���ֽ�
		{		 
			if(i<FileTemp.F_Size)//�������ļ���С
			{
				M45PE_Write(temp,i+100000,512);//��100K�ֽڴ���ʼд��512������   
				i+=512;//����512���ֽ�
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(84,110,t,3,16);//��ʾ��ֵ
			}					    
		}
		UNI2GBKADDR=100000;//UNI2GBKADDR��100K����ʼд��.
		tempsys[0]=UNI2GBKADDR;
		tempsys[1]=FileTemp.F_Size;	 	  //UNI2GBKADDR ��С
		M45PE_Write((uint8_t*)tempsys,0,8);//��¼�ڵ�ַ0~7��

		Delay(1000);			    
		//printf("UNI2GBKд��FLASH���!\n");
		//printf("д�����ݳ���:%d\n",FileTemp.F_Size);
		//printf("UNI2GBKSADDR:%d\n\n",UNI2GBKADDR);
		
		FONT16ADDR=FileTemp.F_Size+UNI2GBKADDR;//F16���׵�ַ 
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[0],T_FON);//��system�ļ����²���FONT16�����ļ�
		if(FileTemp.F_StartCluster==0)return 4;//FONT16�����ļ���ʧ	 
	
		LCD_ShowString(20,90,"Write FONT16 to FLASH... ");		
 		//printf("��ʼFONT16д��FLASH...\n");		
		F_Open(&FileTemp);//�򿪸��ļ�
		i=0;	  
		while(F_Read(&FileTemp,temp))//�ɹ�����512���ֽ�
		{
			if(i<FileTemp.F_Size)//�������ļ���С
			{
				M45PE_Write(temp,i+FONT16ADDR,512);//��0��ʼд��512������   
				i+=512;//����512���ֽ�
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(84,110,t,3,16);//��ʾ��ֵ
			}
		}												   
		tempsys[0]=FONT16ADDR;
		tempsys[1]=FileTemp.F_Size;	 	  //FONT16ADDR ��С
		M45PE_Write((uint8_t*)tempsys,8,8);//��¼�ڵ�ַ8~15��
		
		Delay(1000);	    
		//printf("FONT16д��FLASH���!\n");
		//printf("д�����ݳ���:%d\n",FileTemp.F_Size);


		FONT12ADDR=FileTemp.F_Size+FONT16ADDR;//F16���׵�ַ
		//printf("FONT16SADDR:%d\n\n",FONT16ADDR);
		//LCD_ShowString(20,60,"Write FONT12 to FLASH... ");		
		//FONT12��ʱ������
		/*			  
		FileTemp=F_Search(fcluster,(unsigned char *)sysfile[1],T_FON);//��system�ļ����²���FONT12�����ļ�
		if(FileTemp.F_StartCluster==0)return 5;//FONT12�����ļ���ʧ	 
		printf("��ʼFONT12д��FLASH...\n");		
		F_Open(&FileTemp);//�򿪸��ļ�
		i=0;	  
		while(F_Read(&FileTemp,temp))//�ɹ�����512���ֽ�
		{
			if(i<FileTemp.F_Size)//�������ļ���С
			{
				M45PE_Write(temp,i+FONT12ADDR,512);//��0��ʼд��512������   
				i+=512;//����512���ֽ�
			}
			prog=(float)i/FileTemp.F_Size;
			prog*=100;
			if(t!=prog)
			{
				t=prog;
				if(t>100)t=100;
				LCD_ShowNum(84,80,t,3,16);//��ʾ��ֵ
			}
		}	    
		tempsys[0]=FONT12ADDR;
		tempsys[1]=FileTemp.F_Size;	 	  //FONT16ADDR ��С
		M45PE_Write((uint8_t*)tempsys,16,8);//��¼�ڵ�ַ16~23��

		printf("FONT12д��FLASH���!\n");
		printf("д�����ݳ���:%d\n",FileTemp.F_Size);   
		printf("FONT12SADDR:%d\n\n",FONT12ADDR); */
	}
	t=0XAA;
   	M45PE_Write(&t,24,1);//д���ֿ���ڱ�־	0XAA
	LCD_ShowString(20,90,"  Font Update Successed  ");		    
	Delay(1000);		
	Delay(1000);		
	return 0;//�ɹ�
}
#endif

//����������õ������ַ
//��ϵͳʹ�ú���֧��֮ǰ,������øú���
//����FONT16ADDR,FONT12ADDR,UNI2GBKADDR;  
uint8_t Font_Init(void)
{
	uint32_t tempsys[2];//��ʱ��¼�ļ���ʼλ�ú��ļ���С
	uint8_t fontok=0;
	M45PE_Read(&fontok,24,1);//�����ֿ��־λ,���Ƿ�����ֿ�
	if(fontok!=0XAA)return 1;
	M45PE_Read((uint8_t*)tempsys,0,8);//����8���ֽ�   
	UNI2GBKADDR=tempsys[0];					  
	//printf("tempsysgbk[0]:%d\n",tempsys[0]);
	//printf("tempsysgbk[1]:%d\n",tempsys[1]);

	M45PE_Read((uint8_t*)tempsys,8,8);//����8���ֽ�   
	//printf("tempsysf16[0]:%d\n",tempsys[0]);
	//printf("tempsysf16[1]:%d\n",tempsys[1]);
	FONT16ADDR=tempsys[0];
	
	M45PE_Read((uint8_t*)tempsys,16,8);//����8���ֽ�   
	//printf("tempsysf12[0]:%d\n",tempsys[0]);
	//printf("tempsysf12[1]:%d\n",tempsys[1]);
	FONT12ADDR=tempsys[0];
	return 0;	 
}
