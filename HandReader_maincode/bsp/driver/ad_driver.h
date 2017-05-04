#ifndef __AD_DRIVER_H
#define __AD_DRIVER_H

#include "stm32f10x.h"
#include "device.h"

void ADC1_Config(void);
void CalculateSampleValues(float* ppower_electric,float* prtc_electric);
void FloatChangeToInt(float value,uint16_t* integer, uint16_t* decimal);

#endif