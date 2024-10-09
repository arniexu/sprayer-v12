#ifndef NV_H
#define NV_H

typedef struct {
    unsigned int start_multiplier;
    unsigned char start_counter;
    unsigned int stop_multiplier;
    unsigned char stop_counter;
    unsigned char level_water_short;
    unsigned char delay_water_short;
    unsigned char right_mode;
    unsigned char left_mode;
}sprayerNvType;

#define     DATA_SIZE           128
#define     DATA_START_ADDR     0x3F00

#define 	 	START_COUNTER_OFFSET 0x02
#define  	  STOP_COUNTER_OFFSET 0x04
#define     LEFT_MODE_OFFSET 0x06
#define     RIGHT_MODE_OFFSET 0x08
#define         LEVEL_WATER_SHORT_OFFSET 0x0a
#define         DELAY_WATER_SHORT_OFFSET 0x10
#define         START_MULTIPLIER_OFFSET  0x18
#define         STOP_MULTIPLIER_OFFSET   0x28

unsigned int readStopMultiplier(void);
unsigned int readStartMultiplier(void);
unsigned char readStopCounter(void);
unsigned char readStartCounter(void);
unsigned char readLeftMode(void);
unsigned char readRightMode(void);
unsigned char readWaterShortLevel(void);
unsigned char readWaterShortDelay(void);
void writeFlash(sprayerNvType nv);
void readFlash(sprayerNvType *nv);

#endif