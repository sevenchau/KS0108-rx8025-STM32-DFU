#ifndef __WINDOWS1_H
#define __WINDOWS1_H

#include "window_includes.h"

void AppCursorGlink(void);/*光标闪烁*/
void GLCD_ClearScreen(void);// 清屏
void GLCD_ShowLoGo(void);/*显示公司logo*/

void MainControlEventTrigger(void* para);/*主界面控件事件*/
void MainInterfaceCreate(void* para);/*创建主窗口*/

void StorUserName(INT8U* pvalue,INT8U len);
void ChangeInfomationFromInput(void); // 刷新窗体显示值
void SendMsgControlEventTrigger(void* para);/*控件响应*/
void SendMsgWindowCreate(void* para);/*创建主窗口*/
void GetSendMsgWindowData(char* puser_name,char* ppass_word,char* pdata_mark,char* pdata,char* pdlen);

#endif 