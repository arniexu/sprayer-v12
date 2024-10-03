/** ============================================
 * 7 segment display driver for JY-MCU module based on TM1650 chip
 * Copyright (c) 2015 Anatoli Arkhipenko
 * 2018 Alexey Voloshin <microseti@yandex.ru>
 * 
 * 
 * Changelog:
 *	v1.0.0:
 *		2015-02-24 - Initial release 
 *
 *	v1.0.1:  
 *		2015-04-27 - Added support of program memery (PROGMEM) to store the ASCII to Segment Code table
 *
 *	v1.0.2:
 *		2015-08-08 - Added check if panel is connected during init. All calls will be disabled is panel was not connected during init.
 *
 *  v1.1.0:
 *      2015-12-20 - code clean up. Moved to a single header file. Added Gradual brightness method
 *
 *  v1.2.0:
 *      2018-09-04 - Added buttons support
 *
 * ===============================================*/

#ifndef _TM1650_H_
#define _TM1650_H_

#include "MS51_16K.h"
#include <stdio.h>
#include <stdlib.h>


#define TM1650_USE_PROGMEM
#define TM1650_DISPLAY_BASE 0x68 // Address of the left-most digit 
#define TM1650_DCTRL_BASE   0x48 // Address of the control register of the left-most digit
#define TM1650_BUTTON_BASE  0x49
#define TM1650_NUM_DIGITS   4 // max number of digits
#define TM1650_MAX_STRING   8 // number of digits

#define TM1650_BIT_ONOFF    0x01
#define TM1650_MSK_ONOFF    0xfe
#define TM1650_BIT_DOT      0x01
#define TM1650_MSK_DOT      0xf7
#define TM1650_DOT          0x80
#define TM1650_BRIGHT_SHIFT 6
#define TM1650_MSK_BRIGHT   0x8f
#define TM1650_MIN_BRIGHT   0
#define TM1650_MAX_BRIGHT   7

struct TM1650TypeDef {
		char* iPosition;
		char	iActive;
		unsigned int	iNumDigits;
		unsigned int	iBrightness;
		char	iString[TM1650_MAX_STRING+1];
		uint8_t 	iBuffer[TM1650_NUM_DIGITS+1];
		uint8_t 	iCtrl[TM1650_NUM_DIGITS];
};

//		TM1650(struct TM1650TypeDef *tm, unsigned int aNumDigits);

void	tm1650_init(struct TM1650TypeDef *tm, UINT8);
void	tm1650_clear(struct TM1650TypeDef *tm);
void	tm1650_displayOn(struct TM1650TypeDef *tm);
void	tm1650_displayOff(struct TM1650TypeDef *tm);
void	tm1650_displayState(struct TM1650TypeDef *tm, char aState);
void	tm1650_displayString(struct TM1650TypeDef *tm, char *aString);
void	tm1650_displayChar(struct TM1650TypeDef *tm, uint8_t pos, uint8_t c);
void	tm1650_displayChar_withDot(struct TM1650TypeDef *tm, uint8_t pos, uint8_t c);
void    tm1650_displaySegment(struct TM1650TypeDef *tm, uint8_t aPos,  uint8_t c, unsigned char i);
void	tm1650_displayChar_withDot_underMask(struct TM1650TypeDef *tm, uint8_t pos, uint8_t c, uint8_t mask);

int 	tm1650_displayRunning(struct TM1650TypeDef *tm, char *aString);
int 	tm1650_displayRunningShift(struct TM1650TypeDef *tm);
void	tm1650_setBrightness(struct TM1650TypeDef *tm, unsigned int aValue);
void	tm1650_setBrightnessGradually(struct TM1650TypeDef *tm, unsigned int aValue);
unsigned int tm1650_getBrightness(struct TM1650TypeDef *tm);

void	tm1650_controlPosition(struct TM1650TypeDef *tm, unsigned int aPos, uint8_t aValue);
void	tm1650_setPosition(struct TM1650TypeDef *tm, unsigned int aPos, uint8_t aValue);
void	tm1650_setDot(struct TM1650TypeDef *tm, unsigned int aPos, unsigned char aState);
uint8_t	tm1650_getPosition(struct TM1650TypeDef *tm, unsigned int aPos);
unsigned int	tm1650_getNumPositions(struct TM1650TypeDef *tm);
UINT8	tm1650_getButtons(struct TM1650TypeDef *tm);
unsigned char isButtonCodeValid(UINT8 c);

#endif /* _TM1650_H_ */
