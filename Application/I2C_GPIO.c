#include "i2c_gpio.h"
#include "timer.h"
#include "hardware.h"
#include <intrins.h>
/**
 * @brief       初始化I2C管脚成开漏输出
 * @param       无
 * @retval      无
 */
void IIC_Init(void)
{
	#ifdef DEVOARD 
	P13_OPENDRAIN_MODE;
	P14_OPENDRAIN_MODE;
	#else
	P00_OPENDRAIN_MODE;
	P04_OPENDRAIN_MODE;
	#endif
}

/*
功能描述：单片机写出一个0或1到i2c的数据线
*/
static void IIC_SDA(uint8_t val)
{
	if(val)
		TM1650_SDA = 1;
	else
		TM1650_SDA = 0;
}
/*
功能描述：单片机驱动时钟线产生高电平或低电平
*/
static void IIC_SCL(uint8_t val)
{
	if(val)
		TM1650_SCL = 1;
	else
		TM1650_SCL = 0;
}

/**
 * @brief       IIC延时函数,用于控制IIC读写速度
 * @param       无
 * @retval      无
 */
static void IIC_delay2us(void)
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}

/**
 * @brief       产生IIC起始信号
 * @param       无
 * @retval      无
 */
void IIC_Start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    IIC_delay2us();
    IIC_SDA(0);     /* START信号: 当SCL为高时, SDA从高变成低, 表示起始信号 */
    IIC_delay2us();
    IIC_SCL(0);     /* 钳住I2C总线，准备发送或接收数据 */
    IIC_delay2us();
}

/**
 * @brief       产生IIC停止信号
 * @param       无
 * @retval      无
 */
void IIC_Stop(void)
{
    IIC_SDA(0);     /* STOP信号: 当SCL为高时, SDA从低变成高, 表示停止信号 */
    IIC_delay2us();
    IIC_SCL(1);
    IIC_delay2us();
    IIC_SDA(1);     /* 发送I2C总线结束信号 */
    IIC_delay2us();
}

/**
 * @brief       等待应答信号到来
 * @param       无
 * @retval      1，接收应答失败
 *              0，接收应答成功
 */
uint8_t IIC_Wait_Ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;

    IIC_SDA(1);     /* 主机释放SDA线(此时外部器件可以拉低SDA线) */
    IIC_delay2us();
    IIC_SCL(1);     /* SCL=1, 此时从机可以返回ACK */
    IIC_delay2us();

    while (TM1650_SDA)    /* 等待应答 */
    {
        waittime++;

        if (waittime > 250)
        {
            IIC_Stop();
            rack = 1;
            break;
        }
    }

    IIC_SCL(0);     /* SCL=0, 结束ACK检查 */
    IIC_delay2us();
    return rack;
}
#if 0
/**
 * @brief       产生ACK应答
 * @param       无
 * @retval      无
 */
void IIC_Ack(void)
{
    IIC_SDA(0);     /* SCL 0 -> 1  时 SDA = 0,表示应答 */
    IIC_delay2us();
    IIC_SCL(1);     /* 产生一个时钟 */
    IIC_delay2us();
    IIC_SCL(0);
    IIC_delay2us();
    IIC_SDA(1);     /* 主机释放SDA线 */
    IIC_delay2us();
}

/**
 * @brief       不产生ACK应答
 * @param       无
 * @retval      无
 */
void IIC_Nack(void)
{
    IIC_SDA(1);     /* SCL 0 -> 1  时 SDA = 1,表示不应答 */
    IIC_delay2us();
    IIC_SCL(1);     /* 产生一个时钟 */
    IIC_delay2us();
    IIC_SCL(0);
    IIC_delay2us();
}
#endif
/**
 * @brief       IIC发送一个字节
 * @param       data: 要发送的数据
 * @retval      无
 */
void IIC_Send_Byte(uint8_t dat)
{
    uint8_t t;
    
    for (t = 0; t < 8; t++)
    {
        IIC_SDA((dat & 0x80) >> 7);    /* 高位先发送 */
        IIC_delay2us();
        IIC_SCL(1);
        IIC_delay2us();
        IIC_SCL(0);
        dat <<= 1;     /* 左移1位,用于下一次发送 */
    }
    IIC_SDA(1);         /* 发送完成, 主机释放SDA线 */
}

#if 0
/**
 * @brief       IIC读取一个字节
 * @param       ack:  ack=1时，发送ack; ack=0时，发送nack
 * @retval      接收到的数据
 */
uint8_t IIC_Receive_Byte(uint8_t ack)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++ )    /* 接收1个字节数据 */
    {
        receive <<= 1;  /* 高位先输出,所以先收到的数据位要左移 */
        IIC_SCL(1);
        IIC_delay2us();

        if (TM1650_SDA)      /*高电平读数据*/
        {
            receive++;
        }
        
        IIC_SCL(0);
        IIC_delay2us();
    }

    if (!ack)
    {
        IIC_Nack();     /* 发送nACK */
    }
    else
    {
        IIC_Ack();      /* 发送ACK */
    }

    return receive;
}
#endif
//--------------------------------------------------------------------------------------------
//----  Byte Write----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void I2C_Write_Byte(UINT8 u8Addr, UINT8 u8DAT)
{
	IIC_Start();
	IIC_Send_Byte(u8Addr & ~0x01);
	if (IIC_Wait_Ack() != 0)
		return;
	IIC_Send_Byte(u8DAT);
	if (IIC_Wait_Ack() != 0)
		return;
	IIC_Stop();
}

#if 0	
void I2C_Write_Word(UINT8 u8Addr, UINT8 u8DAT[], UINT8 siz)
{
	unsigned char  u8Count;
	/* Write Step1 */
	IIC_Start();
	
	/* Write Step2 */
	IIC_Send_Byte(u8Addr & ~0x01);
	if (IIC_Wait_Ack() != 0)
		return;
	
	/* Write Step3 */
	for (u8Count = 0; u8Count < siz; u8Count++)
	{
		/* Write Step2 */
		IIC_Send_Byte(u8DAT[u8Count]);
		if (IIC_Wait_Ack() != 0)
			return;
	}

	IIC_Stop();
}

//--------------------------------------------------------------------------------------------
//----  Byte Read ----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
UINT8 I2C_Read_Byte(UINT8 u8Addr)
{
	UINT8 u8Data = 0;
	
	IIC_Start();
	/* Write Step2 */
	IIC_Send_Byte(u8Addr & ~0x01);
	if (IIC_Wait_Ack() != 0)
		return 0xff;
	/* Step13 */
	u8Data = IIC_Receive_Byte(1);
        
  /* Step15 */
	IIC_Stop();
	return u8Data;
}
//========================================================================================================

#endif