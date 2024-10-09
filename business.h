#ifndef _BUSINESS_H_
#define _BUSINESS_H_

#include "nv.h"
#include "tm1650.h"
#include "output.h"

#define LEFT_FUNCTION_1_MODE 1
#define LEFT_FUNCTION_2_MODE 2
#define LEFT_FUNCTION_3_MODE 3
#define LEFT_FUNCTION_4_MODE 4
#define LEFT_FUNCTION_5_MODE 5
#define LEFT_FUNCTION_6_MODE 6
#define LEFT_FUNCTION_7_MODE 7
#define LEFT_FUNCTION_8_MODE 8
#define LEFT_TIME_EFFECTIVE_MODE 9

#define RIGHT_FUNCTION_1_MODE 11
#define RIGHT_FUNCTION_2_MODE 12
#define RIGHT_FUNCTION_3_MODE 13
#define RIGHT_FUNCTION_4_MODE 14
#define RIGHT_FUNCTION_5_MODE 15
#define RIGHT_FUNCTION_6_MODE 16
#define RIGHT_FUNCTION_7_MODE 17
#define RIGHT_FUNCTION_8_MODE 18
#define RIGHT_TIME_EFFECTIVE_MODE 19
#define LEARN_MODE 21

#define SPRAY_STARTING 1
#define SPRAY_STOPPING 2
#define SPRAY_WAITING 3
#define SPRAY_DELAYING 4
#define SPRAY_IDLE 0

unsigned int water_is_not_short(sprayerNvType *nv, unsigned char *s);
unsigned char start_spray(sprayerNvType *nv, unsigned char *s);
unsigned char stop_spray(sprayerNvType *nv, unsigned char *s);
void learn_code(sprayerNvType *nv, unsigned int *s);


#endif