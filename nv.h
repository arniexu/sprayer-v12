#ifndef NV_H
#define NV_H

#define     DATA_SIZE           128
#define     DATA_START_ADDR     0x3F00

#define 	 	START_COUNTER_OFFSET 0x02
#define  	  STOP_COUNTER_OFFSET 0x04
#define     LEFT_MODE_OFFSET 0x06
#define     RIGHT_MODE_OFFSET 0x08
//#define 		START_MULTIPLIER_OFFSET 0x0a
//#define 		STOP_MULTIPLIER_OFFSET 	0x0c
#define         LEVEL_WATER_SHORT_OFFSET 0x0a
#define         DELAY_WATER_SHORT_OFFSET 0x10
#define         START_MULTIPLIER_OFFSET  0x18
#define         STOP_MULTIPLIER_OFFSET   0x28

void flushFlash();

#endif