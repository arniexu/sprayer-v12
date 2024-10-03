/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2020 nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Nuvoton Technoledge Corp. 
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : Apr/29/2020
//***********************************************************************************************************

//***********************************************************************************************************
//  File Function: MS51 I2C master mode demo code, the Slave address = 0xA4
//
//   ____________            _____________ 
//  |            |   SDA    |             |
//  |            |<-------->|             |
//  |            |          |             |
//  |MS51(M)     |          | MS51(S)     |
//  |(I2C_Master)|          | (I2C_Slave) |
//  |            |   SCL    |             |
//  |            |--------->|             |
//  |____________|          |_____________|
//
//  The protocol of I2C is master: start -> write 10 byte(ACK) ->stop -> start ->read 10 byte(ACK) -> stop
//***********************************************************************************************************

#include  "I2C_M.h"
#include <stdlib.h>
#include <stdio.h>

#define I2C_CLOCK                 13
#define DIG0_ADDRESS         			0x34
#define DIG1_ADDRESS         			0x6A
#define DIG2_ADDRESS         			0x6C
#define DIG3_ADDRESS         			0x6E

#define I2C_WR                    0
#define I2C_RD                    1

#define LOOP_SIZE                 1

//========================================================================================================
void Init_I2C(unsigned long u32SYSCLK, unsigned long u32I2CCLK)
{
		P13_OPENDRAIN_MODE;
		P14_OPENDRAIN_MODE;
		clr_I2CON_I2CPX;

    /* Set I2C clock rate */
		I2CLK = (u32SYSCLK/4/u32I2CCLK-1); 
    /* Enable I2C */
    set_I2CON_I2CEN;                                   
}
//========================================================================================================
void I2C_Error(void)
{
    // do not wait forever 
		printf("error in i2c comms.\n\r");
		return;
}
//--------------------------------------------------------------------------------------------
//----  Address Write----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void I2C_Write_Addr(UINT8 u8Addr){
    /* Write Step1 */
    set_I2CON_STA;                                    /* Send Start bit to I2C EEPROM */
    clr_I2CON_SI;
    while (!SI);                                /*Check SI set or not  */
    if (I2STAT != 0x08)                         /*Check status value after every step   */
        I2C_Error();
    
    /* Write Step2 */
    clr_I2CON_STA;                                    /*STA=0*/
    I2DAT = (u8Addr | I2C_WR);
    clr_I2CON_SI;
    while (!SI);                                /*Check SI set or not */
    if (I2STAT != 0x18)              
        I2C_Error();

    /* Write Step3 */
    set_I2CON_STO;
    clr_I2CON_SI;
    while (STO);                                /* Check STOP signal */
	}

//--------------------------------------------------------------------------------------------
//----  Byte Write----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void I2C_Write_Byte(UINT8 u8Addr, UINT8 u8DAT)
{
    unsigned char  u8Count;
    /* Write Step1 */
    set_I2CON_STA;                                    /* Send Start bit to I2C EEPROM */
    clr_I2CON_SI;
    while (!SI);                                /*Check SI set or not  */
    if (I2STAT != 0x08)                         /*Check status value after every step   */
        I2C_Error();
    
    /* Write Step2 */
    clr_I2CON_STA;                                    /*STA=0*/
    I2DAT = (u8Addr | I2C_WR);
    clr_I2CON_SI;
    while (!SI);                                /*Check SI set or not */
    if (I2STAT != 0x18)              
        I2C_Error();

    /* Write Step3 */
    for (u8Count = 0; u8Count < 1; u8Count++)
    {
        I2DAT = u8DAT;
        clr_I2CON_SI;
        while (!SI);                            /*Check SI set or not*/
        if (I2STAT != 0x28)              
            I2C_Error();

        u8DAT = ~u8DAT;        
    }

    /* Write Step4 */
    set_I2CON_STO;
    clr_I2CON_SI;
    while (STO);                                /* Check STOP signal */
	}

	
void I2C_Write_Word(UINT8 u8Addr, UINT8 u8DAT[], UINT8 siz)
{
    unsigned char  u8Count;
    /* Write Step1 */
    set_I2CON_STA;                                    /* Send Start bit to I2C EEPROM */
    clr_I2CON_SI;
    while (!SI);                                /*Check SI set or not  */
    if (I2STAT != 0x08)                         /*Check status value after every step   */
        I2C_Error();
    
    /* Write Step2 */
    clr_I2CON_STA;                                    /*STA=0*/
    I2DAT = (u8Addr | I2C_WR);
    clr_I2CON_SI;
    while (!SI);                                /*Check SI set or not */
    if (I2STAT != 0x18)              
        I2C_Error();

    /* Write Step3 */
    for (u8Count = 0; u8Count < siz; u8Count++)
    {
        I2DAT = u8DAT[u8Count]	;
        clr_I2CON_SI;
        while (!SI);                            /*Check SI set or not*/
        if (I2STAT != 0x28)              
            I2C_Error();

        u8DAT[u8Count] = ~u8DAT[u8Count];        
    }

    /* Write Step4 */
    set_I2CON_STO;
    clr_I2CON_SI;
    while (STO);                                /* Check STOP signal */
	}
//--------------------------------------------------------------------------------------------
//----  Byte Read ----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
UINT8 I2C_Read_Byte(UINT8 u8Addr)
{
		unsigned char  u8Data = 0;
    unsigned char  u8Count;
    /* Read Step1 */
    set_I2CON_STA;
    clr_I2CON_SI;          
    while (!SI);                                //Check SI set or not
    if (I2STAT != 0x08)                         //Check status value after every step
        I2C_Error();

    /* Step13 */
    clr_I2CON_STA;                                    //STA needs to be cleared after START codition is generated
    I2DAT = (u8Addr | I2C_RD);
    clr_I2CON_SI;
    while (!SI);                                //Check SI set or not
    if (I2STAT != 0x40)              
        I2C_Error();
    
    /* Step14 */
    for (u8Count = 0; u8Count <1; u8Count++)
    {
        set_I2CON_AA;
        clr_I2CON_SI;        
        while (!SI);                            //Check SI set or not

        if (I2STAT != 0x50)              
            I2C_Error();
				
				u8Data = I2DAT;
    } 
    
    /* Step15 */
    clr_I2CON_AA;
    clr_I2CON_SI;
    while (!SI);                                //Check SI set or not
    if (I2STAT != 0x58)              
        I2C_Error();

    /* Step16 */
    set_I2CON_STO;
    clr_I2CON_SI;
    while (STO);                                /* Check STOP signal */ 
		return u8Data;
}
//========================================================================================================


