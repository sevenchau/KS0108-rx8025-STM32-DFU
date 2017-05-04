#ifndef __WINDOW_INCLUDES_H
#define __WINDOW_INCLUDES_H

#include "includes.h"
#include "queue.h"

#include "KS0108.h"
#include "graphic.h"
#include "string.h"
#include "serve.h"

#include "windows1.h"
#include "windows2.h"
#include "windows3.h"
//#include "lcdappshow.h"

#define CONTORL_NUM             5

//主 界面 (一级界面)
#define MAIN_INTERFACE          1
//二级 界面
#define TWO_INTERFACE(x)        (20+x)
//三级界面(弹窗)
#define TREE_INTERFACE(x)       (30+x)

#define BUTTON(x)                (10+x)
#define TEXT(x)                 (30+x)
#define SWITCH(x)               (50+x)

// 控件类型
#define __BUTTON                  1
#define __TEXT                    2
#define __SWITCH                  3

// 控件状态 
#define SELECT                  1 // 选中
#define UNSELECT                2 // 未选中
#define TIGGER_EVNT             3 // 触发事件状态

#define X_MOVE_DISABLE   0xFF
#define Y_MOVE_DISABLE   0xFF

#define ACTIVE_NUM     5

typedef void (*pfuc) (void*);

typedef struct __TIGGER_EVETN
{
  unsigned char active_window;
  char* pactive_button_name;
  unsigned char data_len;
  int date_tcb[BUF_LEN];
}BUT_TypeDef;

typedef union __WORD_TO_TYTE
{
  unsigned int word;
  struct
  {
    unsigned char bytel;
    unsigned char byteh;
  };
}strWordToType;

typedef struct __CONTORLS_STR // 控件
{
  unsigned char x_axis;
  unsigned char y_axis;
  char* cchar_name;           // 字符命名方式
  int* cchar_value;           // 字符值
  unsigned char type;         // 控件属性 button text  switch progress bar(进度条)
  unsigned char status;       // 状态：选中 未选中  触发事件
  unsigned char contorl_addrx;// 控件x地址(行地址 用于游标搜寻)
  unsigned char contorl_addry;// 控件x地址(行地址 用于游标搜寻)
  pfuc control_create;        // 控件构造
  pfuc event_trigger;         // 事件触发
}strContro;

typedef struct __INTERFACE    // 窗体
{
  unsigned char interface_name;    // 窗口名称
  unsigned char cursor_addrx;// 游标 x 地址 行运动
  unsigned char cursor_addry; // 游标 y 地址 列运动
  unsigned char cursor_addr_max;// 游标 运动指针 最大值
  strContro* pctrpoint_tcb;     // 控件指针列表
  pfuc interface_father;       // 父 窗口
  pfuc interface_create;        // 窗体构造函数
}strWindow;

extern const unsigned char gImage_LOGO[282];
extern const unsigned char gImage_yunengyibiao[405];
extern const unsigned char gImage_nanjynybkjyxgs[363];
extern const unsigned char gImage_now_msg[128];
extern const unsigned char gImage_stor_msg[128];
extern const unsigned char gImage_loca_msg[128];
extern const unsigned char gImage_cfg_msg[128];
extern const unsigned char gImage_TIME[66];
extern const unsigned char gImage_buttonframe[44];

extern const unsigned char gImage_sendmsg[128];
extern const unsigned char gImage_password[128];
extern const unsigned char gImage_usercode[128];
extern const unsigned char gImage_datamark[128];

extern strWindow sMainWindow;
extern strContro sMainControBuf[CONTORL_NUM];
extern strWindow HisActinveWindow; // 历史活动窗体
extern strContro HisActiveContro; //  历史活动控件
extern pfuc      fTiggerEventHook[CONTORL_NUM];//触发事件挂钩
extern BUT_TypeDef ActiveTcb[ACTIVE_NUM];
//extern Queue QActive;

#endif