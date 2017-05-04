#include "windows2.h"
#include "lcdappshow.h"
/*
********************************************************************************
                             二级主界面 - 当前信息窗体
********************************************************************************
*/
void MainSecondLevelFrameCreate(void* para) // 界面框架 如参数 para = "CUR" 
{
  strContro* tmp_para;
  
  tmp_para = (strContro*)para;
  
  GLCD_Line(0, 1*8-5,128,1*8-5); 
  
  GLCD_GoTo(0,1);
  GLCD_WriteString("TOAL(0):");//4294967296.00
  
  GLCD_GoTo(0,2);
  GLCD_WriteString("RATE(1):");
  
  GLCD_GoTo(0,3);
  GLCD_WriteString("RATE(2):");
  
  GLCD_GoTo(0,4);
  GLCD_WriteString("RATE(3):");
  
  GLCD_GoTo(0,5);
  GLCD_WriteString("RATE(4):");
  
  GLCD_GoTo(6*tmp_para->x_axis,tmp_para->y_axis);
  GLCD_WriteString(tmp_para->cchar_name);
  
  GLCD_Line(0, 7*8-5,128,7*8-5);
  
  IcoInit();
}

/*未选中控件（创建）*/
void MainSecondLevelUnselectEvent(void *para)
{
  strContro* tmp_para;
  strWindow* active_window;
  
  tmp_para = (strContro*)para;
  active_window  = &sMainWindow;
  
  if(tmp_para->x_axis > 21) return;
  if(tmp_para->y_axis > 7 ) return;
  
  if(TREE_INTERFACE(2) != active_window->interface_name)
  {
//    GLCD_RectangleDotted(6*tmp_para->x_axis-2,tmp_para->y_axis*8-2,16,10);
    if((0 == active_window->cursor_addrx) && (0 == active_window->cursor_addry))
    {
      GLCD_GoTo(6*tmp_para->x_axis,tmp_para->y_axis);
      GLCD_WriteString(tmp_para->cchar_name); 
      GLCD_Rectangle(6*tmp_para->x_axis-2,tmp_para->y_axis*8-2,16,10); //GLCD_RectangleDotted  
    }
    else
    {
//      GLCD_RectangleDotted(6*tmp_para->x_axis-2,tmp_para->y_axis*8-2,16,10);
//      GLCD_RectangleAntiColor(6*tmp_para->x_axis-2,tmp_para->y_axis*8-2,16,10);
      GLCD_RectangleAntiColor(6*tmp_para->x_axis-3,tmp_para->y_axis*8-1,18,8);
      GLCD_RectangleAntiColor(6*tmp_para->x_axis-1,tmp_para->y_axis*8-1,14,8); 
      GLCD_GoTo(6*tmp_para->x_axis,tmp_para->y_axis);
      GLCD_WriteString(tmp_para->cchar_name); 
      GLCD_Rectangle(6*tmp_para->x_axis-2,tmp_para->y_axis*8-2,16,10); //GLCD_RectangleDotted  
    }
//    GLCD_Rectangle(6*tmp_para->x_axis-2,tmp_para->y_axis*8-2,16,10); //GLCD_RectangleDotted   
  }
  else
  {
    GLCD_GoTo(6*tmp_para->x_axis,tmp_para->y_axis);
    GLCD_WriteString(tmp_para->cchar_name); 
  }
}
/*选中控件*/
void MainSecondLevelSelectEvent(void *para)
{
  strContro* tmp_para;
  strWindow* active_window;
  
  tmp_para = (strContro*)para;
  active_window  = &sMainWindow;
  
  if(tmp_para->x_axis > 21) return;
  if(tmp_para->y_axis > 7 ) return;
  
  if(TREE_INTERFACE(2) != active_window->interface_name)
  {
    GLCD_Rectangle(6*tmp_para->x_axis-2,tmp_para->y_axis*8-2,16,10);
    GLCD_Rectangle(6*tmp_para->x_axis-3,tmp_para->y_axis*8-1,18,8);
    GLCD_Rectangle(6*tmp_para->x_axis-1,tmp_para->y_axis*8-1,14,8);
  }
  GLCD_GoTo(6*tmp_para->x_axis,tmp_para->y_axis);
  GLCD_WriteAntiColorString(tmp_para->cchar_name);  
}

extern int InputBuf[BUF_LEN];
BUT_TypeDef tigger_event;
/*控件响应*/
void MainSecondLevelControlEventTrigger(void* para)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif 
  strContro* active_contorl;
  strWindow* active_window;
  
  active_contorl = (strContro*)para; // 活动控件
  active_window  = &sMainWindow;
  
  switch(active_contorl->status)
  {
  case SELECT:
  	MainSecondLevelSelectEvent(para);
    break;
  case UNSELECT:
  	MainSecondLevelUnselectEvent(para);
    break;
  case TIGGER_EVNT:
    if(GetLcdData() == (void*)0)
    {
      if((TWO_INTERFACE(1) == active_window->interface_name)||(TWO_INTERFACE(2) == active_window->interface_name))
      {
        if((1 == active_contorl->x_axis)&&(TWO_INTERFACE(2) == active_window->interface_name))
        {
          break;
        }
        GLCD_TextGoTo(8, 1);
        GLCD_WriteString("             ");
        GLCD_TextGoTo(8, 2);
        GLCD_WriteString("             ");
        GLCD_TextGoTo(8, 3);
        GLCD_WriteString("             ");
        GLCD_TextGoTo(8, 4);
        GLCD_WriteString("             ");
        GLCD_TextGoTo(8, 5);
        GLCD_WriteString("             ");
        InputBuf[1] = '0';
        InputBuf[2] = '1';
        InputBuf[3] = '2';
        InputBuf[4] = '3';
        InputBuf[5] = '4';
      }
      else
      {
        break;
      }
    }
    tigger_event.active_window = active_window->interface_name;
    tigger_event.pactive_button_name = active_contorl->cchar_name;
    memset(&tigger_event.date_tcb,-1,sizeof(BUT_TypeDef));
    CPU_CRITICAL_ENTER();
    for(char i=0;i<BUF_LEN-1;i++)
    {
      tigger_event.date_tcb[i] = *(GetLcdData()+i+1);
      if(-1 == *(GetLcdData()+i+1)) break;
    }
    CPU_CRITICAL_EXIT();
    //    memcpy(tigger_event.date_tcb,GetLcdData(),BUF_LEN);
    
    if(LcdEventEnqueue(&tigger_event))
    {
      OSSemPost (SemNormalTcb[UpdateEventSem]);
      IconOper(WARING,ICO_EN,5);
      break;
    }
    IconOper(HOME,ICO_EN,5);
    OSSemPost (SemNormalTcb[UpdateEventSem]);
    ClearLcdStorData();
    break;
  default:
    break;
  }
}

/*
二级界面当前信息窗体
*/
void CurSecondLevelInterfaceCreate(void* para)
{
  unsigned char i;
  
  strWindow* psMainWindow;
  strContro*    pstrContro;
  
  GLCD_ClearScreen();
  
  memset(&sMainWindow,0,sizeof(sMainWindow));
  memset(sMainControBuf,0,sizeof(sMainControBuf));
  
  // 界面初始化
  psMainWindow = &sMainWindow;
  psMainWindow->interface_name = TWO_INTERFACE(1);
  psMainWindow->interface_father = MainInterfaceCreate;
  psMainWindow->pctrpoint_tcb = sMainControBuf;
  psMainWindow->interface_create = MainSecondLevelFrameCreate;
  psMainWindow->cursor_addrx = 0;
  psMainWindow->cursor_addry = 0;
  psMainWindow->cursor_addr_max = 3;
   
  pstrContro = &sMainControBuf[0];
  pstrContro->x_axis = 0;
  pstrContro->y_axis = 7;
  pstrContro->cchar_name   = "CUR";
  psMainWindow->interface_create((void*)pstrContro);//创建主界面
  
  // 控件布局及初始化 (1,x) (2,x) (3,x) (4,x)
//  pstrContro = &sMainControBuf[0];
//  pstrContro->x_axis             = 6;
//  pstrContro->y_axis             = 7;
//  pstrContro->cchar_name         = "__";
//  pstrContro->cchar_value        = (void*)0;
//  pstrContro->contorl_addrx      = 1;
//  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
//  pstrContro->type               = __BUTTON;
//  pstrContro->status             = UNSELECT;
//  pstrContro->control_create     = MainSecondLevelUnselectEvent;
//  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  pstrContro = &sMainControBuf[0];
  pstrContro->x_axis             = 10;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "AP";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 1;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __BUTTON;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  pstrContro = &sMainControBuf[1];
  pstrContro->x_axis             = 14;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "+P";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 2;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __TEXT;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  pstrContro = &sMainControBuf[2];
  pstrContro->x_axis             = 18;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "-P";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 3;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __BUTTON;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  // 创建控件
  for(i=0;i<psMainWindow->cursor_addr_max;i++)
  {
    psMainWindow->pctrpoint_tcb[i].control_create((void*)&psMainWindow->pctrpoint_tcb[i]);
  }
}

/*
********************************************************************************
                             end of 当前信息窗体
********************************************************************************
*/

char StringNu[4]={0};

void SetStringNumberValue(char* string)
{
  StringNu[0] = string[0];
  StringNu[1] = string[1];
  
  GLCD_TextGoTo(6,7);
  GLCD_WriteAntiColorString(StringNu);  
}

char GetStringNumberValue(void)
{
  static char init = 0;
  
  if(!init)
  { 
    init = 1;
    memcpy(StringNu,"__",4);
  }
  if((0x5f == StringNu[0]) && (0x5f == StringNu[1]))
  {
    StringNu[0] = '0';
    StringNu[1] = '1';
    return ((StringNu[0]-0x30)*10+(StringNu[1]-0x30));
  }
  if(0x20 == StringNu[1])
  {
    return StringNu[0]-0x30;
  }
  return ((StringNu[0]-0x30)*10+(StringNu[1]-0x30));
}


/*
二级界面历史信息窗体
*/
void StorySecondLevelInterfaceCreate(void* para)
{
  unsigned char i;
  
  strWindow* psMainWindow;
  strContro*    pstrContro;
  
  GetStringNumberValue();
  
  GLCD_ClearScreen();
  
  memset(&sMainWindow,0,sizeof(sMainWindow));
  memset(sMainControBuf,0,sizeof(sMainControBuf));
  
  // 界面初始化
  psMainWindow = &sMainWindow;
  psMainWindow->interface_name = TWO_INTERFACE(2);
  psMainWindow->interface_father = MainInterfaceCreate;
  psMainWindow->pctrpoint_tcb = sMainControBuf;
  psMainWindow->interface_create = MainSecondLevelFrameCreate;
  psMainWindow->cursor_addrx = 0;
  psMainWindow->cursor_addry = 0;
  psMainWindow->cursor_addr_max = 4;
   
  pstrContro = &sMainControBuf[0];
  pstrContro->x_axis = 0;
  pstrContro->y_axis = 7;
  pstrContro->cchar_name   = "HIS";
  psMainWindow->interface_create((void*)pstrContro);//创建主界面
  
  // 控件布局及初始化 (1,x) (2,x) (3,x) (4,x)
  pstrContro = &sMainControBuf[0];
  pstrContro->x_axis             = 6;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = StringNu;//"__";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 1;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __BUTTON;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  pstrContro = &sMainControBuf[1];
  pstrContro->x_axis             = 10;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "AP";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 2;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __BUTTON;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  pstrContro = &sMainControBuf[2];
  pstrContro->x_axis             = 14;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "+P";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 3;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __TEXT;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  pstrContro = &sMainControBuf[3];
  pstrContro->x_axis             = 18;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "-P";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 4;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __BUTTON;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  // 创建控件
  for(i=0;i<psMainWindow->cursor_addr_max;i++)
  {
    psMainWindow->pctrpoint_tcb[i].control_create((void*)&psMainWindow->pctrpoint_tcb[i]);
  }
}

/*
********************************************************************************
                             end of 历史信息窗体
********************************************************************************
*/

// 本地信息窗体
void LocaSecondLevelFrameCreate(void* para)
{
  GLCD_GoTo(0,0);
  GLCD_WriteAntiColorString         ("                     ");//GLCD_WriteAntiColorString GLCD_WriteString
  GLCD_Line(0, 2*8-6,128,2*8-6);
  
  GLCD_GoTo(0,2);
  GLCD_WriteString("VER :");
  
  GLCD_GoTo(0,3);
  GLCD_WriteString("ADDR:");
  
  GLCD_GoTo(0,4);
  GLCD_WriteString("VBAT:");
  
  GLCD_GoTo(0,5);
  GLCD_WriteString("VRTC:");
  
  GLCD_Line(0, 7*8-3,128,7*8-3);
  GLCD_GoTo(0,7);
  GLCD_WriteAntiColorString         ("                     ");
  
  IcoInit();
}

void LocalSecondLevelInterfaceCreate(void* para)
{
  strWindow* psMainWindow;
  
  GLCD_ClearScreen();
  
  memset(&sMainWindow,0,sizeof(sMainWindow));
  
  // 界面初始化
  psMainWindow = &sMainWindow;
  psMainWindow->interface_name = TWO_INTERFACE(3);
  psMainWindow->interface_father = MainInterfaceCreate;
  psMainWindow->pctrpoint_tcb = sMainControBuf;
  psMainWindow->interface_create = LocaSecondLevelFrameCreate;
  psMainWindow->cursor_addrx = 0;
  psMainWindow->cursor_addry = 0;
  psMainWindow->cursor_addr_max = 0;
  
  psMainWindow->interface_create((void*)0);
}
/*
*******************************************************************************
*************************end of 本地信息窗体***********************************
*******************************************************************************
*/

//配置信息窗体
void CfgMsgSecondLevelFrameCreate(void* para)
{
  strContro* tmp_para;
  
  tmp_para = (strContro*)para;
  
  GLCD_GoTo(0,0);
  GLCD_Line(0, 4,128,4);
  
  GLCD_GoTo(0,1);
  GLCD_WriteString("DATE :");
  
  GLCD_GoTo(0,2);
  GLCD_WriteString("TIME :");
  
  GLCD_GoTo(0,3);
  GLCD_WriteString("BATIM:");
  
  GLCD_GoTo(0,4);
  GLCD_WriteString("SHDOW:");
  
  GLCD_GoTo(0,5);
  GLCD_WriteString("ADDR :");
  
  GLCD_GoTo(6*tmp_para->x_axis,tmp_para->y_axis);
  GLCD_WriteString(tmp_para->cchar_name);
  
//  GLCD_Line(0, 7*8-3,128,7*8-3);
//  GLCD_GoTo(0,7);
  GLCD_Line(0, 7*8-5,128,7*8-5);
  
  IcoInit();
}

void CfgSecondLevelInterfaceCreate(void* para)
{
  unsigned char i;
  
  strWindow* psMainWindow;
  strContro*    pstrContro;
  
  GLCD_ClearScreen();
  
  memset(&sMainWindow,0,sizeof(sMainWindow));
  memset(sMainControBuf,0,sizeof(sMainControBuf));
  
  // 界面初始化
  psMainWindow = &sMainWindow;
  psMainWindow->interface_name = TWO_INTERFACE(4);
  psMainWindow->interface_father = MainInterfaceCreate;
  psMainWindow->pctrpoint_tcb = sMainControBuf;
  psMainWindow->interface_create = CfgMsgSecondLevelFrameCreate;
  psMainWindow->cursor_addrx = 0;
  psMainWindow->cursor_addry = 0;
  psMainWindow->cursor_addr_max = 5;
  
  pstrContro = &sMainControBuf[0];
  pstrContro->x_axis = 0;
  pstrContro->y_axis = 7;
  pstrContro->cchar_name   = " ";
  psMainWindow->interface_create((void*)pstrContro);//创建主界面
//  psMainWindow->interface_create((void*)0);
  
  // 控件布局及初始化 (1,x) (2,x) (3,x) (4,x)
  pstrContro = &sMainControBuf[0];
  pstrContro->x_axis             = 1;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "DA";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 1;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __BUTTON;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  pstrContro = &sMainControBuf[1];
  pstrContro->x_axis             = 5;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "TI";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 2;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __BUTTON;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  pstrContro = &sMainControBuf[2];
  pstrContro->x_axis             = 9;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "BA";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 3;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __TEXT;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  pstrContro = &sMainControBuf[3];
  pstrContro->x_axis             = 13;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "SH";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 4;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __BUTTON;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  pstrContro = &sMainControBuf[4];
  pstrContro->x_axis             = 17;
  pstrContro->y_axis             = 7;
  pstrContro->cchar_name         = "AD";
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 5;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __BUTTON;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = MainSecondLevelUnselectEvent;
  pstrContro->event_trigger      = MainSecondLevelControlEventTrigger;
  
  // 创建控件
  for(i=0;i<psMainWindow->cursor_addr_max;i++)
  {
    psMainWindow->pctrpoint_tcb[i].control_create((void*)&psMainWindow->pctrpoint_tcb[i]);
  }
}
/*
*******************************************************************************
*************************end of 配置信息窗体***********************************
*******************************************************************************
*/
