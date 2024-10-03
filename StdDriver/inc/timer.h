#ifndef _TIMER_H_
#define _TIMER_H_

#include "MS51_16K.H"

void Timer0_Delay100us(UINT32 u32CNT);	// timer0 occupied for system tick
void Timer0_Delay1ms(UINT32 u32CNT);	  // timer0 occupied for system tick
void Timer1_Delay10ms(UINT32 u32CNT);
void Timer2_Delay500us(unsigned char u8CNT);
void Timer2_Delay1ms(unsigned char u8CNT);
void Timer3_Delay100ms(UINT32 u32CNT);

void Timer0_Delay40ms(UINT32 u32CNT);	  // timer0 occupied for system tick
void Timer3_Delay10us(UINT32 u32CNT);

#endif