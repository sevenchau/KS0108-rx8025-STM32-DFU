#ifndef __KEY_DRIVER_H
#define __KEY_DRIVER_H

#include "stm32f10x.h"
#include "device.h"

typedef void (*fcb)(INT32U para);

void GPIO_KeyConfInit(void);
void KeyScanEnable(void);

INT16U CaptureKeyNum(fcb call_osdelay);

#endif