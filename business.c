#include "business.h"
#include "Application/button.h"
#include "Application/nv.h"
#include "Application/output.h"
#include "Application/systick.h"
#include "tm1650.h"
#include "nv.h"
#include "output.h"
#include "systick.h"
#include "button.h"

extern unsigned int flag_collaborate;

extern unsigned int tButton;

// 缺水的业务逻辑
unsigned int water_is_not_short(sprayerNvType *nv, unsigned char *spraying)
{
	unsigned int water_present = TRUE;
	unsigned int now = get_Timer1_Systemtick();
	nv->level_water_short = readWaterShortLevel();
	// 定时半小时不断检测缺水条件
	while (nv->level_water_short == external_water_short_blocked(*nv) && get_Timer1_Systemtick() - now < 30*60*400)
	{
		// 更新缺水，高低电平是根据用户F7设定来表示是否缺水的
    	nv->level_water_short = readWaterShortLevel();
		relay = 0;
		*spraying = SPRAY_WAITING;
		water_present = FALSE;
		beeper_2hz();
		if (tButton == RF_STOP_BUTTON
		|| TRUE == external_stop_valid_blocked()
		|| (flag_collaborate && collaborate_ex_button == 1)) // waiting can be forcibly stopped
		{

			beeper_once();
			beeper_2hz_stop();
			*spraying = SPRAY_IDLE;
			tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
			return FALSE;
		}
		else if(tButton == RF_START_BUTTON) // waiting can not be skipped
		{
			beeper_once();
			tButton = 0;
		}
	}
	beeper_2hz_stop();
	nv->level_water_short = readWaterShortLevel();
	if(water_present == FALSE && nv->level_water_short == external_water_short_blocked(*nv)) // water is still short 
	{
		relay = 0;
		*spraying = SPRAY_IDLE;
		return FALSE;
	}
	else if(water_present == FALSE && nv->level_water_short != external_water_short_blocked(*nv)) // water is not short now
	{
        unsigned int delay_water_short_backup = 0;            
        nv->delay_water_short = readWaterShortDelay();
        delay_water_short_backup = nv->delay_water_short;
		*spraying = SPRAY_DELAYING;
		nv->delay_water_short = readWaterShortDelay();
		while(0 < nv->delay_water_short)
		{
			unsigned int temp = 0;
			unsigned int prev = 0;
			temp = get_Timer1_Systemtick();
			while (get_Timer1_Systemtick() - temp < 400) {
				if(nv->level_water_short == external_water_short_blocked(*nv))
				{
					return TRUE;
				}
				else if (tButton == RF_STOP_BUTTON
				|| external_stop_valid_blocked()
				|| (flag_collaborate && collaborate_ex_button == 1)) // waiting can be forcibly stopped
				{
					beeper_once();
					tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					return FALSE;
				}
				else if (tButton == RF_START_BUTTON
				|| (external_start_valid_blocked() && !external_stop_valid_blocked())
				|| (external_collaborate_valid_blocked() && !external_stop_valid_blocked()))
				{
					beeper_once();
					tButton = (tButton == RF_START_BUTTON ? 0 : tButton);
					return TRUE;
				}
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 1);
			}
			nv->delay_water_short --;
		}
		nv->delay_water_short = delay_water_short_backup;
		*spraying = SPRAY_IDLE;
        return TRUE;
	}else if (water_present) // water is never short
	{
		return TRUE;
	}
	else{
		*spraying = SPRAY_IDLE;
		return FALSE;
	}
}
// 喷淋中的业务逻辑
unsigned char start_spray(sprayerNvType *nv, unsigned char *spraying)
{
	unsigned int start_counter_backup =nv->start_counter%100;
	unsigned int cnt = nv->start_multiplier;
	unsigned int now = 0;
	if (nv->left_mode == LEFT_TIME_EFFECTIVE_MODE)
	{
		while(nv->start_counter>0)
		{
			// 吸合继电器，开始喷淋
			relay = 1;
			// 设置显示模式
			*spraying = SPRAY_STARTING;
			// 读取定时间隔
			cnt = nv->start_multiplier;
			now = get_Timer1_Systemtick();
			// 延时start_multiplier*2.54ms
			while(get_Timer1_Systemtick() - now < cnt){
				// 延时期间每2.54ms检查一遍后面端子
				Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
				// 延时期间每隔2.54ms检查停止条件
				if (tButton == RF_STOP_BUTTON
					|| external_stop_valid_blocked()
					|| (flag_collaborate && collaborate_ex_button == 1))
				{
					// 蜂鸣器响一声
					beeper_once();
					// 关闭继电器
					relay = 0;
					// 回复喷淋时间的设定
					nv->start_counter = start_counter_backup;
					// 清除遥控按键键码
                    tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					// 停止整个流程
					return FALSE;
				}
				// 延时期间每隔2.54ms检查开始条件
				else if(tButton == RF_START_BUTTON)
				{
					// 蜂鸣器响一声
					beeper_once();
					// 清除遥控按键键码
					tButton = (tButton == RF_START_BUTTON ? 0 : tButton);
				}
				// 延時期间每隔2.54ms检查是否缺水
				else if(nv->level_water_short == external_water_short_blocked(*nv))
				{
					// 缺水关闭继电器
					relay = 0; 
					nv->start_counter = start_counter_backup;
					return TRUE;
				}
			}
			nv->start_counter --;
		}
	}
	nv->start_counter = start_counter_backup;
	return TRUE;
}

// 喷淋间隔的业务逻辑
unsigned char stop_spray(sprayerNvType *nv, unsigned char *spraying)
{
	unsigned int cnt = nv->stop_multiplier;
	unsigned int stop_counter_backup = nv->stop_counter%100;
	unsigned int now = 0;
	// 判断当前显示模式是否正确
	if (nv->right_mode == RIGHT_TIME_EFFECTIVE_MODE)
	{
		// 判断不喷淋的时间长度是否大于0
		while(nv->stop_counter > 0)
		{
			// 关闭继电器停止喷淋
			relay = 0;
			// 设置喷淋模式
			*spraying = SPRAY_STOPPING;
			// 每个定时间隔对应的时间长度
			cnt = nv->stop_multiplier;
			// 获取当前系统时间
			now =  get_Timer1_Systemtick();
			// 延时用户指定的时间长度=stop_multiplier*2.54ms
			while(get_Timer1_Systemtick() - now < cnt){
				Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
				// 如果遥控停止按键按下
				if (tButton == RF_STOP_BUTTON
					// 后面端子检测到停止信号
					|| external_stop_valid_blocked()
					// 联动模式下联动信号丢失
					|| (flag_collaborate && collaborate_ex_button == 1))
				{
					// 蜂鸣器响一声
					beeper_once();
					// 从flash中恢复stop_counter
					nv->stop_counter = stop_counter_backup;
					// 清除遥控键码
					tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					// 退出整个喷淋流程
					return FALSE;
				}
				// 收到遥控器上的开始按键
				else if(tButton == RF_START_BUTTON)
				{
					//蜂鸣器响一声
					beeper_once();
					//清除遥控器的按键码
					tButton = 0;
				}
				// 如果检测到缺水
				else if (nv->level_water_short == external_water_short_blocked(*nv))
				{
					// 从flash中恢复stop_counter
				    nv->stop_counter = stop_counter_backup;
					// 继续下个喷淋流程
					return TRUE;
				}
			}
			// 
			nv->stop_counter --;
		}
	}
	nv->stop_counter = stop_counter_backup;
	return TRUE;
}

// 判断学码按键是否长按
static unsigned char is_learn_button_long_pressed(void)
{
	//记录当前时间
	unsigned int now = get_Timer1_Systemtick();
	//每~10ms判斷按鍵一次是否按下
	while (learn_button == 0)
	{
		// 延时5ms
		Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 2);
		// 按键持续按下3s
		if (get_Timer1_Systemtick() - now > 1200)
			// 就是一次学码按键长按事件
			return LEARN_LONG_BUTTON;
	}
	return 0;
}

// 学码流程
void learn_code(sprayerNvType *nv, unsigned int* learning)
{
	// 获取当前运行时间的系统滴答个数，从上电开始2.54ms的个数
	unsigned int now = get_Timer1_Systemtick();
	unsigned int now1 = 0;
	// 检查当前是不是在学码模式下
	if (nv->left_mode != LEARN_MODE || nv->right_mode != LEARN_MODE)
		return;
	// 倒数8s等待遥控信号
	// tButton 遥控按钮的键码
	while(get_Timer1_Systemtick() - now < 400*8 && tButton == 0)
	{
		// 学码模式下长按学码键
		if (is_learn_button_long_pressed())
		{
			// 蜂鸣器响一声
			beeper_once();
			// 清除之前配对的遥控器的地址
			clearAddress();
			break;
		}
		*learning = TRUE;
		Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
	}
	//收到遥控键码
	if(tButton == RF_START_BUTTON || tButton == RF_STOP_BUTTON)
	{
		//标记学码完成
		*learning = FALSE;
		//蜂鸣器响一声
		beeper_once();
		//保存遥控地址码到flash
		saveToFlash();
		//获取当前系统滴答时间
		now1 = get_Timer1_Systemtick();
		//延时1s
		while(get_Timer1_Systemtick() -	now1 < 400)
			Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
		//标记要进入的下个模式
		nv->left_mode = LEFT_TIME_EFFECTIVE_MODE;
		nv->right_mode = RIGHT_TIME_EFFECTIVE_MODE;
		//清除遥控按键码
		tButton = 0;
		return;
	}
	// 未收到遥控按键码
	*learning = FALSE;
	// 延時2s
	Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 800);
	// 標記要進去的下個模式
	nv->left_mode = LEFT_TIME_EFFECTIVE_MODE;
	nv->right_mode = RIGHT_TIME_EFFECTIVE_MODE;
	tButton = 0;
	return;
}

