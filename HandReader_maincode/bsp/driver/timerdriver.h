#ifndef __TIMERDRIVER_H_
#define __TIMERDRIVER_H_

#include "includes.h"
#include "stm32f10x.h"

#define CNTLEN            5    //软件定时器最大个数（必需<15）

#define TIME_MODE_ONCE    0x01 //计数一次模式
#define TIME_MODE_CYCLE   0x02 //循环计数模式

#define FLAGSETBIT(x)     (1<<x)
#define FLAGRESETBIT(x)   (~FLAGSETBIT(x))

typedef void(*timefuc)(void* para);

extern void TIMER2_TimerInit(void);
/*
- 功能名称:  软件定时器设置
- 入口参数:  time_out 溢出时间,prio_timnum带回当前定时器号
             time_fuction 回调函数  para回调参数  time_mode定时计数模式
- 出口参数:  无
- 备注    ： 由main使用
*/
void TimeAStartSet(INT32U time_out,INT8U* prio_timnum,timefuc time_fuction,
                   void* para,INT8U time_mode);
/*
-功能名称:  GetTimeOutFlag 获取定时溢出标志
-入口参数:  当前定软件时器号
-出口参数:  1——发送过溢出 0——未发生溢出
- 备注    ： 由main使用
*/
INT8U GetTimeOutFlag(INT8U prio_timenum);

INT32U GetTimeSet(INT8U prio_timenum);

INT32U GetProTime(INT8U prio_timenum);

/*失能当前定时器计数*/
void TimexDis(INT8U prio_timenum);
/*使能当前定时器计数*/
void TimexEn(INT8U prio_timenum , INT32U time_set);
void TimexTimeSet(INT8U prio_timenum , INT32U time_set);
/*
- 功能名称:  软件定时器中断,完成个软件定时器的定时过程及回调调用
- 备注    ： 由interrupt使用
*/
void IntTimeACnt(INT32U* timeout_flag);

#endif