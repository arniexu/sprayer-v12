#ifndef __MYIIC_H
#define __MYIIC_H

#include "hardware.h"
#include "MS51_16K.h"

/******************************************************************************************/
/* IIC1引脚 定义*/
// tm1650 2 scl <-----------------> mcu 16 p0.0 <--------> devboard B4
// tm1650 3 sda <-----------------> mcu 20 p0.4 <--------> devboard B0


/* i2c is driven by i2c module on devboard but driven by GPIO on production board */
sbit TM1650_SDA = P0^4;
sbit TM1650_SCL = P0^0;

/* IIC所有操作函数 */
void IIC_Init(void);            /* 初始化IIC的IO口 */
void IIC_Start(void);           /* 发送IIC开始信号 */
void IIC_Stop(void);            /* 发送IIC停止信号 */
void IIC_Ack(void);             /* IIC发送ACK信号 */
void IIC_Nack(void);            /* IIC不发送ACK信号 */
uint8_t IIC_Wait_Ack(void);     /* IIC等待ACK信号 */
void IIC_Send_Byte(uint8_t txd);						/* IIC发送一个字节 */
uint8_t IIC_Receive_Byte(unsigned char ack);/* IIC读取一个字节 */


void I2C_Write_Byte(uint8_t u8Addr, uint8_t u8DAT);
void I2C_Write_Word(uint8_t u8Addr, uint8_t u8DAT[], uint8_t siz);
uint8_t I2C_Read_Byte(uint8_t u8Addr);
//========================================================================================================



#endif
