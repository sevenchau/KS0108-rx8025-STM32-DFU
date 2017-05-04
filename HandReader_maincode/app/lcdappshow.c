#include "lcdappshow.h"

static Queue QActive;
INT8U uTargetAddr[6] = {0x00,0x00,0x00,0x00,0x00,0x00};

INT8U* GetTargetAddress(void)
{
  return uTargetAddr;
}

void SetTargetAddress(INT8U* addr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif 

  CPU_CRITICAL_ENTER();
  for(INT8U i=0;i<6;i++)
  {
    uTargetAddr[i] = *(addr+i);
  }
//  memcpy(uTargetAddr,addr,6);
  CPU_CRITICAL_EXIT();
}

/*初始化*/
void LCDKS0108_Initialize(void)
{
  GLCD_Initialize();
  
  GLCD_ClearScreen();
  
  IcoInit();             // 图标init
  
  CursorInit(0,0,21,7);  // 光标 init
  
  Queue_Init(&QActive,ACTIVE_NUM,sizeof(BUT_TypeDef));//
}

void LcdLightOff(void)
{
  KS0108_LightOff();//
}

void LcdLightOn(void)
{
  KS0108_LightOn();
}

// 事件更新 入队列
INT8U LcdEventEnqueue(BUT_TypeDef* tigger_event)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif 
  
  unsigned short ptigger;
  
  CPU_CRITICAL_ENTER(); 
  if(!enQueue(&QActive, &ptigger))
  {
    //    ActiveTcb[ptigger].active_window = tigger_event->active_window;
    //    ActiveTcb[ptigger].data_len      = tigger_event->data_len;
    //    for(INT8U i=0;i<BUF_LEN;i++)
    //    {
    //      ActiveTcb[ptigger].date_tcb[i]    = tigger_event->date_tcb[i];
    //    }
    //    ActiveTcb[ptigger].pactive_button_name = tigger_event->pactive_button_name;
    
    memcpy(&ActiveTcb[ptigger],tigger_event,sizeof(BUT_TypeDef));
    CPU_CRITICAL_EXIT();
    return 0;
  }
  CPU_CRITICAL_EXIT();
  return 1;
}

// 事件更新 出队列 消耗
INT8U LcdEventDelqueue(unsigned char* pwindow_name,char* pbutton_name,int* pvalue_buf,unsigned char* pbuf_len)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif 
  
  unsigned short ptigger;
  BUT_TypeDef tigger_event;
    
  CPU_CRITICAL_ENTER();
  if(!deQueue(&QActive, &ptigger))
  {
    
    memcpy(&tigger_event,&ActiveTcb[ptigger],sizeof(BUT_TypeDef));
    memset(ActiveTcb[ptigger].date_tcb,-1,sizeof(ActiveTcb[ptigger].date_tcb));
    CPU_CRITICAL_EXIT();
    *pwindow_name = tigger_event.active_window;
    memcpy(pbutton_name,tigger_event.pactive_button_name,sizeof(tigger_event.pactive_button_name));
    memcpy(pvalue_buf,tigger_event.date_tcb,sizeof(tigger_event.date_tcb));
    *pbuf_len = tigger_event.data_len;
    return 0;
  }
  CPU_CRITICAL_EXIT();
  return 1;
}

/*
*******************************************************************************
************************************事件响应***********************************
*******************************************************************************
*/
const char Key1Value[2][26] = 
{
  {'.', '~', '!', '@','#','$','%','^','&','*','(',')','-','_','+','=','|','{','}',':',';','"','<','>','/','?'},
  {'.', '~', '!', '@','#','$','%','^','&','*','(',')','-','_','+','=','|','{','}',':',';','"','<','>','/','?'}
};
const char Key2Value[2][3] = {{'a','b','c'},    {'A','B','C'}};
const char Key3Value[2][3] = {{'d','e','f'},    {'D','E','F'}};
const char Key4Value[2][3] = {{'g','h','i'},    {'G','H','I'}};
const char Key5Value[2][3] = {{'j','k','l'},    {'J','K','L'}};
const char Key6Value[2][3] = {{'m','n','o'},    {'M','N','O'}};
const char Key7Value[2][4] = {{'p','q','r','s'},{'P','Q','R','S'}};
const char Key8Value[2][3] = {{'t','u','v'},    {'T','U','V'}};
const char Key9Value[2][4] = {{'w','x','y','z'},{'W','X','Y','Z'}};
const char Key0Value[2][3] = {{'.','%','-'},    {'.','%','-'}};

#define KEY_NUM       10
typedef struct __KEY_VALUE_
{
  INT8U  status; // 1 该按键处于活动状态   0 未活动
  INT8U* pchick; // 检查输入数字还是大小写字母，横向
  INT8U  key_cnt; // 该按键被按的次数，根据大小写切换和按次数来显示当前回输入的字母
}KEY_TypeDef;

KEY_TypeDef strKeyTcb[KEY_NUM];

/*事件响应*/
void EventResponseHander(unsigned int key_value)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif 
  
  static unsigned char char_input_flag = 0; // no input
  
  static unsigned char asii_input_cnt = 0;
  static unsigned char asii_input_flag = 0;
  static unsigned char active_char_flag;  
  
  static unsigned char auto_get_flag = 0; 
  static unsigned char active_char;

  
  unsigned char i;
  unsigned char active_window_name;
  strWindow* psMainWindow;
  strContro*    pstrContro;
  unsigned char pro_addr;
  
  psMainWindow = &sMainWindow;
  pstrContro      = sMainControBuf;
  
  switch(key_value)
  {
  case 0xBC01: // S14  L//游标左移
    if(active_char_flag)break;
    if(char_input_flag) // 录入字符状态
    {
      //MOVE_LEFT MOVE_RIGHT  MOVE_UP MOVE_DOWN
      CursorMove(MOVE_LEFT);
      break;
    }
    if(0 == psMainWindow->cursor_addrx) // 界面刚刷入,如果有按键则跳到第一个控件
    {
      psMainWindow->cursor_addrx = (*(pstrContro+0)).contorl_addrx;
      psMainWindow->cursor_addry = (*(pstrContro+0)).contorl_addry;
      pro_addr = psMainWindow ->cursor_addrx;
    }
    else
    {
      if(X_MOVE_DISABLE == psMainWindow->cursor_addrx)break;// 该方向禁止移动
      pro_addr = psMainWindow ->cursor_addrx;
      psMainWindow ->cursor_addrx -- ;
      if(0 == psMainWindow ->cursor_addrx)          // 如果当前地址被减为 0
      {
        psMainWindow ->cursor_addrx = psMainWindow->cursor_addr_max;
      }
    }
    for(i=0;i<psMainWindow->cursor_addr_max;i++)// 取消上次选中图标
    {
      if((pstrContro+i)->contorl_addrx == pro_addr)
      {
        if(UNSELECT == (pstrContro+i)->status) break;
         (pstrContro+i)->status = UNSELECT;// 取消选中事件
         (pstrContro+i)->event_trigger((void*)(pstrContro+i));
         break;
      }
    }
    
    for(i=0;i<psMainWindow->cursor_addr_max;i++)//选中本次目标
    {
      if((pstrContro+i)->contorl_addrx == psMainWindow->cursor_addrx)
      {
         psMainWindow->cursor_addry = (pstrContro+i)->contorl_addry;
         if(SELECT == (pstrContro+i)->status) break;
         (pstrContro+i)->status = SELECT;// 选中事件
         (pstrContro+i)->event_trigger((void*)(pstrContro+i));
         break;
      }
    }
    break;
  case 0x7C04: // S11  R//游标右移
    if(active_char_flag)break;
    if(char_input_flag) // 录入字符状态
    {
      if(0 == CursorBehindData())break; //在录入末尾 无法再右移
      //MOVE_LEFT MOVE_RIGHT  MOVE_UP MOVE_DOWN
      CursorMove(MOVE_RIGHT);
      break;
    }
    
    if(0 == psMainWindow->cursor_addrx)
    {
      psMainWindow->cursor_addrx = (*(pstrContro+0)).contorl_addrx;
      psMainWindow->cursor_addry = (*(pstrContro+0)).contorl_addry;
      pro_addr = psMainWindow ->cursor_addrx;
    }
    else
    {
      if(X_MOVE_DISABLE == psMainWindow->cursor_addrx)break;
      pro_addr = psMainWindow ->cursor_addrx;
      psMainWindow ->cursor_addrx = (psMainWindow ->cursor_addrx)%(psMainWindow->cursor_addr_max)+1;
    }
    for(i=0;i<psMainWindow->cursor_addr_max;i++)
    {
      if((pstrContro+i)->contorl_addrx == pro_addr)
      {
        if(UNSELECT == (pstrContro+i)->status) break;
         (pstrContro+i)->status = UNSELECT;// 取消选中事件
         (pstrContro+i)->event_trigger((void*)(pstrContro+i));
         break;
      }
    }
    for(i=0;i<psMainWindow->cursor_addr_max;i++)
    {
      if((pstrContro+i)->contorl_addrx == psMainWindow->cursor_addrx)
      {
         psMainWindow->cursor_addry = (pstrContro+i)->contorl_addry;
         if(SELECT == (pstrContro+i)->status) break;
         (pstrContro+i)->status = SELECT;// 选中事件
         (pstrContro+i)->event_trigger((void*)(pstrContro+i));
         break;
      }
    }
    break;
  case 0x7C01: // S12  UP//游标上移
    if(active_char_flag)break;
    if(MAIN_INTERFACE == psMainWindow->interface_name)
    {
      HisActinveWindow = sMainWindow;// 记录历史活动窗体及界面
      SendMsgWindowCreate((void*)0);
      break;
    }
    
    if(char_input_flag) // 录入字符状态
    {
      //MOVE_LEFT MOVE_RIGHT  MOVE_UP MOVE_DOWN
      CursorMove(MOVE_UP);
      break;
    }
    if(0 == psMainWindow->cursor_addry)
    {
      psMainWindow->cursor_addrx = (*(pstrContro+0)).contorl_addrx;
      psMainWindow->cursor_addry = (*(pstrContro+0)).contorl_addry;
      pro_addr = psMainWindow ->cursor_addry;
    }
    else
    {
      if(Y_MOVE_DISABLE == psMainWindow->cursor_addrx)break;
      pro_addr = psMainWindow ->cursor_addry;
      psMainWindow ->cursor_addry = (psMainWindow ->cursor_addry)%(psMainWindow->cursor_addr_max)+1;
    }
    for(i=0;i<psMainWindow->cursor_addr_max;i++)
    {
      if((pstrContro+i)->contorl_addry == pro_addr)
      {
        if(UNSELECT == (pstrContro+i)->status) break;
         (pstrContro+i)->status = UNSELECT;// 取消选中事件
         (pstrContro+i)->event_trigger((void*)(pstrContro+i));
         break;
      }
    }
    for(i=0;i<psMainWindow->cursor_addr_max;i++)
    {
      if((pstrContro+i)->contorl_addry == psMainWindow->cursor_addry)
      {
         psMainWindow->cursor_addrx = (pstrContro+i)->contorl_addrx;
         if(SELECT == (pstrContro+i)->status) break;
         (pstrContro+i)->status = SELECT;// 选中事件
         (pstrContro+i)->event_trigger((void*)(pstrContro+i));
         break;
      }
    }
    break;
  case 0xBC02: // S13  DN//游标下移
    if(active_char_flag)break;
    if(char_input_flag) // 录入字符状态,不能弹出自动获取地址弹窗
    {
      //MOVE_LEFT MOVE_RIGHT  MOVE_UP MOVE_DOWN
      CursorMove(MOVE_DOWN);
      break;
    }
    else
    {
      auto_get_flag = ~auto_get_flag;
      if(auto_get_flag)     // 自动获取弹窗
      {
        HisActinveWindow = sMainWindow;// 记录历史活动窗体及界面
        for(INT8U i=0;i<sMainWindow.cursor_addr_max;i++)
        {
          if(SELECT == sMainWindow.pctrpoint_tcb[i].status)
          {
            HisActiveContro = sMainWindow.pctrpoint_tcb[i];
            break;
          }
          else
          {
            HisActiveContro.cchar_name = "TAID";
            HisActiveContro.cchar_value = (void*)0;
            HisActiveContro.contorl_addrx = 0xff;
            HisActiveContro.contorl_addry = 0xff;
            HisActiveContro.control_create = (void*)0;
            HisActiveContro.event_trigger = (void*)0;
            HisActiveContro.status = UNSELECT;
            HisActiveContro.type = __BUTTON;
            HisActiveContro.x_axis = X_MOVE_DISABLE;
            HisActiveContro.y_axis = Y_MOVE_DISABLE;
          }
        }
        AutoGetTargetAddrCreate((void*)0);
        break;
      }// 按第二次 弹回历史界面
      goto LOOP2;
    }
    if(0 == psMainWindow->cursor_addry)
    {
      psMainWindow->cursor_addrx = (*(pstrContro+0)).contorl_addrx;
      psMainWindow->cursor_addry = (*(pstrContro+0)).contorl_addry;
      pro_addr = psMainWindow ->cursor_addry;
    }
    else
    {
      if(Y_MOVE_DISABLE == psMainWindow->cursor_addry)break;
      pro_addr = psMainWindow ->cursor_addry;
      psMainWindow ->cursor_addry -- ;
      if(0 == psMainWindow ->cursor_addry)
      {
        psMainWindow ->cursor_addry = psMainWindow->cursor_addr_max;
      }
    }
    for(i=0;i<psMainWindow->cursor_addr_max;i++)
    {
      if((pstrContro+i)->contorl_addry == pro_addr)
      {
        if(UNSELECT == (pstrContro+i)->status) break;
         (pstrContro+i)->status = UNSELECT;// 取消选中事件
         (pstrContro+i)->event_trigger((void*)(pstrContro+i));
         break;
      }
    }
    
    for(i=0;i<psMainWindow->cursor_addr_max;i++)
    {
      if((pstrContro+i)->contorl_addry == psMainWindow->cursor_addry)
      {
         psMainWindow->cursor_addrx = (pstrContro+i)->contorl_addrx;
         if(SELECT == (pstrContro+i)->status) break;
         (pstrContro+i)->status = SELECT;// 选中事件
         (pstrContro+i)->event_trigger((void*)(pstrContro+i));
         break;
      }
    }
    break;
  case 0x7C02: // S15  OK// 触发 OK 确认
    if(((TREE_INTERFACE(1) == psMainWindow->interface_name)&&char_input_flag)||// 录入字符状态
       ((TREE_INTERFACE(1) == psMainWindow->interface_name)&&asii_input_flag))
    {
      if((void*)0 == GetLcdData())  // 无数据,窗口不弹回
      {
        break;
      }
      if(psMainWindow->interface_father == (void*)0)
        break;
      psMainWindow->pctrpoint_tcb[0].event_trigger((void*)&psMainWindow->pctrpoint_tcb[0]); // 调用OK处理事件
      if(GetOKStatus()) break;
      
      GLCD_TextGoTo(1,4);
      GLCD_WriteString("        OK          ");
      GLCD_TextGoTo(1,5);
      GLCD_WriteAntiColorString("    ");
      OSTimeDly (25);
      GLCD_WriteAntiColorString("    ");
      OSTimeDly (20);
      GLCD_WriteAntiColorString("    ");
      OSTimeDly (15);
      GLCD_WriteAntiColorString("    ");
      OSTimeDly (10);
      GLCD_WriteAntiColorString("    ");
      OSTimeDly (5);
      
      psMainWindow = &sMainWindow;
      active_window_name = psMainWindow->interface_name;   // 记录当前活动窗体名称 
      psMainWindow->interface_father((void*)0);
      if(TREE_INTERFACE(1) == active_window_name)             // 上一次窗口为输入窗口
      {
        psMainWindow = &sMainWindow;                    // 指针重新找回活动界面
        for(i=0;i<HisActinveWindow.cursor_addr_max;i++) // 信息输入弹窗之前的窗口是否有活动控件
        {
          if((HisActiveContro.contorl_addrx == (pstrContro+i)->contorl_addrx)
             && (HisActiveContro.contorl_addry == (pstrContro+i)->contorl_addry))
          {
            psMainWindow->cursor_addrx = HisActiveContro.contorl_addrx; // 游标移动
            psMainWindow->cursor_addry = HisActiveContro.contorl_addry;
            
            if(MAIN_INTERFACE != psMainWindow->interface_name)  // 如果为主主窗口  不跳转
            {
              psMainWindow->pctrpoint_tcb[i].status = TIGGER_EVNT;// 
              psMainWindow->pctrpoint_tcb[i].event_trigger((void*)&(psMainWindow->pctrpoint_tcb[i]));
            }

            psMainWindow = &sMainWindow;
            psMainWindow->pctrpoint_tcb[i].status = SELECT;//TIGGER_EVNT;//
            psMainWindow->pctrpoint_tcb[i].event_trigger((void*)&(psMainWindow->pctrpoint_tcb[i]));
            break;
          }
        }
        if((i == HisActinveWindow.cursor_addr_max)||(MAIN_INTERFACE == psMainWindow->interface_name)) // 无活动控件或者为主界面
        {
          BUT_TypeDef tigger_event;
          
          tigger_event.active_window = psMainWindow->interface_name;
          tigger_event.pactive_button_name = "TAID";
          memset(&tigger_event.date_tcb,-1,sizeof(BUT_TypeDef));
          CPU_CRITICAL_ENTER();
          for(char i=0;i<BUF_LEN-1;i++)
          {
            tigger_event.date_tcb[i] = *(GetLcdData()+i+1);
            if(-1 == *(GetLcdData()+i+1)) break;
          }
          CPU_CRITICAL_EXIT();
//          memcpy(tigger_event.date_tcb,GetLcdData(),BUF_LEN);
          
          if(LcdEventEnqueue(&tigger_event))
          {
            OSSemPost (SemNormalTcb[UpdateEventSem]);
            IconOper(WARING,ICO_EN,5);
            
            asii_input_cnt = char_input_flag = auto_get_flag = 0;
            asii_input_flag = 0;
            for(INT8U j=0;j<KEY_NUM;j++)
            {
              strKeyTcb[j].pchick = &asii_input_cnt;
              strKeyTcb[j].status = 0;
              strKeyTcb[j].key_cnt = 0;
              active_char_flag = 0;
            } 
            CursorDisable();
            break;
          }
          IconOper(HOME,ICO_EN,5);
          OSSemPost (SemNormalTcb[UpdateEventSem]);
        }
        
        asii_input_cnt = char_input_flag = auto_get_flag = 0;
        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
          active_char_flag = 0;
        } 
        CursorDisable();
      }
    }
    else
    {
      if((0 == psMainWindow->cursor_addrx)&&(0 == psMainWindow->cursor_addry))
      {
        CursorDisable();
        break;
      }        

      pstrContro      = sMainControBuf;
      for(i=0;i<psMainWindow->cursor_addr_max;i++)
      {
        if(((pstrContro+i)->contorl_addry == psMainWindow->cursor_addry)
           &&(psMainWindow->cursor_addrx == (pstrContro+i)->contorl_addrx))
        {
          if(TREE_INTERFACE(2) == sMainWindow.interface_name) // 记录历史窗体，除自动获取地址弹窗外
          {
            (pstrContro+i)->status = TIGGER_EVNT;// 选中事件
            (pstrContro+i)->event_trigger((void*)(pstrContro+i));
            
            psMainWindow = &sMainWindow; 
            if(psMainWindow->cursor_addr_max)
            {
              psMainWindow->pctrpoint_tcb[i].status = SELECT;//TIGGER_EVNT;//
              psMainWindow->pctrpoint_tcb[i].event_trigger((void*)&(psMainWindow->pctrpoint_tcb[i]));
            }
            break;
          }
          HisActinveWindow = sMainWindow;   // 历史活动窗体
          HisActiveContro  = psMainWindow->pctrpoint_tcb[i];//历史活动控件
          (pstrContro+i)->status = TIGGER_EVNT;// 选中事件
          (pstrContro+i)->event_trigger((void*)(pstrContro+i));
          
          psMainWindow = &sMainWindow; 
          if((psMainWindow->cursor_addr_max)&&(HisActinveWindow.interface_name == psMainWindow->interface_name))
          {
            psMainWindow->pctrpoint_tcb[i].status = SELECT;//TIGGER_EVNT;//
            psMainWindow->pctrpoint_tcb[i].event_trigger((void*)&(psMainWindow->pctrpoint_tcb[i]));
          }
          break;
        }
      }
      CursorDisable();
    }
    break;
  case 0xBC04: // S18  CHANGE 删除
    if((asii_input_flag)&&(asii_input_cnt))// 全字符录入状态
    {
      if(0 == active_char_flag)
      {
        DeletChar();
        break;
      }
      active_char_flag = 0;
      ScanfChar(active_char);
      CursorEnable();
      //        char_input_flag = 0;
//      asii_input_flag = 0;
      for(INT8U j=0;j<KEY_NUM;j++)
      {
        strKeyTcb[j].pchick = &asii_input_cnt;
        strKeyTcb[j].status = 0;
        strKeyTcb[j].key_cnt = 0;
      }
      break;
    }
    DeletChar();
    break;
  case 0xF804: // S16  #      // 按第一输入，按第二次取消输入
    {
      if(auto_get_flag) break; // 处于自动获取地址 弹窗状态下，不能弹窗该弹窗
      
      // 不为弹窗时需要，记录历史活动窗体及界面
      if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      {
        HisActinveWindow = sMainWindow;// 记录历史活动窗体及界面
        for(INT8U i=0;i<sMainWindow.cursor_addr_max;i++)// 记录历史活动控件
        {
          if(SELECT == sMainWindow.pctrpoint_tcb[i].status)
          {
            HisActiveContro = sMainWindow.pctrpoint_tcb[i];
            break;
          }
          else
          {
            HisActiveContro.cchar_name = "TAID";
            HisActiveContro.cchar_value = (void*)0;
            HisActiveContro.contorl_addrx = 0xff;
            HisActiveContro.contorl_addry = 0xff;
            HisActiveContro.control_create = (void*)0;
            HisActiveContro.event_trigger = (void*)0;
            HisActiveContro.status = UNSELECT;
            HisActiveContro.type = __BUTTON;
            HisActiveContro.x_axis = X_MOVE_DISABLE;
            HisActiveContro.y_axis = Y_MOVE_DISABLE;
          }
        }
        EnterMsgThirdLevelCreate((void*)0);
        CursorEnable();
      }
      
      if((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))// 如果在编程窗体下，可以输入全ascii码
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt; // 指向当前输入的是数字还是字母大小写
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
//        CursorDisable();
        asii_input_cnt %= 3;
        asii_input_flag = 1;
        switch(asii_input_cnt)
        {
        case 0:
          GLCD_TextGoTo(17,4);
          GLCD_WriteAntiColorString(" 12 ");
          break;
        case 1:
          GLCD_TextGoTo(17,4);
          GLCD_WriteAntiColorString(" en ");
          break;
        case 2:
          GLCD_TextGoTo(17,4);
          GLCD_WriteAntiColorString(" EN ");
          break;
        default:
          break;
        }
        asii_input_cnt ++;
        
        char_input_flag = 0xff;
        
        break;
      }
      
      char_input_flag = ~char_input_flag;
      if(char_input_flag)     // 输入
      {
        break;
      }
      ClearLcdStorData();    //取消输入,返回上一层界面
    }
  case 0xDC01: // S17  CANCEL// 返回上一层界面 
  LOOP2:
    if(psMainWindow->interface_father == (void*)0)
      break;
    active_window_name = psMainWindow->interface_name;   // 记录当前活动窗体名称 
    psMainWindow->interface_father((void*)0);
    if((TREE_INTERFACE(1) == active_window_name)||(TREE_INTERFACE(2) == active_window_name))             // 上一次窗口为输入窗口
    {
      for(unsigned char i=0;i<HisActinveWindow.cursor_addr_max;i++) // 信息输入弹窗之前的窗口是否有活动控件
      {
        if((HisActiveContro.contorl_addrx == (pstrContro+i)->contorl_addrx)
           && (HisActiveContro.contorl_addry == (pstrContro+i)->contorl_addry))
        {
          psMainWindow->cursor_addrx = HisActiveContro.contorl_addrx; // 游标移动
          psMainWindow->cursor_addry = HisActiveContro.contorl_addry;
          
          *(pstrContro+i) = HisActiveContro;
          (pstrContro+i)->event_trigger((void*)(pstrContro+i));// 选中事件
          char_input_flag = auto_get_flag = 0;
          break;
        }
      }
      asii_input_cnt = char_input_flag = auto_get_flag = 0;
      asii_input_flag = 0;
      for(INT8U j=0;j<KEY_NUM;j++)
      {
        strKeyTcb[j].pchick = &asii_input_cnt;
        strKeyTcb[j].status = 0;
        strKeyTcb[j].key_cnt = 0;
        active_char_flag = 0;
      }  
    }
    CursorDisable();
    break;
  case 0xEC01: // S1   '1'
    if(((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))&&(asii_input_flag)) // 输入全字符码弹窗 使能
    {
      if(0 == strKeyTcb[1].status)
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
      }
      active_char_flag = 1;
      strKeyTcb[1].status = 1;
      if(0 == (*(strKeyTcb[4].pchick)-1))
      {
        ScanfChar('1');
        CursorEnable();
//        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
        active_char_flag = 0;
        break;
      }
      CursorDisable();
      ShowScanfChar(Key1Value[(*(strKeyTcb[1].pchick))-2][strKeyTcb[1].key_cnt]);
      active_char = Key1Value[(*(strKeyTcb[1].pchick))-2][strKeyTcb[1].key_cnt];
      strKeyTcb[1].key_cnt ++;
      strKeyTcb[1].key_cnt %= 26;
      break;
    }
    if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      break;
    if((!asii_input_flag)&&(!char_input_flag))
      break;
//    active_char_flag = 0;
    ScanfChar('1');
    CursorEnable();
    break;
  case 0xDC02: // S2   2
    if(((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))&&(asii_input_flag)) // 输入全字符码弹窗 使能
    {
      if(0 == strKeyTcb[2].status)
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
      }
      active_char_flag = 1;
      strKeyTcb[2].status = 1;
      if(0 == (*(strKeyTcb[2].pchick)-1))
      {
        ScanfChar('2');
        CursorEnable();
//        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
        active_char_flag = 0;
        break;
      }
      CursorDisable();
      ShowScanfChar(Key2Value[(*(strKeyTcb[2].pchick))-2][strKeyTcb[2].key_cnt]);
      active_char = Key2Value[(*(strKeyTcb[2].pchick))-2][strKeyTcb[2].key_cnt];
      strKeyTcb[2].key_cnt ++;
      strKeyTcb[2].key_cnt %= 3;
      break;
    }
    if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      break;
    if((!asii_input_flag)&&(!char_input_flag))
      break;
//    active_char_flag = 0;
    ScanfChar('2');
    CursorEnable();
    break;
  case 0xDC04: // S3   3
    if(((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))&&(asii_input_flag)) // 输入全字符码弹窗 使能
    {
      if(0 == strKeyTcb[3].status)
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
      }
      active_char_flag = 1;
      strKeyTcb[3].status = 1;
      if(0 == (*(strKeyTcb[3].pchick)-1))
      {
        ScanfChar('3');
        CursorEnable();
//        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
        active_char_flag = 0;
        break;
      }
      CursorDisable();
      ShowScanfChar(Key3Value[(*(strKeyTcb[3].pchick))-2][strKeyTcb[3].key_cnt]);
      active_char = Key3Value[(*(strKeyTcb[3].pchick))-2][strKeyTcb[3].key_cnt];
      strKeyTcb[3].key_cnt ++;
      strKeyTcb[3].key_cnt %= 3;
      break;
    }
    if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      break;
    if((!asii_input_flag)&&(!char_input_flag))
      break;
//    active_char_flag = 0;
    ScanfChar('3');
    CursorEnable();
    break;
  case 0xF401: // S4   4
    if(((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))&&(asii_input_flag)) // 输入全字符码弹窗 使能
    {
      if(0 == strKeyTcb[4].status)
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
      }
      active_char_flag = 1;
      strKeyTcb[4].status = 1;
      if(0 == (*(strKeyTcb[4].pchick)-1))
      {
        ScanfChar('4');
        CursorEnable();
//        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
        active_char_flag = 0;
        break;
      }
      CursorDisable();
      ShowScanfChar(Key4Value[(*(strKeyTcb[4].pchick))-2][strKeyTcb[4].key_cnt]);
      active_char = Key4Value[(*(strKeyTcb[4].pchick))-2][strKeyTcb[4].key_cnt];
      strKeyTcb[4].key_cnt ++;
      strKeyTcb[4].key_cnt %= 3;
      break;
    }
    if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      break;
    if((!asii_input_flag)&&(!char_input_flag))
      break;
//    active_char_flag = 0;
    ScanfChar('4');
    CursorEnable();
    break;
  case 0xEC02: // S5   5
    if(((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))&&(asii_input_flag)) // 输入全字符码弹窗 使能
    {
      if(0 == strKeyTcb[5].status)
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
      }
      active_char_flag = 1;
      strKeyTcb[5].status = 1;
      if(0 == (*(strKeyTcb[5].pchick)-1))
      {
        ScanfChar('5');
        CursorEnable();
//        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
        active_char_flag = 0;
        break;
      }
      CursorDisable();
      ShowScanfChar(Key5Value[(*(strKeyTcb[5].pchick))-2][strKeyTcb[5].key_cnt]);
      active_char = Key5Value[(*(strKeyTcb[5].pchick))-2][strKeyTcb[5].key_cnt];
      strKeyTcb[5].key_cnt ++;
      strKeyTcb[5].key_cnt %= 3;
      break;
    }
    if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      break;
    if((!asii_input_flag)&&(!char_input_flag))
      break;
//    active_char_flag = 0;
    ScanfChar('5');
    CursorEnable();
    break;
  case 0xEC04: // S6   6
    if(((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))&&(asii_input_flag)) // 输入全字符码弹窗 使能
    {
      if(0 == strKeyTcb[6].status)
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
      }
      active_char_flag = 1;
      strKeyTcb[6].status = 1;
      if(0 == (*(strKeyTcb[6].pchick)-1))
      {
        ScanfChar('6');
        CursorEnable();
//        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
        active_char_flag = 0;
        break;
      }
      CursorDisable();
      ShowScanfChar(Key6Value[(*(strKeyTcb[6].pchick))-2][strKeyTcb[6].key_cnt]);
      active_char = Key6Value[(*(strKeyTcb[6].pchick))-2][strKeyTcb[6].key_cnt];
      strKeyTcb[6].key_cnt ++;
      strKeyTcb[6].key_cnt %= 3;
      break;
    }
    if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      break;
    if((!asii_input_flag)&&(!char_input_flag))
      break;
//    active_char_flag = 0;
    ScanfChar('6');
    CursorEnable();
    break;
  case 0xF801: // S7   7
    if(((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))&&(asii_input_flag)) // 输入全字符码弹窗 使能
    {
      if(0 == strKeyTcb[7].status)
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
      }
      active_char_flag = 1;
      strKeyTcb[7].status = 1;
      if(0 == (*(strKeyTcb[1].pchick)-1))
      {
        ScanfChar('7');
        CursorEnable();
//        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
        active_char_flag = 0;
        break;
      }
      CursorDisable();
      ShowScanfChar(Key7Value[(*(strKeyTcb[7].pchick))-2][strKeyTcb[7].key_cnt]);
      active_char = Key7Value[(*(strKeyTcb[7].pchick))-2][strKeyTcb[7].key_cnt];
      strKeyTcb[7].key_cnt ++;
      strKeyTcb[7].key_cnt %= 4;
      break;
    }
    if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      break;
    if((!asii_input_flag)&&(!char_input_flag))
      break;
//    active_char_flag = 0;
    ScanfChar('7');
    CursorEnable();
    break;
  case 0xF402: // S8   8
    if(((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))&&(asii_input_flag)) // 输入全字符码弹窗 使能
    {
      if(0 == strKeyTcb[8].status)
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
      }
      active_char_flag = 1;
      strKeyTcb[8].status = 1;
      if(0 == (*(strKeyTcb[1].pchick)-1))
      {
        ScanfChar('8');
        CursorEnable();
//        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
        active_char_flag = 0;
        break;
      }
      CursorDisable();
      ShowScanfChar(Key8Value[(*(strKeyTcb[8].pchick))-2][strKeyTcb[8].key_cnt]);
      active_char = Key8Value[(*(strKeyTcb[8].pchick))-2][strKeyTcb[8].key_cnt];
      strKeyTcb[8].key_cnt ++;
      strKeyTcb[8].key_cnt %= 3;
      break;
    }
    if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      break;
    if((!asii_input_flag)&&(!char_input_flag))
      break;
//    active_char_flag = 0;
    ScanfChar('8');
    CursorEnable();
    break;
  case 0xF404: // S9   9
    if(((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))&&(asii_input_flag)) // 输入全字符码弹窗 使能
    {
      if(0 == strKeyTcb[9].status)
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
      }
      active_char_flag = 1;
      strKeyTcb[9].status = 1;
      if(0 == (*(strKeyTcb[1].pchick)-1))
      {
        ScanfChar('9');
        CursorEnable();
//        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
        active_char_flag = 0;
        break;
      }
      CursorDisable();
      ShowScanfChar(Key9Value[(*(strKeyTcb[9].pchick))-2][strKeyTcb[9].key_cnt]);
      active_char = Key9Value[(*(strKeyTcb[9].pchick))-2][strKeyTcb[9].key_cnt];
      strKeyTcb[9].key_cnt ++;
      strKeyTcb[9].key_cnt %= 4;
      break;
    }
    if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      break;
    if((!asii_input_flag)&&(!char_input_flag))
      break;
//    active_char_flag = 0;
    ScanfChar('9');
    CursorEnable();
    break;
  case 0xF802: // S10  0
    if(((TWO_INTERFACE(5) == HisActinveWindow.interface_name)&&(1 == HisActiveContro.contorl_addrx))&&(asii_input_flag)) // 输入全字符码弹窗 使能
    {
      if(0 == strKeyTcb[0].status)
      {
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
      }
      active_char_flag = 1;
      strKeyTcb[0].status = 1;
      if(0 == (*(strKeyTcb[1].pchick)-1))
      {
        ScanfChar('0');
        CursorEnable();
//        asii_input_flag = 0;
        for(INT8U j=0;j<KEY_NUM;j++)
        {
          strKeyTcb[j].pchick = &asii_input_cnt;
          strKeyTcb[j].status = 0;
          strKeyTcb[j].key_cnt = 0;
        }
        active_char_flag = 0;
        break;
      }
      CursorDisable();
      ShowScanfChar(Key0Value[(*(strKeyTcb[0].pchick))-2][strKeyTcb[0].key_cnt]);
      active_char = Key0Value[(*(strKeyTcb[0].pchick))-2][strKeyTcb[0].key_cnt];
      strKeyTcb[0].key_cnt ++;
      strKeyTcb[0].key_cnt %= 3;
      break;
    }
    if((TREE_INTERFACE(1) != psMainWindow->interface_name)&&(TREE_INTERFACE(2) != psMainWindow->interface_name))
      break;
    if((!asii_input_flag)&&(!char_input_flag))
      break;
//    active_char_flag = 0;
    ScanfChar('0');
    CursorEnable();
    break;
  default:
    break;
  }
}

// 获取当前活动窗体
char GetActiveWindow(void)
{
  return sMainWindow.interface_name;
}

// 获取当前活动控件
char* GetActiveButton(void)
{
  strWindow* pactive_window;
  strContro*    pactive_button;
  
  pactive_window = &sMainWindow;
  pactive_button = sMainControBuf;
  
  for(char i=0;i<pactive_window->cursor_addr_max;i++)
  {
    if(SELECT == pactive_button->status)
    {
      return sMainControBuf[i].cchar_name;
    }
  }
  
  return (void*)0;
}
