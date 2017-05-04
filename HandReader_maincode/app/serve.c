/*
-  文件名称: serve.c
-  文件描述: 主要完成响应配置任务
-  版本日期: 2012.04      v1.1
-  作者署名: __Rangers
*/

#include "serve.h"
//#include "KS0108.h"
#include "window_includes.h"
#include "lcdappshow.h"

OS_STK  App_RefreshEventStk[APP_RFFRESH_EVENT_STK_SIZE];
OS_STK  App_TaskFeedDogStk[APP_TASK_FEEDDOG_STK_SIZE];
OS_STK  App_TaskStartStk[APP_TASK_START_STK_SIZE];
OS_STK  App_KeyScanStk[APP_TASK_KEY_SCAN_STK_SIZE];
OS_STK  App_TaskIndicateStk[APP_TASK_INDICATESTK_SIZE];
OS_STK  App_LcdShowRFStk[APP_LCD_SHOW_RFMSG_STK_SIZE];

OS_EVENT *SemNormalTcb[SEM_MORMAL_NUM];
OS_EVENT *SemMutexTcb[SEM_MUTEX_NUM];

void OSSemNomarlCreate(SemNormalEnum normal_sem) //  创建普通信号量
{
  SemNormalTcb[normal_sem] = OSSemCreate(0);
  if(SemNormalTcb[normal_sem] == (OS_EVENT *)0)
  {
    while(1){}
  }
}

void OSSemMutexCreate(SemMutexEnum mutex_sem)   // 创建互斥 信号量
{
  SemMutexTcb[mutex_sem] = OSSemCreate(0);
  if(SemMutexTcb[mutex_sem] == (OS_EVENT *)0)
  {
    while(1){}
  }
  while(OS_ERR_NONE != OSSemPost (SemMutexTcb[mutex_sem]));
}

void TimeDelay(INT16U cnt)
{
  INT32U time = 10000*cnt;
  while (time --);
}

void WindowTaskRefreshEvent(char* string,unsigned char y_axis,unsigned char window)
{
  extern strWindow HisActinveWindow;
  unsigned char active_window;
  
  active_window = GetActiveWindow();
  if((window != active_window)&&(TREE_INTERFACE(1) != active_window)&&(TREE_INTERFACE(2) != active_window)) return;
  
  if((TREE_INTERFACE(1) == active_window)||(TREE_INTERFACE(2) == active_window))//弹窗需要刷历史窗体
  {
    active_window = HisActinveWindow.interface_name;
  }
  
  switch(active_window)
  {
  case MAIN_INTERFACE:
    GLCD_TextGoTo(8, y_axis);
    GLCD_WriteString(string);//"2014/08/06"
    break;
  case TWO_INTERFACE(1):
    GLCD_TextGoTo(8, y_axis);
    GLCD_WriteString(string);
    break;
  case TWO_INTERFACE(2):
    GLCD_TextGoTo(8, y_axis);
    GLCD_WriteString(string);
    break;
  case TWO_INTERFACE(3):
    GLCD_TextGoTo(8,2);
    GLCD_WriteString("01.00.14");
    if((TREE_INTERFACE(1) == GetActiveWindow())||(TREE_INTERFACE(2) == GetActiveWindow()))//弹窗需要刷历史窗体
    {
      break;
    }
    GLCD_TextGoTo(8,y_axis);
    GLCD_WriteString(string);
    break;
  case TWO_INTERFACE(4):
    GLCD_TextGoTo(8, y_axis);
    GLCD_WriteString(string);//"2014/08/06"
    break;
  default:
    break;
  }
}

void PrintfStringToWindowEvent(char* string,unsigned char x_axis,unsigned char y_axis)
{
  GLCD_TextGoTo(x_axis, y_axis);
  GLCD_WriteString(string);
}