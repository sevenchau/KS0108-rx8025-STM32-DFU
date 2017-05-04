#ifndef  __REBOOT_H__
#define  __REBOOT_H__

#include "stm32f10x.h"
#include "device.h"
#include "includes.h"

void  reboot (void);
void Wdt_Init(void);
void Kick_Dog(void);

void WKUP_Init(void);
u8 IntCheckWakeUp(void);
void Sys_Enter_Standby(void);
void StandbyIntEnable(void);

#endif
