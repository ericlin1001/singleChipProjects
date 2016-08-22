#ifndef __I2C_H
#define __I2C_H

#define ADDR_24LC02		0xA0
extern uint8_t I2C_Read(uint8_t *buf,uint8_t addr,uint8_t num);

extern uint8_t I2C_Write(uint8_t *buf,uint8_t addr,uint8_t num);

void II2C_Configuration(void);
void II2C_Init(void);

#endif
/*********************************************************************************************************
** End of File
*********************************************************************************************************/
