#include <MS51_16K.H>
#include "nv.h"
#include "iap.h"
#include "common.h"
#include "MS51_16K.h"

void flushFlash()
{
	Erase_APROM(DATA_START_ADDR,128);
	Erase_Verify_APROM(DATA_START_ADDR,128);
	Program_APROM(DATA_START_ADDR,128);
	Program_Verify_APROM(DATA_START_ADDR,128);
	return;
}
