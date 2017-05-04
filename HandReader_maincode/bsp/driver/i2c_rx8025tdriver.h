#ifndef __I2C_RX8025T_RIVER_H
#define __I2C_RX8025T_RIVER_H

#include "stm32f10x.h"
#include "device.h"

INT8U RX8025T_Init(void);
INT8U ReadTimeReg(INT8U* ptimtcb);
INT8U ReadDateReg(INT8U* pdatetcb);

INT8U WriteDateReg(INT8U* pwdatepcb);
INT8U SetTimeReg(INT8U* pwtimtcb);

#endif