#ifndef SYSTEMTICK_H
#define SYSTEMTICK_H 
#include "MS51_16K.H"

#define RF_START_BUTTON 0x0c
#define RF_STOP_BUTTON 0x10
#define WATER_IS_SHORT 0x20
#define RF_COLLABORATE_BUTTON 0x40

void start_Timer1_Systemtick (void);
uint32_t get_Timer1_Systemtick(void);
void enable_Timer2_IC2 (void);
unsigned char Timer1_Delay2Dot54ms_Unblocked(uint32_t start, uint32_t nms);
unsigned char Timer1_Delay2Dot54ms_blocked(uint32_t start, uint32_t nms);
unsigned char decode(void);
unsigned char match(void);
unsigned char oneFrameReceived();
unsigned char getRfButtonCode();
unsigned char saveToFlash();

#endif