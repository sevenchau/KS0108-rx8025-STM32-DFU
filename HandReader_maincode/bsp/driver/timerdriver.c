#include "timerdriver.h"
#include "string.h"

#if CNTLEN > 31
#error "CNTLEN  不允许超过31!"
#endif

//#define uint8  INT8U     
//#define uint16 INT32U    

typedef INT32U strFlag;
typedef struct __TIME_CNT
{
  INT32U  TimeOut;         //定时时间
  INT32U  TimeCnt;         //计数器
  INT8U   TimeMode;        //定时模式
  strFlag TimeOutFlag;     //标志位
  timefuc TimeFuction;     //回调
  void*   Para;            //参数
}strTime;
static strTime TimeACnt[CNTLEN];
static INT8U UseTimeMaxNum;       //当前以及使用定时器最大编号

void TIMER2_TimerInit(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  TIM_DeInit(TIM2);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_TimeBaseStructure.TIM_Period = 20;                          // 最大计数值,根据设定的分频， 0.001s  
  TIM_TimeBaseStructure.TIM_Prescaler = 3600-1;                  // 分频3600
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;         // 时钟分割  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;     // 计数方向向上计数
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  TIM_SetAutoreload(TIM2, 20);        
  TIM_ARRPreloadConfig(TIM2, ENABLE); 
  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  
  TIM_Cmd(TIM2, ENABLE);
  
  BSP_IntVectSet(BSP_INT_ID_TIM2, BSP_IntHandlerTIM2); 
  BSP_IntPrioSet(BSP_INT_ID_TIM2,1);
  BSP_IntEn(BSP_INT_ID_TIM2);
}

static INT8U GetTimeANum(void);
/*
- 功能名称:  获取一个定时器
- 入口参数:  无
- 出口参数:  定时器号
- 备注    ： 由文件内部使用
*/
static INT8U GetTimeANum(void)
{
  static INT8U init_flag = 0;
  static INT8U time_num = 0;
  
  if(!init_flag)
  {
    init_flag = 1;
    return time_num;
  }
  
  TimeACnt[time_num].TimeOut     = 0;
  TimeACnt[time_num].TimeCnt     = 0;
  TimeACnt[time_num].TimeMode    = 0;
  TimeACnt[time_num].TimeOutFlag = 0;
  TimeACnt[time_num].TimeFuction = (void*)0;
  TimeACnt[time_num].Para        = (void*)0;
  time_num ++;

  return time_num;
}

/*
- 功能名称:  软件定时器设置
- 入口参数:  time_out 溢出时间,prio_timnum带回当前定时器号
             time_fuction 回调函数  para回调参数  time_mode定时计数模式
- 出口参数:  无
- 备注    ： 由main使用
*/
void TimeAStartSet(INT32U time_out,INT8U* prio_timnum,timefuc time_fuction,void* para,INT8U time_mode)
{
  static int initcnt = 0;
  
  if(!initcnt)
  {
    initcnt = 1;
    memset(TimeACnt,0,CNTLEN);
  }
  *prio_timnum = GetTimeANum();
  UseTimeMaxNum = *prio_timnum ;
  TimeACnt[*prio_timnum].TimeOut     = time_out;
  TimeACnt[*prio_timnum].TimeCnt     = time_out;
  TimeACnt[*prio_timnum].TimeMode    = time_mode;
  TimeACnt[*prio_timnum].TimeFuction = time_fuction;
  TimeACnt[*prio_timnum].Para        = para;
}

/*
-功能名称:  GetTimeOutFlag 获取定时溢出标志
-入口参数:  当前定软件时器号
-出口参数:  1——发送过溢出 0——未发生溢出
- 备注    ： 由main使用
*/
INT8U GetTimeOutFlag(INT8U prio_timenum)
{
  
  __disable_irq();
  if(TimeACnt[prio_timenum].TimeOutFlag&FLAGSETBIT(prio_timenum))
  {
    TimeACnt[prio_timenum].TimeOutFlag &= FLAGRESETBIT(prio_timenum);
    __enable_irq();
    return 1;
  }
  __enable_irq();
  return 0;
}

/*
-功能名称:  GetTimeOutSet 获取定时时间
-入口参数:  当前定软件时器号
-出口参数:  定时时间
- 备注    ： 由main使用
*/
INT32U GetTimeSet(INT8U prio_timenum)
{
  return TimeACnt[prio_timenum].TimeOut;
}

/*
-功能名称:  GetProTime 获取当前时间
-入口参数:  当前定软件时器号
-出口参数:  当前时间
- 备注    ： 由main使用
*/
INT32U GetProTime(INT8U prio_timenum)
{
  return TimeACnt[prio_timenum].TimeCnt;
}

/*设置定时时间*/
void TimexTimeSet(INT8U prio_timenum , INT32U time_set)
{
  __disable_irq();
  TimeACnt[prio_timenum].TimeOut = time_set;
  TimeACnt[prio_timenum].TimeCnt = 0;
  __enable_irq();
}

/*失能当前定时器计数*/
void TimexDis(INT8U prio_timenum)
{
  __disable_irq();
  TimeACnt[prio_timenum].TimeCnt = 0;
  __enable_irq();
}
/*使能当前定时器计数*/
void TimexEn(INT8U prio_timenum , INT32U time_set)
{
  __disable_irq();
  if(time_set)
    TimeACnt[prio_timenum].TimeCnt = time_set;
  else
    TimeACnt[prio_timenum].TimeCnt = TimeACnt[prio_timenum].TimeOut;
  __enable_irq();
}

/*
- 功能名称:  软件定时器中断,完成个软件定时器的定时过程及回调调用
- 入口参数:  定时器有溢出标志(高1位)+溢出定时器编号(低31位)
- 出口参数:  无
- 备注    ： 由interrupt使用
*/
void IntTimeACnt(INT32U* timeout_flag)
{
  INT8U i;
  
  for(i=0;i<UseTimeMaxNum+1;i++)
  {
    if((TimeACnt[i].TimeCnt != 0)&&(TimeACnt[i].TimeCnt<TimeACnt[i].TimeOut+1))
    {
      TimeACnt[i].TimeCnt --;
      if(TimeACnt[i].TimeCnt == 0)
      {
        *timeout_flag = (1<<31)+i;
        TimeACnt[i].TimeOutFlag |= FLAGSETBIT(i);
        if(TimeACnt[i].TimeMode == TIME_MODE_CYCLE)//循环计数模式需要重新设置计数
        {
          TimeACnt[i].TimeCnt = TimeACnt[i].TimeOut;
        }
//        TimeACnt[i].TimeFuction(TimeACnt[i].Para);        
      }
    }
  }
}