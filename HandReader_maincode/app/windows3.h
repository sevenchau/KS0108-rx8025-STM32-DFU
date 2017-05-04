#ifndef __WINDOWS3_H
#define __WINDOWS3_H

#include "window_includes.h"

extern unsigned char  OK_Status ;
unsigned char GetOKStatus(void);           // 获取OK状态
void EnterMsgThirdControEventTrigger(void* para);// 输入窗体事件响应

void EnterMsgThirdLevelCreate(void* para);// 构建输入框窗体(弹窗)

void AutoGetTargetAddrControEventTrigger(void* para);// 自动获取目标地址响应事件
void AutoGetTargetAddrCreate(void* para);// 构建自动获取目标地址(弹窗)

#endif 