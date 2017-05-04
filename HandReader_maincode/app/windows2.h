#ifndef __WINDOWS2_H
#define __WINDOWS2_H

#include "window_includes.h"

void MainSecondLevelUnselectEvent(void *para);/*未选中控件（创建）*/
void MainSecondLevelSelectEvent(void *para);/*选中控件*/

void MainSecondLevelControlEventTrigger(void* para);/*控件响应*/

void CurSecondLevelInterfaceCreate(void* para);//创建当前信息窗体

void SetStringNumberValue(char* string);       // 设置控件显示值
char GetStringNumberValue(void);               // 获取控件显示值
void StorySecondLevelInterfaceCreate(void* para);//创建历史信息窗体

void LocalSecondLevelInterfaceCreate(void* para);//创建本地信息窗体

void CfgSecondLevelInterfaceCreate(void* para);// 创建配置信息窗体

#endif 