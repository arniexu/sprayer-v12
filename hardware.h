#ifndef _BOARD_H_
#define _BOARD_H_

#include "MS51_16K.h"
#include "Function_Define_MS51_16K.h"

/* used for production board */
// mcu 3 p0.7 <-------------------> production ADC
// mcu 2 p0.6 <-------------------> ````````

/* used for dev board */
// mcu 3 p0.7 <-------------------> devboard A2
// mcu 2 p0.6 <-------------------> devboard A1

/* used for printf on devboard, no printf on production board */
#ifdef DEVBOARD
#define UART1_RX_PIN_INIT   { P07_QUASI_MODE; }
#define UART1_TX_PIN_INIT   { P06_QUASI_MODE; }
#else
#define UART1_RX_PIN_INIT
#define UART1_TX_PIN_INIT
#endif


#endif