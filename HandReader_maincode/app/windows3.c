#include "windows3.h"
#include "lcdappshow.h"
/*
*******************************************************************************
******************************** 信息录入窗体***********************************
*******************************************************************************
*/
unsigned char  OK_Status = 0;

void EnterMsgThirdLevelFrameCreate(void* para)
{
  GLCD_ClearScreenLiney(3);
  GLCD_ClearScreenLiney(4);
  GLCD_ClearScreenLiney(5);
  GLCD_ClearScreenLiney(6);
  
  GLCD_Rectangle(3,3*8+6,125,3*8-4);
//  GLCD_Line(6,31,125,31);
//  GLCD_Rectangle(4,3*8+7,123,9);
//  GLCD_Rectangle(5,3*8+7,121,9);
  
  IcoInit();
}

void EnterMsgThirdControCreate(void* para)
{
  strContro* tmp_para;
  
  tmp_para = (strContro*)para;
  
  GLCD_TextGoTo(tmp_para->x_axis,tmp_para->y_axis);
  GLCD_WriteString(tmp_para->cchar_name);//
}

unsigned char GetOKStatus(void)
{
  return OK_Status;
}

void EnterMsgThirdControEventTrigger(void* para)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif 
  char tmp_s[2];
  int tmp_tcb[BUF_LEN];
  unsigned char i = 0;
  
  strWindow* hiswindow;
  strContro* hisbutton;
  
  hiswindow = &HisActinveWindow; // 历史活动窗体
  hisbutton = &HisActiveContro; //  历史活动控件
  
  memset(tmp_tcb,-1,BUF_LEN);
  CPU_CRITICAL_ENTER();
  for(i=0;i<BUF_LEN-1;i++)
  {
    tmp_tcb[i] = *(GetLcdData()+i+1);
    if(-1 == tmp_tcb[i]) break;
  }
  CPU_CRITICAL_EXIT();

  if((MAIN_INTERFACE == HisActinveWindow.interface_name)||(0 ==strcmp("TAID",hisbutton->cchar_name)))// 当前活动控件名称
  {
    if(i != 12)
    {
      OK_Status = 1;
      return;
    } 
    OK_Status = 0;
    return;
  }
  
  switch(hiswindow->interface_name)
  {
  case MAIN_INTERFACE:
    break;
  case TWO_INTERFACE(1):
    for(char j=0;j<i;j++)
    {
      if(tmp_tcb[j] > '4')
      {
        OK_Status = 1; 
        return ;
      }
    }
    OK_Status = 0; 
    break;
  case TWO_INTERFACE(2):
    if(1 ==hisbutton->contorl_addrx)
    {
      if(((0 == (tmp_tcb[0]-0x30)*10 + (tmp_tcb[1]-0x30)))||(((tmp_tcb[0]-0x30)*10 + (tmp_tcb[1]-0x30)) > 13)) // 0-12
      {
        OK_Status = 1;
        return;
      }
      tmp_s[0] = tmp_tcb[0];
      if(-1 == tmp_tcb[1])
      {
        tmp_s[1] = ' ';
        if(0 == tmp_s[0] - 0x30)
        {
          OK_Status = 1;
          return;
        }
      }
      else
      {
        tmp_s[1] = tmp_tcb[1];
      }
      
      SetStringNumberValue(tmp_s);
      OK_Status = 0;
      break;
    }
    for(unsigned char j=0;j<i;j++)
    {
      if(tmp_tcb[j] > '4')
      {
        OK_Status = 1; 
        return ;
      }
    }
    OK_Status = 0; 
    break;
  case TWO_INTERFACE(3):
    OK_Status = 0;
    break;
  case TWO_INTERFACE(4):
    if(0 ==strcmp("DA",hisbutton->cchar_name))
    {
      if(i != 8) {OK_Status = 1;return;}
      if((tmp_tcb[0] == '2') && (tmp_tcb[1] == '0')) // 20
      {
        if(((tmp_tcb[2]-0x30)*10 + (tmp_tcb[3]-0x30)) < 99) // 2099  年
        {
          if(((tmp_tcb[4]-0x30)*10 + (tmp_tcb[5]-0x30)) < 12) // 12  月
          {
            switch((tmp_tcb[4]-0x30)*10 + (tmp_tcb[5]-0x30))
            {
             case 2:  // 平月
               {
                 if(((0 == (2000 +(tmp_tcb[2]-0x30)*10 + (tmp_tcb[3]-0x30))%4)&&(0 != (2000 +(tmp_tcb[2]-0x30)*10 + (tmp_tcb[3]-0x30))%100))
                    ||(0 == (2000 +(tmp_tcb[2]-0x30)*10 + (tmp_tcb[3]-0x30))%400)) // 闰年
                 {
                   if(((tmp_tcb[6]-0x30)*10 + (tmp_tcb[7]-0x30)) < 29)
                   {
                     OK_Status = 0;
                     return;
                   }
                 }
                 if(((tmp_tcb[6]-0x30)*10 + (tmp_tcb[7]-0x30)) < 28)//
                 {
                   OK_Status = 0;
                   return;
                 }
                 OK_Status = 1;
                 break;
               }
            case 1: //大月
            case 3:
            case 5:  
            case 7:
            case 8:   
            case 10: 
            case 12: 
              {
                if(((tmp_tcb[6]-0x30)*10 + (tmp_tcb[7]-0x30)) < 31)//
                {
                  OK_Status = 0;
                  return;
                }
                OK_Status = 1;
                break;
              }
            //小月
            case 4:
            case 6:
            case 9:
            case 11:
              {
                if(((tmp_tcb[6]-0x30)*10 + (tmp_tcb[7]-0x30)) < 30)//
                {
                  OK_Status = 0;
                  return;
                }
                OK_Status = 1;
                break;
              } 
            default:
              OK_Status = 1;
              break;
            }
          }
        }
      }
      OK_Status = 1; 
      return ;      
    }
    else if(0 ==strcmp("TI",hisbutton->cchar_name))
    {
      if(i != 6) {OK_Status = 1;return;}
      if(((tmp_tcb[0]-0x30)*10+(tmp_tcb[1]-0x30)) < 24)
      {
        if(((tmp_tcb[2]-0x30)*10+(tmp_tcb[3]-0x30)) < 60)
        {
          if(((tmp_tcb[4]-0x30)*10+(tmp_tcb[5]-0x30)) < 60)
          {
            OK_Status = 0;
            return;
          }
        }
      }
    }
    else if(0 ==strcmp("BA",hisbutton->cchar_name))
    {
      if(i != 4)
      {
        OK_Status = 1;
        return;
      } 
      OK_Status = 0;
      return;
    }
    else if(0 ==strcmp("SH",hisbutton->cchar_name)) 
    {
      if(i != 4)
      {
        OK_Status = 1;
        return;
      } 
      OK_Status = 0;
      return;
    }
    else if(0 ==strcmp("AD",hisbutton->cchar_name)) 
    {
      if(i != 12)
      {
        OK_Status = 1;
        return;
      } 
      OK_Status = 0;
      return;
    }
    OK_Status = 1;
    break;
  case TWO_INTERFACE(5):
    ChangeInfomationFromInput();
    break;
  default:
    break;
  }
}

// 构建窗体(弹窗)
void EnterMsgThirdLevelCreate(void* para)
{
  strWindow* psMainWindow;
  strContro*    pstrContro;
  pfuc   pfather_name;
  char* button_name;
  
  psMainWindow = &sMainWindow;
  pstrContro      = sMainControBuf;
  
  if(TREE_INTERFACE(1) == psMainWindow->interface_name) // 如果已经是弹窗  不再构建
  {
    return;
  }
  
  HisActinveWindow = *psMainWindow;                             //保存父窗体
  
  if(MAIN_INTERFACE == psMainWindow->interface_name)            // 确定弹窗 父窗体
  {
    pfather_name = MainInterfaceCreate;
  }
  else if(TWO_INTERFACE(1) == psMainWindow->interface_name)
  {
    pfather_name = CurSecondLevelInterfaceCreate;
  }
  else if(TWO_INTERFACE(2) == psMainWindow->interface_name)
  {
    pfather_name = StorySecondLevelInterfaceCreate;
  }
  else if(TWO_INTERFACE(3) == psMainWindow->interface_name)
  {
    pfather_name = LocalSecondLevelInterfaceCreate;
  }
  else if(TWO_INTERFACE(4) == psMainWindow->interface_name)
  {
    pfather_name = CfgSecondLevelInterfaceCreate;
  }
  else if(TWO_INTERFACE(5) == psMainWindow->interface_name)
  {
    pfather_name = SendMsgWindowCreate;
  }
  else
  {
    return;
  }  
  
  for(unsigned char i=0;i<psMainWindow->cursor_addr_max;i++)   // 保存活动控件
  {
    if(SELECT == (pstrContro+i)->status)
    {
      HisActiveContro = *(pstrContro+i);
      break;
    }
    else
    {
      HisActiveContro.cchar_name = "TAID";
      HisActiveContro.cchar_value = (void*)0;
      HisActiveContro.contorl_addrx = X_MOVE_DISABLE;
      HisActiveContro.contorl_addry = Y_MOVE_DISABLE;
      HisActiveContro.control_create = (void*)0;
      HisActiveContro.event_trigger = (void*)0;
      HisActiveContro.status = UNSELECT;
      HisActiveContro.type = 0;
      HisActiveContro.x_axis = 0;
      HisActiveContro.y_axis = 0;
    }
  }
  
  switch(psMainWindow->interface_name)                        // 确定活动控件并对应显示弹窗信息
  {
  case MAIN_INTERFACE:
  case TWO_INTERFACE(3):
    button_name         = "TargetAddrSet:";
    CursorInit(1,5,13,5);  // 光标初始化
    CursorEnable();        // 使能闪烁
    break;
  case TWO_INTERFACE(1):
    if((1 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name         = "AP NumberSelect:";//"AP NUMBER SELECT:";
      CursorInit(1,5,6,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    if((2 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name         = "+P NumberSelect:";//"+P NUMBER SELECT:";
      CursorInit(1,5,6,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    if((3 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name         = "-P NumberSelect:";//"-P NUMBER SELECT:";
      CursorInit(1,5,6,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    button_name           = "TargetAddrSet:";//"TARGET ADDR SELECT:";
    CursorInit(1,5,13,5);  // 光标初始化
    CursorEnable();        // 使能闪烁
    break;
  case TWO_INTERFACE(2):
    if((1 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name         = "N NumberSelect:";;//"N NUMBER SELECT:";
      CursorInit(1,5,3,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    if((2 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name         = "AP NumberSelect:";//"AP NUMBER SELECT:";
      CursorInit(1,5,6,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    if((3 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name         = "+P NumberSelect:";//"+P NUMBER SELECT:";
      CursorInit(1,5,6,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    if((4 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name         = "-P NumberSelect:";//"-P NUMBER SELECT:";
      CursorInit(1,5,6,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    button_name           = "TargetAddrSet:";
    CursorInit(1,5,13,5);  // 光标初始化
    CursorEnable();        // 使能闪烁
    break;
  case TWO_INTERFACE(4):
    if((1 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name         = "CurrentDateSet:";//"CURRENT DATE SET:";
      CursorInit(1,5,9,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    if((2 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name         = "CurrentTimeSet:";//"CURRENT TIME SET:";
      CursorInit(1,5,7,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    if((3 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name         = "BackLightTimeSet:";//"BACKLINHT TIME SET:";
      CursorInit(1,5,5,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    if((4 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name        = "ShutDownTimeSet:";//"SHUTDOWN TIME SET:";
      CursorInit(1,5,5,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    if((5 == HisActiveContro.contorl_addrx)&&(SELECT == HisActiveContro.status))
    {
      button_name        = "LocalAddrSet:";//"LOCAL ADDR SET:";
      CursorInit(1,5,13,5);  // 光标初始化
      CursorEnable();        // 使能闪烁
      break;
    }
    button_name          = "TargetAddrSet:";
    CursorInit(1,5,13,5);  // 光标初始化
    CursorEnable();        // 使能闪烁
    break;
  case TWO_INTERFACE(5):
    {
      if(SELECT == HisActiveContro.status)
      {
        switch(HisActiveContro.contorl_addrx)
        {
        case 3:
          {
            button_name        = "UserCodeSet:";//"USER CODE SET:";
            CursorInit(1,5,9,5);  // 光标初始化
            CursorEnable();        // 使能闪烁
            break;
          }
        case 4:
          {
            button_name        = "PassWordSet:";//"PASS WORD SET:";
            CursorInit(1,5,9,5);  // 光标初始化
            CursorEnable();        // 使能闪烁
            break;
          }
        case 2:
          {
            button_name        = "DataMarkedSet:";//"DATA MARKED SET:";
            CursorInit(1,5,9,5);  // 光标初始化
            CursorEnable();        // 使能闪烁
            break;
          }
        case 1:
          {
            button_name        = "TxNumberSet:";//"TX NUMBER SET:";
            CursorInit(1,5,21,5);  // 光标初始化
            CursorEnable();        // 使能闪烁
          }
          break;
        default:
          {
            button_name         = "TargetAddrSet:";
            CursorInit(1,5,13,5);  // 光标初始化
            CursorEnable();        // 使能闪烁
            break;
          }
        }
      }
      else
      {
        button_name             = "TargetAddrSet:";//"TargetAddrSet:";
        CursorInit(1,5,13,5);  // 光标初始化
        CursorEnable();        // 使能闪烁
      }
      break;
    }
  default:
    break;
  }

  memset(&sMainWindow,0,sizeof(sMainWindow));
  memset(sMainControBuf,0,sizeof(sMainControBuf));
  
  // 弹窗界面初始化
  psMainWindow = &sMainWindow;
  psMainWindow->interface_name = TREE_INTERFACE(1);
  psMainWindow->interface_father = pfather_name;
  psMainWindow->pctrpoint_tcb = sMainControBuf;
  psMainWindow->interface_create = EnterMsgThirdLevelFrameCreate;
  psMainWindow->cursor_addrx = 0;
  psMainWindow->cursor_addry = 0;
  psMainWindow->cursor_addr_max = 1;
  
  psMainWindow->interface_create((void*)pstrContro);

  pstrContro->cchar_name         = button_name;
  pstrContro->x_axis             = 1;
  pstrContro->y_axis             = 4;
  pstrContro->cchar_value        = GetLcdData();
  pstrContro->contorl_addrx      = X_MOVE_DISABLE;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __TEXT;
  pstrContro->status             = UNSELECT;
  pstrContro->control_create     = EnterMsgThirdControCreate;
  pstrContro->event_trigger      = EnterMsgThirdControEventTrigger;
  
  psMainWindow->pctrpoint_tcb[0].control_create((void*)&psMainWindow->pctrpoint_tcb[0]);
}

// 自动获取目标地址响应事件
void AutoGetTargetAddrControEventTrigger(void* para)
{
//#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
//  CPU_SR         cpu_sr;
//#endif 
  
  BUT_TypeDef tigger_event;
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
    {
      tigger_event.active_window = active_window->interface_name;
      tigger_event.pactive_button_name = active_contorl->cchar_name;
      memset(&tigger_event.date_tcb,-1,sizeof(tigger_event.date_tcb));

      tigger_event.data_len = 0;
      
      GLCD_TextGoTo(1,5);
      GLCD_WriteString("                    ");
      
      if(LcdEventEnqueue(&tigger_event)) // 存入缓存中,并发送信号
      {
        OSSemPost (SemNormalTcb[UpdateEventSem]);
        IconOper(WARING,ICO_EN,5);
        break;
      }
      IconOper(HOME,ICO_EN,5);
      OSSemPost (SemNormalTcb[UpdateEventSem]);
      break;
    }
  }
}

// 构建自动获取目标地址(弹窗)
void AutoGetTargetAddrCreate(void* para)
{
  strWindow* psMainWindow;
  strContro*    pstrContro;
  pfuc   pfather_name;
  
  psMainWindow = &sMainWindow;
  pstrContro      = sMainControBuf;
  
  if(TREE_INTERFACE(2) == psMainWindow->interface_name) // 如果已经是弹窗  不再构建
  {
    IconOper(WARING,ICO_EN,5);
    return;
  }
  
  HisActinveWindow = *psMainWindow;                             //保存父窗体
  
  if(MAIN_INTERFACE == psMainWindow->interface_name)            // 确定弹窗 父窗体
  {
    pfather_name = MainInterfaceCreate;
  }
  else if(TWO_INTERFACE(1) == psMainWindow->interface_name)
  {
    pfather_name = CurSecondLevelInterfaceCreate;
  }
  else if(TWO_INTERFACE(2) == psMainWindow->interface_name)
  {
    pfather_name = StorySecondLevelInterfaceCreate;
  }
  else if(TWO_INTERFACE(3) == psMainWindow->interface_name)
  {
    pfather_name = LocalSecondLevelInterfaceCreate;
  }
  else if(TWO_INTERFACE(4) == psMainWindow->interface_name)
  {
    pfather_name = CfgSecondLevelInterfaceCreate;
  }
  else if(TWO_INTERFACE(5) == psMainWindow->interface_name)
  {
    pfather_name = SendMsgWindowCreate;
  }
  else
  {
    return;
  }  
  
  for(unsigned char i=0;i<psMainWindow->cursor_addr_max;i++)   // 保存活动控件
  {
    if(SELECT == (pstrContro+i)->status)
    {
      HisActiveContro = *(pstrContro+i);
      break;
    }
    else
    {
      HisActiveContro.cchar_name = "TAID";
      HisActiveContro.cchar_value = (void*)0;
      HisActiveContro.contorl_addrx = X_MOVE_DISABLE;
      HisActiveContro.contorl_addry = Y_MOVE_DISABLE;
      HisActiveContro.control_create = (void*)0;
      HisActiveContro.event_trigger = (void*)0;
      HisActiveContro.status = UNSELECT;
      HisActiveContro.type = 0;
      HisActiveContro.x_axis = 0;
      HisActiveContro.y_axis = 0;
    }
  }
  
  memset(&sMainWindow,0,sizeof(sMainWindow));
  
  // 弹窗界面初始化
  psMainWindow = &sMainWindow;
  psMainWindow->interface_name = TREE_INTERFACE(2);
  psMainWindow->interface_father = pfather_name;
  psMainWindow->pctrpoint_tcb = sMainControBuf;
  psMainWindow->interface_create = EnterMsgThirdLevelFrameCreate;
  psMainWindow->cursor_addrx = 1;
  psMainWindow->cursor_addry = Y_MOVE_DISABLE;
  psMainWindow->cursor_addr_max = 1;
  
  psMainWindow->interface_create((void*)pstrContro);

  pstrContro->cchar_name         = "    AutoGetAddr     ";
  pstrContro->x_axis             = 1;
  pstrContro->y_axis             = 4;
  pstrContro->cchar_value        = (void*)0;
  pstrContro->contorl_addrx      = 1;
  pstrContro->contorl_addry      = Y_MOVE_DISABLE;
  pstrContro->type               = __BUTTON;
  pstrContro->status             = SELECT;
  pstrContro->control_create     = MainSecondLevelSelectEvent;//EnterMsgThirdControCreate;
  pstrContro->event_trigger      = AutoGetTargetAddrControEventTrigger;
  
  psMainWindow->pctrpoint_tcb[0].control_create((void*)&psMainWindow->pctrpoint_tcb[0]);
}