#ifndef _I2C_M_H_
#define _I2C_M_H_

#include "MS51_16K.h"


void Init_I2C(unsigned long u32SYSCLK, unsigned long u32I2CCLK);
//========================================================================================================
void I2C_Error(void);
//========================================================================================================
void I2C_Write_Addr(UINT8 u8Addr);
//--------------------------------------------------------------------------------------------
//----  Page Write----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void I2C_Write_Byte(UINT8 u8Addr, UINT8 u8DAT);
void I2C_Write_Word(UINT8 u8Addr, UINT8 u8DAT[], UINT8 siz);
//--------------------------------------------------------------------------------------------
//----  Page Read ----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
UINT8 I2C_Read_Byte(UINT8 u8DAT);
//========================================================================================================


#endif