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

// ȱˮ��ҵ���߼�
unsigned int water_is_not_short(sprayerNvType *nv, unsigned char *spraying)
{
	unsigned int water_present = TRUE;
	unsigned int now = get_Timer1_Systemtick();
	nv->level_water_short = readWaterShortLevel();
	// ��ʱ��Сʱ���ϼ��ȱˮ����
	while (nv->level_water_short == external_water_short_blocked(*nv) && get_Timer1_Systemtick() - now < 30*60*400)
	{
		// ����ȱˮ���ߵ͵�ƽ�Ǹ����û�F7�趨����ʾ�Ƿ�ȱˮ��
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
// �����е�ҵ���߼�
unsigned char start_spray(sprayerNvType *nv, unsigned char *spraying)
{
	unsigned int start_counter_backup =nv->start_counter%100;
	unsigned int cnt = nv->start_multiplier;
	unsigned int now = 0;
	if (nv->left_mode == LEFT_TIME_EFFECTIVE_MODE)
	{
		while(nv->start_counter>0)
		{
			// ���ϼ̵�������ʼ����
			relay = 1;
			// ������ʾģʽ
			*spraying = SPRAY_STARTING;
			// ��ȡ��ʱ���
			cnt = nv->start_multiplier;
			now = get_Timer1_Systemtick();
			// ��ʱstart_multiplier*2.54ms
			while(get_Timer1_Systemtick() - now < cnt){
				// ��ʱ�ڼ�ÿ2.54ms���һ��������
				Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
				// ��ʱ�ڼ�ÿ��2.54ms���ֹͣ����
				if (tButton == RF_STOP_BUTTON
					|| external_stop_valid_blocked()
					|| (flag_collaborate && collaborate_ex_button == 1))
				{
					// ��������һ��
					beeper_once();
					// �رռ̵���
					relay = 0;
					// �ظ�����ʱ����趨
					nv->start_counter = start_counter_backup;
					// ���ң�ذ�������
                    tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					// ֹͣ��������
					return FALSE;
				}
				// ��ʱ�ڼ�ÿ��2.54ms��鿪ʼ����
				else if(tButton == RF_START_BUTTON)
				{
					// ��������һ��
					beeper_once();
					// ���ң�ذ�������
					tButton = (tButton == RF_START_BUTTON ? 0 : tButton);
				}
				// �ӕr�ڼ�ÿ��2.54ms����Ƿ�ȱˮ
				else if(nv->level_water_short == external_water_short_blocked(*nv))
				{
					// ȱˮ�رռ̵���
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

// ���ܼ����ҵ���߼�
unsigned char stop_spray(sprayerNvType *nv, unsigned char *spraying)
{
	unsigned int cnt = nv->stop_multiplier;
	unsigned int stop_counter_backup = nv->stop_counter%100;
	unsigned int now = 0;
	// �жϵ�ǰ��ʾģʽ�Ƿ���ȷ
	if (nv->right_mode == RIGHT_TIME_EFFECTIVE_MODE)
	{
		// �жϲ����ܵ�ʱ�䳤���Ƿ����0
		while(nv->stop_counter > 0)
		{
			// �رռ̵���ֹͣ����
			relay = 0;
			// ��������ģʽ
			*spraying = SPRAY_STOPPING;
			// ÿ����ʱ�����Ӧ��ʱ�䳤��
			cnt = nv->stop_multiplier;
			// ��ȡ��ǰϵͳʱ��
			now =  get_Timer1_Systemtick();
			// ��ʱ�û�ָ����ʱ�䳤��=stop_multiplier*2.54ms
			while(get_Timer1_Systemtick() - now < cnt){
				Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
				// ���ң��ֹͣ��������
				if (tButton == RF_STOP_BUTTON
					// ������Ӽ�⵽ֹͣ�ź�
					|| external_stop_valid_blocked()
					// ����ģʽ�������źŶ�ʧ
					|| (flag_collaborate && collaborate_ex_button == 1))
				{
					// ��������һ��
					beeper_once();
					// ��flash�лָ�stop_counter
					nv->stop_counter = stop_counter_backup;
					// ���ң�ؼ���
					tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					// �˳�������������
					return FALSE;
				}
				// �յ�ң�����ϵĿ�ʼ����
				else if(tButton == RF_START_BUTTON)
				{
					//��������һ��
					beeper_once();
					//���ң�����İ�����
					tButton = 0;
				}
				// �����⵽ȱˮ
				else if (nv->level_water_short == external_water_short_blocked(*nv))
				{
					// ��flash�лָ�stop_counter
				    nv->stop_counter = stop_counter_backup;
					// �����¸���������
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

// �ж�ѧ�밴���Ƿ񳤰�
static unsigned char is_learn_button_long_pressed(void)
{
	//��¼��ǰʱ��
	unsigned int now = get_Timer1_Systemtick();
	//ÿ~10ms�Дఴ�Iһ���Ƿ���
	while (learn_button == 0)
	{
		// ��ʱ5ms
		Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 2);
		// ������������3s
		if (get_Timer1_Systemtick() - now > 1200)
			// ����һ��ѧ�밴�������¼�
			return LEARN_LONG_BUTTON;
	}
	return 0;
}

// ѧ������
void learn_code(sprayerNvType *nv, unsigned int* learning)
{
	// ��ȡ��ǰ����ʱ���ϵͳ�δ���������ϵ翪ʼ2.54ms�ĸ���
	unsigned int now = get_Timer1_Systemtick();
	unsigned int now1 = 0;
	// ��鵱ǰ�ǲ�����ѧ��ģʽ��
	if (nv->left_mode != LEARN_MODE || nv->right_mode != LEARN_MODE)
		return;
	// ����8s�ȴ�ң���ź�
	// tButton ң�ذ�ť�ļ���
	while(get_Timer1_Systemtick() - now < 400*8 && tButton == 0)
	{
		// ѧ��ģʽ�³���ѧ���
		if (is_learn_button_long_pressed())
		{
			// ��������һ��
			beeper_once();
			// ���֮ǰ��Ե�ң�����ĵ�ַ
			clearAddress();
			break;
		}
		*learning = TRUE;
		Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
	}
	//�յ�ң�ؼ���
	if(tButton == RF_START_BUTTON || tButton == RF_STOP_BUTTON)
	{
		//���ѧ�����
		*learning = FALSE;
		//��������һ��
		beeper_once();
		//����ң�ص�ַ�뵽flash
		saveToFlash();
		//��ȡ��ǰϵͳ�δ�ʱ��
		now1 = get_Timer1_Systemtick();
		//��ʱ1s
		while(get_Timer1_Systemtick() -	now1 < 400)
			Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
		//���Ҫ������¸�ģʽ
		nv->left_mode = LEFT_TIME_EFFECTIVE_MODE;
		nv->right_mode = RIGHT_TIME_EFFECTIVE_MODE;
		//���ң�ذ�����
		tButton = 0;
		return;
	}
	// δ�յ�ң�ذ�����
	*learning = FALSE;
	// �ӕr2s
	Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 800);
	// ��ӛҪ�Mȥ����ģʽ
	nv->left_mode = LEFT_TIME_EFFECTIVE_MODE;
	nv->right_mode = RIGHT_TIME_EFFECTIVE_MODE;
	tButton = 0;
	return;
}

