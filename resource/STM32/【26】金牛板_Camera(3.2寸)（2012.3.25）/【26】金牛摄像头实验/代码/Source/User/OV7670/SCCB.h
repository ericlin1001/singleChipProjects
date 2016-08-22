/*********************************************************/
#ifndef _sccb_H
#define _sccb_H
//=============================================ÉãÏñÍ·¶Ë¿Ú
#define	   SCCB_SIC  	PCout(8)
#define	   SCCB_SID  	PCout(9)

#define SCCB_SID_IN      SCCB_SID_GPIO_INPUT()
#define SCCB_SID_OUT     SCCB_SID_GPIO_OUTPUT()

#define SCCB_SID_STATE	 GPIOC->IDR&0x0200
///////////////////////////////////////////
void SCCB_GPIO_Config(void);
void SCCB_SID_GPIO_OUTPUT(void);
void SCCB_SID_GPIO_INPUT(void);
void startSCCB(void);
void stopSCCB(void);
void noAck(void);
unsigned char SCCBwriteByte(unsigned char m_data);
unsigned char SCCBreadByte(void);
void DelaySCCB(unsigned int i);


#endif /* _IIC_H */


