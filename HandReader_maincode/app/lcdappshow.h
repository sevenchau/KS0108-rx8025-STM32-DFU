#ifndef __LCD_APP_SHOW_H
#define __LCD_APP_SHOW_H

#include "window_includes.h"

INT8U* GetTargetAddress(void);   // 获取目标地址
void SetTargetAddress(INT8U* addr); // 设置目标地址

void LCDKS0108_Initialize(void);  // 初始化
void LcdLightOff(void);
void LcdLightOn(void);

void GLCD_ShowLoGo(void);       //开机界面
void MainInterfaceCreate(void*);//主界面

char GetActiveWindow(void);// 获取活动窗体

void EventResponseHander(unsigned int key_value);// 事件响应

INT8U LcdEventEnqueue(BUT_TypeDef* tigger_event); // 数据入队
INT8U LcdEventDelqueue(unsigned char* pwindow_name,char* pbutton_name,int* pvalue_buf,unsigned char* pbuf_len);//数据出队

#endif