#include <MS51_16K.H>
#include "nv.h"
#include "business.h"
#include "iap.h"
#include "common.h"
#include "MS51_16K.h"
/*
MS51 flash 分区映射图
0000 - 3f00 16KB - 0x100 Flash 用于存储代码
3f00 - 3fff 256字节 用于存储用户设置
*/
/*
功能描述：从flash上读取4个字节到无符号整型数
对应地址：0x3f00 + 0x0c
对应长度：4个字节
*/
unsigned int readStopMultiplier()
{
	unsigned int val = 0;
	Read_DATAFLASH_ARRAY(DATA_START_ADDR+ STOP_MULTIPLIER_OFFSET, (unsigned char *)&val, sizeof(val));
	return val;
}

/*
功能描述：从flash上读取4个字节到无符号整型数
对应地址：0x3f00 + 0x10
对应长度：4个字节
*/
unsigned int readStartMultiplier()
{
	unsigned int val = 0;
	Read_DATAFLASH_ARRAY(DATA_START_ADDR+ START_MULTIPLIER_OFFSET, (unsigned char *)&val, sizeof(val));
	return val;
}
/*
功能描述：从flash上读取4个字节到无符号整型数
对应地址：0x3f00 + 0x15
对应长度：1个字节
*/
unsigned char readStopCounter(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR+ STOP_COUNTER_OFFSET);
}
/*
功能描述：从flash上读取4个字节到无符号整型数
对应地址：0x3f00 + 0x16
对应长度：1个字节
*/
unsigned char readStartCounter(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR + START_COUNTER_OFFSET);
}
/*
功能描述：从flash上读取4个字节到无符号整型数
对应地址：0x3f00 + 0x17
对应长度：1个字节
*/
unsigned char readWaterShortLevel(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR+ LEVEL_WATER_SHORT_OFFSET);
}

unsigned char readWaterShortDelay(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR + DELAY_WATER_SHORT_OFFSET);
}
/*
功能描述：将变量从内存同步到flash
注意点：耗时操作，比访问内存慢的多，慎用
注意点2：内存中的变量掉电是会丢失的，所以需要写入flash
*/
void writeFlash(sprayerNvType nv)
{
	unsigned char mode = 0;
	if (nv.start_multiplier == 40)
		mode = LEFT_FUNCTION_1_MODE;
	else if(nv.start_multiplier == 400)
		mode = LEFT_FUNCTION_2_MODE;
	else if(nv.start_multiplier == 400*6)
		mode = LEFT_FUNCTION_3_MODE;
	else if(nv.start_multiplier == 400*6*10)
		mode = LEFT_FUNCTION_4_MODE;
	else if(nv.start_multiplier == 400*6*10*6)
		mode = LEFT_FUNCTION_5_MODE;
	else if(nv.start_multiplier == 400*6*10*6*10)
		mode = LEFT_FUNCTION_6_MODE;
	else {
		nv.start_multiplier = 40;
		mode = LEFT_FUNCTION_1_MODE;
	}
	Write_DATAFLASH_BYTE(DATA_START_ADDR + LEFT_MODE_OFFSET, mode);
	Write_DATAFLASH_ARRAY(DATA_START_ADDR + START_MULTIPLIER_OFFSET, (unsigned char *)&nv.start_multiplier, sizeof(nv.start_multiplier));
	if (nv.stop_multiplier == 40)
		mode = RIGHT_FUNCTION_1_MODE;
	else if(nv.stop_multiplier == 400)
		mode = RIGHT_FUNCTION_2_MODE;
	else if(nv.stop_multiplier == 400*6)
		mode = RIGHT_FUNCTION_3_MODE;
	else if(nv.stop_multiplier == 400*6*10)
		mode = RIGHT_FUNCTION_4_MODE;
	else if(nv.stop_multiplier == 400*6*10*6)
		mode = RIGHT_FUNCTION_5_MODE;
	else if(nv.stop_multiplier == 400*6*10*6*10)
		mode = RIGHT_FUNCTION_6_MODE;
	else {
		nv.stop_multiplier = 40;
		mode = RIGHT_FUNCTION_1_MODE;
	}
	Write_DATAFLASH_BYTE(DATA_START_ADDR + RIGHT_MODE_OFFSET, mode);
	Write_DATAFLASH_ARRAY(DATA_START_ADDR + STOP_MULTIPLIER_OFFSET, (unsigned char *)&nv.stop_multiplier, sizeof(nv.stop_multiplier));
	Write_DATAFLASH_BYTE(DATA_START_ADDR + START_COUNTER_OFFSET, nv.start_counter%100);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + STOP_COUNTER_OFFSET, nv.stop_counter%100);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + LEVEL_WATER_SHORT_OFFSET, nv.level_water_short%2);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + DELAY_WATER_SHORT_OFFSET, nv.delay_water_short%100);
	//flushFlash();
}

unsigned char readLeftMode(void)
{
	unsigned char mode = Read_APROM_BYTE(DATA_START_ADDR + LEFT_MODE_OFFSET);
	if ((mode < LEFT_FUNCTION_1_MODE || mode > LEFT_TIME_EFFECTIVE_MODE) && mode != LEARN_MODE)
		return LEFT_FUNCTION_1_MODE;
	else
		return mode;
}

unsigned char readRightMode(void)
{
	unsigned char mode = Read_APROM_BYTE(DATA_START_ADDR + RIGHT_MODE_OFFSET);
	if ((mode < RIGHT_FUNCTION_1_MODE || mode > RIGHT_TIME_EFFECTIVE_MODE) && mode != LEARN_MODE)
		return RIGHT_FUNCTION_1_MODE;
	else
		return mode;
}

#if 0
unsigned char IS_VALID_LEFT_MODE(unsigned char mode)
{
	if ((mode >= LEFT_FUNCTION_1_MODE && mode <= LEFT_FUNCTION_8_MODE)
	|| (mode == LEARN_MODE)
	|| (mode == LEFT_TIME_EFFECTIVE_MODE))
		return TRUE;
	else
	 	return FALSE;
}

unsigned char IS_VALID_RIGHT_MODE(unsigned char mode)
{
	if ((mode >= RIGHT_FUNCTION_1_MODE && mode <= RIGHT_FUNCTION_8_MODE)
	|| (mode == LEARN_MODE)
	|| (mode == RIGHT_TIME_EFFECTIVE_MODE))
		return TRUE;
	else
	 	return FALSE;
}
#endif

void readFlash(sprayerNvType *nv)
{
	if (!nv)
		return;
	nv->start_counter = readStartCounter()%100;
	nv->stop_counter = readStopCounter()%100;
	nv->start_multiplier = readStartMultiplier();
	nv->stop_multiplier = readStopMultiplier();
	nv->level_water_short = readWaterShortLevel()%2;
	nv->delay_water_short = readWaterShortDelay()%100;
	nv->left_mode = readLeftMode();
	nv->right_mode = readRightMode();
}
