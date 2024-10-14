#include "systick.h"
#include "button.h"
#include "eeprom.h"
#include "nv.h"
#include <RTX51TNY.h>

/* if define TIMER0_FSYS_DIV12, timer = (0xFFFF-0x1000)*12/24MHz = 36.58ms */
/* if define TIMER0_FSYS, timer = (0xFFFF-0x1000)/24MHz = 2.54ms */
#define TH1_INIT        0x10
#define TL1_INIT        0x00
#define MAX_TRIGGER     80

extern unsigned int learning;
static unsigned int g_u32SystemTick = 0;
struct rfCode {
	unsigned char level;
  unsigned int duration;
};
// 保存遥控器发过来的键码
unsigned int tButton = 0;
static xdata struct rfCode g_codes[MAX_TRIGGER] = {0};
//@TODO: save into flash
static xdata unsigned char g_addresses[MAX_TRIGGER/2] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1};
static xdata unsigned char g_decodes[MAX_TRIGGER/2] = {0};
static unsigned int i = 0;
static unsigned int i_decodes = 0;
static unsigned int val = 0;
static unsigned int g_u32Timer2Overflow = 0;

static void user_systemtick_callback(void)
{
	g_u32SystemTick ++;
}

/************************************************************************************************************
*    TIMER 0 interrupt subroutine
************************************************************************************************************/
void Timer1_ISR (void) interrupt 3  //interrupt address is 0x000B
{
    _push_(SFRS);
	
	TH1 = TH1_INIT;
    TL1 = TL1_INIT;    
    TF1 = 0;
    user_systemtick_callback();

    _pop_(SFRS);
}

void Timer2_ISR (void) interrupt 5  //interrupt address is 0x000B
{
    _push_(SFRS);
	
	//TH2 = 0;
    //TL2 = 0;    
    //TF2 = 0;
    g_u32Timer2Overflow ++;
	clr_T2CON_TF2;
    _pop_(SFRS);
}

/************************************************************************************************************
*    start_systemtick 
************************************************************************************************************/
void start_Timer1_SystemTick (void)
{  
    ENABLE_TIMER1_MODE1;
    TIMER1_FSYS;

    TH1 = TH1_INIT;
    TL1 = TL1_INIT;
    
    ENABLE_TIMER1_INTERRUPT;                       //enable Timer0 interrupt
    ENABLE_GLOBAL_INTERRUPT;                       //enable interrupts
  
    set_TCON_TR1;                                  //Timer0 run
}

/************************************************************************************************************/
/*    输入捕获中断监听P10                                                                   */
/************************************************************************************************************/
void Capture_ISR (void) interrupt 12
{
    _push_(SFRS);
	clr_CAPCON0_CAPF2;                  /* clear capture0 interrupt flag  */
	val = C2L + (C2H * 0x100) + 0xffff*g_u32Timer2Overflow;
	g_u32Timer2Overflow = 0;
	g_codes[i].duration = val; 							/* For capture mode CxL/CxH with data capture from I/O pin*/
	g_codes[i].level = !P10;	
	if (i == MAX_TRIGGER - 1)
	{
		i = 0;
		i_decodes = 0;
		//clr_T2CON_TR2;                       //Triger Timer2
		decode();
		memset(&g_codes, 0, sizeof(struct rfCode)*MAX_TRIGGER);
	}
	i ++;
	C2L = 0;
	C2H = 0;
    _pop_(SFRS);
}
/************************************************************************************************************/
/*    Main function                                                                                         */
/************************************************************************************************************/
void enable_Timer2_IC2 (void)
{
    P10_QUASI_MODE;
	TIMER2_CAP2_Capture_Mode;
    IC2_P10_CAP3_BOTHEDGE_CAPTURE;
	T2MOD |= 0x20;											 // prescaler is 16	
    set_EIE_ECAP;                        //Enable Capture interrupt
	set_EIE_ET2;												 //Enable Timer2 Interrupt
    set_T2CON_TR2;                       //Triger Timer2
    ENABLE_GLOBAL_INTERRUPT;
	Read_DATAFLASH_ARRAY(DATA_START_ADDR+ REMOTE_CONTROLLER_ADDRESS_OFFSET, (unsigned char *)g_addresses, 19);
}

uint32_t get_Timer1_Systemtick(void)
{
	return g_u32SystemTick;
}

unsigned char Timer1_Delay2Dot54ms_Unblocked(uint32_t start, uint32_t nms)
{
	while (get_Timer1_Systemtick() - start < nms){
		Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 1);
		os_switch_task();
	}
	return 1;
}

unsigned char Timer1_Delay2Dot54ms_blocked(uint32_t start, uint32_t nms)
{
	while (get_Timer1_Systemtick() - start < nms);
	return 1;
}

unsigned char decode(void)
{
	unsigned int j = 0; // index to loop over g_codes
	unsigned int major = 0;
	unsigned int minor = 0;
	unsigned int found = FALSE;
	unsigned int head = 0;
	for(j = 0; j<MAX_TRIGGER; j++)
	{
		if (g_codes[j].level == 0
			&& g_codes[j].duration > 0x2800)
		{
			head = j;
			found ++;
		}
		if(found >= 1 && found < 2)
		{
			if (g_codes[j].level == 1
				&& j < MAX_TRIGGER - 1
			  && j%2 == (head+1)%2)
			{
				if(g_codes[j+1].duration > g_codes[j].duration)
				{
					major = g_codes[j+1].duration/g_codes[j].duration;
					minor = g_codes[j+1].duration%g_codes[j].duration;
					if (minor > g_codes[j].duration/2)
						major += 1;
					if (major == 3)
					{
						g_decodes[i_decodes++]=0;
					}
				}
				else if(g_codes[j+1].duration < g_codes[j].duration)
				{
					major = g_codes[j].duration/g_codes[j+1].duration;
					minor = g_codes[j].duration%g_codes[j+1].duration;
					if (minor > g_codes[j+1].duration/2)
						major += 1;
					if (major == 3)
					{
						g_decodes[i_decodes++]=1;
					}
				}
			}
		}
	}
	if (i_decodes == 24){
		tButton = getRfButtonCode();
		if (tButton == RF_START_BUTTON || tButton == RF_STOP_BUTTON)
			/*saveToFlash()*/;
		else if (match())
		{
			tButton = getRfButtonCode();
		}
	}
	return i_decodes; 
}

unsigned char saveToFlash()
{
	if (i_decodes == 0x18)
	{
		memcpy(g_addresses, g_decodes, 19);
		Write_DATAFLASH_ARRAY(DATA_START_ADDR + REMOTE_CONTROLLER_ADDRESS_OFFSET, 
								(unsigned char *)g_addresses, 19);
		return TRUE;
	}
	return FALSE;
}

unsigned char clearAddress()
{
	memset(g_addresses, 0, 19);
	Write_DATAFLASH_ARRAY(DATA_START_ADDR + REMOTE_CONTROLLER_ADDRESS_OFFSET, 
							(unsigned char *)g_addresses, 19);
	return TRUE;
}

unsigned char match(void)
{
	if(memcmp(g_addresses, g_decodes, 18) == 0)
		return TRUE;
	return FALSE;
}

unsigned char getRfButtonCode()
{
	unsigned char index = 0;
	unsigned char val = 0;
	if (oneFrameReceived() && (match() || learning))
	{
		for(index = 5; index > 0; index --)
		{
			if (g_decodes[24-index])
				val |= (0x01 << (index - 1));
			else
				val &= ~(0x01 << (index - 1));
		}
	}
	return val;
}

unsigned char oneFrameReceived()
{
	return i_decodes == 24;
}

