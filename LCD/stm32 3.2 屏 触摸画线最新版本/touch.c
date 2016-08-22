
#include "stm32f10x_lib.h"
#include "touch.h"

extern GPIO_InitTypeDef GPIO_InitStructure;

//**********************************************************
void spistar()                                     //SPI开始
{
	DCLK(0);
	Delay(100);
	CS(1);
	Delay(150); 
	DIN(1);
	Delay(100);
	DCLK(1);
	Delay(200);
	CS(0);
	Delay(150); 

}
//**********************************************************
void WriteCharTo7843(unsigned char num)          //SPI写数据
{
	unsigned char count=0;
	DCLK(0);
	for(count=0;count<8;count++)
	{
	   if((num<<count)&0x80)DIN(1);   /*判断发送位*/
       else  DIN(0);   
	DCLK(0); Delay(150);                //上升沿有效
	DCLK(1); Delay(150);
	}
}
//**********************************************************
unsigned int ReadFromCharFrom7843()             //SPI 读数据
{
	unsigned char count=0;
	unsigned int Num=0;
	
	for(count=0;count<12;count++)
	{
	Num<<=1;
	DCLK(1); Delay(150);                //下降沿有效
	DCLK(0); Delay(150);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ; // 选择所有脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //输出的最大频率为50HZ
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)) Num++;
	}
	return(Num);
}


void inttostr(int dd, unsigned char *str)
{
	str[0]=dd/10000+48;	    //个位  48的意思是，触摸IC是12位的AD， 加48是向高位移4位，取高12位的数据
	str[1]=(dd/1000)-((dd/10000)*10)+48; //十位
	str[2]=(dd/100)-((dd/1000)*10)+48;	//百位
	str[3]=(dd/10)-((dd/100)*10)+48; //千位
	str[4]=dd-((dd/10)*10)+48; //万位
	str[5]=0;
}
u16 AD7843(u8 CMD)              //外部中断0 用来接受键盘发来的数据
{
	u16 AD_Data;
	//delayms(1);                     //中断后延时以消除抖动，使得采样数据更准确
	spistar();                       //启动SPI
	//while(BUSY);                //如果BUSY信号不好使可以删除不用
	//delayms(1);
	WriteCharTo7843(CMD);        //送控制字 10010000 即用差分方式读X坐标 详细请见有关资料
	//while(BUSY);               //如果BUSY信号不好使可以删除不用
	//delayms(1);
	DCLK(1); Delay(200);
	DCLK(0); Delay(200);
	AD_Data =ReadFromCharFrom7843();
	//WriteCharTo7843(0xD0);       //送控制字 11010000 即用差分方式读Y坐标 详细请见有关资料
	//DCLK=1; _nop_();_nop_();_nop_();_nop_();
	//DCLK=0; _nop_();_nop_();_nop_();_nop_();
	//TP_X=ReadFromCharFrom7843();
		Delay(150); 
	Delay(150); 
	CS(1);

	return  AD_Data	;
}


//读取一个坐标值
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 

unsigned int ADS_Read_XY(u8 xy)
{
	u16  i, j;
	u16  buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
	  	buf[i]=AD7843(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 


//2次读取ADS7846,连续读取2次有效的AD值,且这两次的偏差不能超过
//50,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度

unsigned char Read_ADS2(u16 *x,u16 *y) 
{
    u16  x1,y1;
    u16  x2,y2;
    u8  flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 


//带滤波的坐标读取
//最小值不能少于100.
unsigned char Read_ADS(u16 *x,u16 *y)
{
	u16  xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	  												   
	if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}
