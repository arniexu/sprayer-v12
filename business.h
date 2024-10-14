#ifndef _BUSINESS_H_
#define _BUSINESS_H_

#include "nv.h"
#include "tm1650.h"
#include "output.h"

// 控制模式定义（数码管显示控制和按键处理逻辑）
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

// 喷淋状态定义
// 喷淋中状态
#define SPRAY_STARTING 1
// 喷淋关闭状态
#define SPRAY_STOPPING 2
// 缺水等待
#define SPRAY_WAITING 3
// 恢复水供给后开始喷淋前的倒数等待状态
#define SPRAY_DELAYING 4
// 等待遥控指令或者端子上面的有效信号
#define SPRAY_IDLE 0

unsigned int water_is_not_short(sprayerNvType *nv, unsigned char *s);
unsigned char start_spray(sprayerNvType *nv, unsigned char *s);
unsigned char stop_spray(sprayerNvType *nv, unsigned char *s);
void learn_code(sprayerNvType *nv, unsigned int *s);


#endif