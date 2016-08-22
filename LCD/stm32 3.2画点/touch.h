


#define u16   unsigned int
#define u8    unsigned char



#define CS_PIN      GPIO_Pin_12	 //RST ��Ӧ��Ƭ�����Ŷ��� P3^4 / ��ӦSTM32�� PA12
#define DIN_PIN     GPIO_Pin_13	 //RD  ��Ӧ��Ƭ�����Ŷ��� P3^5 / ��ӦSTM32�� PA13
#define DCLK_PIN    GPIO_Pin_14	 //WR  ��Ӧ��Ƭ�����Ŷ��� P3^6 / ��ӦSTM32�� PA14
#define DOUT_PIN    GPIO_Pin_15  //RS  ��Ӧ��Ƭ�����Ŷ��� P3^7 / ��ӦSTM32�� PA15
#define BUSY_PIN    GPIO_Pin_6	 //CS  ��Ӧ��Ƭ�����Ŷ��� P2^1 / ��ӦSTM32�� PB6
#define IRQ_PIN     GPIO_Pin_7	 //CS  ��Ӧ��Ƭ�����Ŷ��� P2^0 / ��ӦSTM32�� PB7

#define    CS(x)     x ? GPIO_SetBits(GPIOA,    CS_PIN):   GPIO_ResetBits(GPIOA, CS_PIN) //�Լ�����λ��������
#define    DIN(x)    x ? GPIO_SetBits(GPIOA,  DIN_PIN) :   GPIO_ResetBits(GPIOA, DIN_PIN)
#define    DCLK(x)   x ? GPIO_SetBits(GPIOA, DCLK_PIN) :   GPIO_ResetBits(GPIOA, DCLK_PIN)
#define    DOUT(x)   x ? GPIO_SetBits(GPIOA, DOUT_PIN) :   GPIO_ResetBits(GPIOA, DOUT_PIN)
#define    BUSY(x)   x ? GPIO_SetBits(GPIOB, BUSY_PIN) :   GPIO_ResetBits(GPIOB, BUSY_PIN)
#define    IRQ(x)    x ? GPIO_SetBits(GPIOB,  IRQ_PIN) :   GPIO_ResetBits(GPIOB, IRQ_PIN)


// A/D ͨ��ѡ�������ֺ͹����Ĵ���
#define	CMD_RDX 	0xD0
#define	CMD_RDY 	0x90
#define READ_TIMES 12 //��ȡ����
#define LOST_VAL    2    //����ֵ
#define ERR_RANGE 50 //��Χ 

#define LCDWIDTH		240
#define LCDHEIGHT		400




 void Delay(vu32 nCount);
 extern u16 AD7843(u8 CMD) ;
 void inttostr(int dd, char *str);
 unsigned int ReadFromCharFrom7843(void);
 void WriteCharTo7843(unsigned char num) ;
 void spistar(void);



//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//unsigned int ADS_Read_AD(u8 CMD);
unsigned int ADS_Read_XY(u8 xy);//
unsigned char Read_ADS(u16 *x,u16 *y);////��Сֵ��������100.
unsigned char Read_ADS2(u16 *x,u16 *y);//2�ζ�ȡADS7846,������ȡ2����Ч��ADֵ,�������ε�ƫ��ܳ���ERR_RANGE







