#ifndef __SERVE_H
#define __SERVE_H

#include "stm32f10x.h"
#include "includes.h"
#include "queue.h"
#include "protocol.h"
#include "string.h"
#include "serve.h"
#include  <reboot.h>

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define  APP_TASK_KEY_SCAN_PRIO                         4 //键盘扫描任务优先级
#define  APP_TASK_START_PRIO                      	5 //开始任务优先级 发送无线数据
#define  APP_LCD_SHOWRF_CARD_PRIO                       6 //接收指示无线数据
#define  APP_TASK_RFFRESH_EVENT_PRIO                    7 //更新LCD当前需要跟新的事件任务优先级
#define  APP_TASK_FEEDDOG_PRIO                          8 //喂狗任务优先级

#define  APP_TASK_INDICATE_PRIO				9 //声光指示任务优先级

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_RFFRESH_EVENT_STK_SIZE                     192
#define  APP_TASK_START_STK_SIZE                       	192
#define  APP_TASK_FEEDDOG_STK_SIZE                      192
#define  APP_TASK_KEY_SCAN_STK_SIZE                     192
#define  APP_TASK_INDICATESTK_SIZE                      192
#define  APP_LCD_SHOW_RFMSG_STK_SIZE                    192

extern OS_STK  App_TaskFeedDogStk[APP_TASK_FEEDDOG_STK_SIZE];
extern OS_STK  App_TaskStartStk[APP_TASK_START_STK_SIZE];
extern OS_STK  App_KeyScanStk[APP_TASK_KEY_SCAN_STK_SIZE];
extern OS_STK  App_TaskIndicateStk[APP_TASK_INDICATESTK_SIZE];
extern OS_STK  App_RefreshEventStk[APP_RFFRESH_EVENT_STK_SIZE];
extern OS_STK  App_LcdShowRFStk[APP_LCD_SHOW_RFMSG_STK_SIZE];

#define SEM_MORMAL_NUM       10
typedef enum  
{
  UpdateEventSem = 0,
  KeyStandbySem,
  RfRxIntSem,
  StandbySem,
  AutoGetAddrSem,
}SemNormalEnum;

#define SEM_MUTEX_NUM        10
typedef enum 
{
  LCDRefreshMutexSem     = 0,
}SemMutexEnum;

extern OS_EVENT *SemNormalTcb[SEM_MORMAL_NUM];
extern OS_EVENT *SemMutexTcb[SEM_MUTEX_NUM];

void OSSemMutexCreate(SemMutexEnum mutex_sem);   // 创建互斥 信号量
void OSSemNomarlCreate(SemNormalEnum normal_sem); //  创建普通信号量

void  App_TaskFeedDog (void *p_arg);
void  App_TaskStart (void *p_arg);
void  App_KeyScanTask (void *p_arg);
void  App_TaskIndicate (void *p_arg);
void  App_RefreshEventTask(void);
void  App_LcdShowRFMsgTask (void *p_arg);

void  TimeDelay(INT16U cnt);

void WindowTaskRefreshEvent(char* string,unsigned char y_axis,unsigned char window);
void PrintfStringToWindowEvent(char* string,unsigned char x_axis,unsigned char y_axis);

#endif