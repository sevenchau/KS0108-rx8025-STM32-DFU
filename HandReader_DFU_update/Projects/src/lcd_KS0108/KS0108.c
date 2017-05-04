//-------------------------------------------------------------------------------------------------
// Graphic LCD C library
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include "KS0108.h"
#include "font5x8.h"
#include "string.h"
//-------------------------------------------------------------------------------------------------
// Reference to MCU-specific external functions
//-------------------------------------------------------------------------------------------------
extern void GLCD_InitializePorts(void);
extern void GLCD_WriteData(unsigned char);
extern void GLCD_WriteCommand(unsigned char, unsigned char);
extern unsigned char GLCD_ReadData(void);
extern unsigned char GLCD_ReadStatus(unsigned char);
//-------------------------------------------------------------------------------------------------
unsigned char screen_x = 0, screen_y = 0; // screen coordinates
//-------------------------------------------------------------------------------------------------
// Initialization interface and LCD controller
//-------------------------------------------------------------------------------------------------
void GLCD_Initialize(void)
{
  unsigned int    cnt;
  unsigned char i;
  GLCD_InitializePorts();
  GLCD_ReBoot();
  
  while(GLCD_ReadStatus(0)&DISPLAY_STATUS_OFF)//
  {
    for(i = 0; i < 2; i++)
    {
      cnt = 6000;
      GLCD_WriteCommand(0x3F, i);
      while(cnt  --);
    }
  }
}
//-------------------------------------------------------------------------------------------------
// Sets screen coordinates and address 
//-------------------------------------------------------------------------------------------------
void GLCD_GoTo(unsigned char x, unsigned char y)
{
  unsigned char i;
  screen_x = x;
  screen_y = y;
  
  for(i = 0; i < KS0108_SCREEN_WIDTH/64; i++)
  {
    GLCD_WriteCommand(DISPLAY_SET_Y | 0,i);
    GLCD_WriteCommand(DISPLAY_SET_X | y,i);
    GLCD_WriteCommand(DISPLAY_START_LINE | 0,i);
  }
  GLCD_WriteCommand((DISPLAY_SET_Y | (x % 64)), (x / 64));
  GLCD_WriteCommand((DISPLAY_SET_X | y), (x/ 64));
}
//-------------------------------------------------------------------------------------------------
// Sets text coordinates and address 
//-------------------------------------------------------------------------------------------------
void GLCD_TextGoTo(unsigned char x, unsigned char y)
{
  GLCD_GoTo(x*6,y);
}

//-------------------------------------------------------------------------------------------------
// Writes char to screen memory
//-------------------------------------------------------------------------------------------------
void GLCD_WriteChar(char charToWrite)
{
  char i;
  charToWrite -= 32; 
  for(i = 0; i < 5; i++) 
    GLCD_WriteData(GLCD_ReadByteFromROMMemory((char *)(font5x8 + (5 * charToWrite) + i))); 
  GLCD_WriteData(0x00);
}

void GLCD_WriteAntiColorChar(char charToWrite)
{
  char i;
  charToWrite -= 32; 
  for(i = 0; i < 5; i++) 
    GLCD_WriteData(~GLCD_ReadByteFromROMMemory((char *)(font5x8 + (5 * charToWrite) + i))); 
  GLCD_WriteData(0xff);
}

//-------------------------------------------------------------------------------------------------
// Write null-terminated string to screen memory
//-------------------------------------------------------------------------------------------------
void GLCD_WriteString(char * stringToWrite)
{
  while(*stringToWrite)
    GLCD_WriteChar(*stringToWrite++);
}

void GLCD_WriteAntiColorString(char * stringToWrite)
{
  while(*stringToWrite)
    GLCD_WriteAntiColorChar(*stringToWrite++);
}

//-------------------------------------------------------------------------------------------------
// Sets or clears pixel at (x,y)
//-------------------------------------------------------------------------------------------------
void GLCD_SetPixel(unsigned char x, unsigned char y, unsigned char color)
{
  unsigned char tmp;
  GLCD_GoTo(x, y/8);
  tmp = GLCD_ReadData();
  GLCD_GoTo(x, y/8);
  tmp = GLCD_ReadData();
  GLCD_GoTo(x, y/8);
  if(color)
    tmp |= (1 << (y%8));
  else
    tmp &= ~(1 << (y%8));
  GLCD_WriteData(tmp);
}

//-------------------------------------------------------------------------------------------------
// Displays bitmap at (x,y) and (dx,dy) size
//-------------------------------------------------------------------------------------------------
void GLCD_Bitmap(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy)
{
  unsigned char i, j;
  for(j = 0; j < dy / 8; j++)
  {
    GLCD_GoTo(x,y + j);
    for(i = 0; i < dx; i++) 
    {
      GLCD_WriteData(GLCD_ReadByteFromROMMemory(bmp++));
    } 
  }
}

void GLCD_BitmapAntiColor(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy)
{
  unsigned char i, j;
  for(j = 0; j < dy / 8; j++)
  {
    GLCD_GoTo(x,y + j);
    for(i = 0; i < dx; i++) 
    {
      GLCD_WriteData(~GLCD_ReadByteFromROMMemory(bmp++));
    } 
  }
}

//Çå³ýµ±Ç° ÐÐ
void GLCD_ClearScreen(void)
{
  unsigned char i,j;

  for(j=0;j<8;j++)
  {
    GLCD_GoTo(0,j);
    for(i = 0; i < KS0108_SCREEN_WIDTH; i++)
      GLCD_WriteData(0x00);
  }
}

//Çå³ýµ±Ç° ÐÐ
void GLCD_ClearScreenLiney(unsigned char y)
{
  unsigned char i;

  GLCD_GoTo(0,y);
  for(i = 0; i < KS0108_SCREEN_WIDTH; i++)
    GLCD_WriteData(0x00);
}

/******************************************************************************
                      start ¹â±ê²Ù×÷
*******************************************************************************/
typedef struct __CURSOR_STRUCT
{
  unsigned char cur_en;
  unsigned char cur_x;
  unsigned char cur_y;
  unsigned char min_x;
  unsigned char max_x;
  unsigned char min_y;
  unsigned char max_y;
  int*         pchar;
}stCur;

stCur stCurInformation;
int InputBuf[BUF_LEN] = {-1};

// ¹â±ê³õÊ¼»¯
void CursorInit(unsigned char xstart,unsigned char ystart,unsigned char xmax,unsigned char ymax)
{
  if(xmax > 21) while(1);
  if(ymax > 7 ) while(1);
  
  memset(InputBuf,-1,sizeof(InputBuf));
  
  stCurInformation.cur_en = CUR_DIS;
  stCurInformation.cur_x  = xstart;
  stCurInformation.cur_y  = ystart;
  stCurInformation.max_x  = xmax;
  stCurInformation.max_y  = ymax;
  stCurInformation.min_x  = xstart;
  stCurInformation.min_y  = ystart;
  stCurInformation.pchar  = InputBuf;
}

void CursorEnable(void)
{
  stCur* tmp_curmsg;
  
  tmp_curmsg = &stCurInformation;
  
  tmp_curmsg->cur_en = CUR_EN;
}

void CursorDisable(void)
{
  stCur* tmp_curmsg;
  
  tmp_curmsg = &stCurInformation;
  
  tmp_curmsg->cur_en = CUR_DIS;
//  memset(InputBuf,-1,sizeof(InputBuf));
}

// ¹â±êÉÁË¸
void CursorGlink(void)
{
  static unsigned char value = 0;
  stCur* tmp_curmsg;
  
  tmp_curmsg = &stCurInformation;
  
  if(CUR_DIS == tmp_curmsg->cur_en) return;
  
  GLCD_TextGoTo(tmp_curmsg->cur_x,tmp_curmsg->cur_y);
  value = ~value;
  GLCD_WriteData(value);
}

/*¹â±êmove*/
void CursorMove(unsigned char direction)
{
  stCur* tmp_curmsg;
  int* ptcb;
 
  ptcb = InputBuf;
  tmp_curmsg = &stCurInformation;  
  
  if(CUR_DIS == tmp_curmsg->cur_en) return;

  /*È¡ÏûÉÏµ±Ç°¹â±êÎ»ÖÃµÄÉÁË¸*/
  GLCD_TextGoTo(tmp_curmsg->cur_x,tmp_curmsg->cur_y);
  GLCD_WriteData(0x00);

  if(tmp_curmsg->cur_x > tmp_curmsg->max_x) return;
  if(tmp_curmsg->cur_x < tmp_curmsg->min_x) return;
  
  switch(direction)
  {
  case  MOVE_LEFT:
    if(tmp_curmsg->cur_x > tmp_curmsg->min_x)
    {
      tmp_curmsg->cur_x --;
      if((-1 != *(ptcb+tmp_curmsg->cur_x+1))&&(tmp_curmsg->cur_x+1 < tmp_curmsg->max_x))
      {
        GLCD_TextGoTo(tmp_curmsg->cur_x+1,tmp_curmsg->cur_y);
        GLCD_WriteChar(*((char*)(ptcb+tmp_curmsg->cur_x+1)));
      }
    }
    break;
  case MOVE_RIGHT:
    if(tmp_curmsg->cur_x < tmp_curmsg->max_x)
    {
      tmp_curmsg->cur_x ++;
      if((-1 != *(ptcb+tmp_curmsg->cur_x-1))&&(tmp_curmsg->cur_x-1 >= tmp_curmsg->min_x))
      {
        GLCD_TextGoTo(tmp_curmsg->cur_x-1,tmp_curmsg->cur_y);
        GLCD_WriteChar(*((char*)(ptcb+tmp_curmsg->cur_x-1)));
      }
    }
    break;
  case MOVE_UP:
    if(tmp_curmsg->cur_y < tmp_curmsg->min_y)
    {
      tmp_curmsg->cur_y --;
      memset(InputBuf,-1,sizeof(InputBuf));
    }
    break;
  case MOVE_DOWN:
    if(tmp_curmsg->cur_y < tmp_curmsg->max_y)
    {
      tmp_curmsg->cur_y ++;
      memset(InputBuf,-1,sizeof(InputBuf));
    }
    break;
  default:
    break;
  }
  
  GLCD_TextGoTo(tmp_curmsg->cur_x,tmp_curmsg->cur_y); //  ¹â±êÊ×´ÎÌî³ÉºÚÉ«
  GLCD_WriteData(0xff); 
}

// ÄÚ²¿º¯Êý ²åÈë×Ö·û Ëã·¨´¦Àí
static int _insert_char_to_tcb(char char_msg,stCur* pinfor)
{
  stCur* tmp_curmsg;

  int* ptcb;
  int  tmp_char; // ÖÐ¼ä±äÁ¿
  int  pro_char; // ÉÏÒ»Öµ
  unsigned int i = 0;
  
  tmp_curmsg = &stCurInformation;
  ptcb = InputBuf;
  
  if(((*(ptcb+pinfor->cur_x)) == -1)&&((pinfor->cur_x)<tmp_curmsg->max_x))  // µ±Ç°Î»ÖÃ¿ÉÒÔÐ´Èë
  {
    (*(ptcb+pinfor->cur_x)) = char_msg;
    
    return pinfor->cur_x;
  }
  else// ²åÈë
  {
    i = pinfor->cur_x;
    if(i < tmp_curmsg->max_x)
    {
      pro_char  = *(ptcb+i);
      *(ptcb+i) = char_msg;
      for(;i<tmp_curmsg->max_x;i++)//((sizeof(InputBuf))-1)
      {
        tmp_char = *(ptcb+i+1);
        *(ptcb+i+1) = pro_char;
        pro_char = tmp_char;

        InputBuf[tmp_curmsg->max_x] = -1;
      }
      
      return (i+1);
    }
    if(i == tmp_curmsg->max_x)
    {
      InputBuf[tmp_curmsg->max_x] = -1;
      return (i+1);
    }
  }
  
  return -1;
}

/*ÏÔÊ¾µ±Ç°ÊäÈëµÄ×Ö·û£¬²»±£´æ£¬ÒÔ±ãÑ¡Ôñ*/
void ShowScanfChar(char char_msg)
{
  stCur* tmp_curmsg = &stCurInformation;  
  
  if(tmp_curmsg->cur_x > tmp_curmsg->max_x-1) 
  {
    return;
  }
  if(tmp_curmsg->cur_x < tmp_curmsg->min_x) 
  {
    return;
  }

  
  GLCD_TextGoTo(tmp_curmsg->cur_x,tmp_curmsg->cur_y);
  GLCD_WriteAntiColorChar(char_msg);
}

/*ÊäÈë×Ö·û*/
void ScanfChar(char char_msg)
{
  int tmp_cnt = -1;
  unsigned char i =0;
  stCur* tmp_curmsg;
  int* ptcb;
  
  ptcb = InputBuf;
  tmp_curmsg = &stCurInformation;  
  
//  if(CUR_DIS == tmp_curmsg->cur_en) return;
  
  if(tmp_curmsg->cur_x > tmp_curmsg->max_x) 
  {
    return;
  }
  if(tmp_curmsg->cur_x < tmp_curmsg->min_x) 
  {
    return;
  }

  tmp_cnt = _insert_char_to_tcb(char_msg,tmp_curmsg);  // ×Ö·û¼ÇÂ¼ TCB
  
  if(-1 == tmp_cnt) return ;                       // ·µ»Ø´íÎó
  if(tmp_cnt >= tmp_curmsg->max_x);                // ·µ»Ø²»ÄÜ±È×î´óÖµ»¹´ó
  {
    tmp_cnt = tmp_curmsg->max_x;
  }
  for(i=tmp_curmsg->min_x;i<tmp_cnt;i++)           // Ë¢ÐÂ±¾ÐÐÒÑ¾­ÊäÈëµÄ×Ö·û
  {
    GLCD_TextGoTo(i,tmp_curmsg->cur_y);
    if(-1 == *((ptcb+i))) continue;
    GLCD_WriteChar(*((char*)(ptcb+i)));
  }
  
  if(tmp_curmsg->cur_x < tmp_curmsg->max_x)
  {
    tmp_curmsg->cur_x ++;
  }
}

/*ÄÚ²¿º¯Êý ¹â±êÉ¾³ýËã·¨´¦Àí*/
static int _delet_char_to_tcb(stCur* pinfor)
{
  unsigned int i = 0;
  stCur* tmp_curmsg;
  
  tmp_curmsg = &stCurInformation;
  pinfor->pchar = InputBuf; //Ö¸ÕëÖ¸ÏòµÚÒ»¸öÊý¾Ý
  
  if(pinfor->cur_x == pinfor->max_x)
  {
    *(pinfor->pchar+pinfor->cur_x-1) = -1;
    *(pinfor->pchar+pinfor->cur_x) = -1;
    
    return pinfor->cur_x;
  }
  else if((pinfor->cur_x < pinfor->max_x)&&(pinfor->cur_x > pinfor->min_x))
  {
    if(-1 == *(pinfor->pchar+pinfor->cur_x))//Èç¹ûºóÃæÎÞÊý¾ÝÁËÔòÖ±½ÓÉ¾³ý,¼´ÔÚÊý×éÎ²²¿
    {
      *(pinfor->pchar+pinfor->cur_x-1) = -1;
      GLCD_TextGoTo(pinfor->cur_x-1,tmp_curmsg->cur_y);
      GLCD_WriteChar(' ');
      GLCD_WriteData(0x00);
      return pinfor->cur_x;
    }
    else // ÖÐ¼äÖµÉ¾³ý
    {
      for(i=pinfor->cur_x;i<pinfor->max_x;i++)
      {
        *(pinfor->pchar+i-1) = *(pinfor->pchar+i);
        if(i+1 == pinfor->max_x)
        {
          *(pinfor->pchar+i) = -1;
          *(pinfor->pchar+i+1) = -1;
          GLCD_TextGoTo(pinfor->max_x-1,tmp_curmsg->cur_y);
          GLCD_WriteChar(' ');
          return (pinfor->cur_x - 1);
        }
        if(*(pinfor->pchar+i+1) == -1)
        {
          *(pinfor->pchar+i) = -1;
          GLCD_TextGoTo(i,tmp_curmsg->cur_y);
          GLCD_WriteChar(' ');
          return (pinfor->cur_x - 1);
        }
      }
    }
  }

  return -1;
}

void DeletChar(void)
{
  int tmp_cnt = -1;
  unsigned char i =0;
  stCur* tmp_curmsg;
  int* ptcb;
  
  ptcb = InputBuf;
  tmp_curmsg = &stCurInformation;  
  
  if(CUR_DIS == tmp_curmsg->cur_en) return;

  tmp_cnt = _delet_char_to_tcb(tmp_curmsg);  // ×Ö·û¼ÇÂ¼ TCB
  
  if(-1 == tmp_cnt) return ;                       // ·µ»Ø´íÎó

  for(i=tmp_curmsg->min_x+1;i<tmp_curmsg->max_x;i++)           // Ë¢ÐÂ±¾ÐÐÒÑ¾­ÊäÈëµÄ×Ö·û
  {
    GLCD_TextGoTo(i,tmp_curmsg->cur_y);
    if(-1 == *((ptcb+i)))
    {
      GLCD_WriteChar(' ');
      GLCD_WriteData(0x00);
      continue;
    } 
    GLCD_WriteChar(*((char*)(ptcb+i)));
  }
  
  if(tmp_curmsg->cur_x > tmp_curmsg->min_x)
  {
    tmp_curmsg->cur_x --;
  }
}

unsigned char CursorBehindData(void)//¼ì²é¹â±êºóÃæÊÇ·ñÓÐÊý¾Ý
{
  stCur* tmp_curmsg;
  
  tmp_curmsg = &stCurInformation;
  
  if(-1 == InputBuf[tmp_curmsg->cur_x])
  {
    return 0;
  }
  return 1;
}

/******************************************************************************
                      start ico²Ù×÷
*******************************************************************************/
typedef void (*fico) (unsigned char , unsigned char , unsigned char);

typedef struct __ICON_OPER
{
  unsigned char ico_status;
  unsigned char ico_value;
  unsigned char ico_count;
  fico          ico_call_fuc;
}ICO_TypeDef;

ICO_TypeDef strIcoTcb[ICO_NUM];

void IcoInit(void)
{
  unsigned char i;
  
  for(i=0;i<ICO_NUM;i++)
  {
    strIcoTcb[i].ico_status = ICO_DIS;
    strIcoTcb[i].ico_value  = 0xff;
    strIcoTcb[i].ico_count  = 0xff;
    strIcoTcb[i].ico_call_fuc = GLCD_SetPixel;
    strIcoTcb[i].ico_call_fuc(127-i,63,strIcoTcb[i].ico_value);
  }
}

/*Í¼±ê²Ù×÷*/
void IconOper(unsigned char ico_select, unsigned char ico_active,unsigned char ico_count)//char ico_value
{
  ICO_TypeDef* picotcb;
  
  if(ico_select > ICO_NUM) while(1);
  
  picotcb = &strIcoTcb[ico_select];
  
  picotcb->ico_status = ico_active;
  picotcb->ico_count  = ico_count;
}

void IcoGlintTaskEvent(void) // ico ÉÁË¸ÊÂ¼þ
{
  unsigned char i;
  ICO_TypeDef* picotcb;
  
  picotcb = strIcoTcb;
  
  for(i=0;i<ICO_NUM;i++)
  {
    if(0xff != (picotcb+i)->ico_count)
    {
      (picotcb+i)->ico_count --;
      if(!(picotcb+i)->ico_count) 
      {
        (picotcb+i)->ico_status = ICO_DIS;
      }
    }
    (picotcb+i)->ico_call_fuc(127-i,63,(picotcb+i)->ico_value);
    if(ICO_EN == (picotcb+i)->ico_status)
    {
      (picotcb+i)->ico_value = ~((picotcb+i)->ico_value);
    }
    else
    {
      (picotcb+i)->ico_value = 0xff;
    }
  }
}

int* GetLcdData(void)
{
  int temp_buf[BUF_LEN];
  
  memset(temp_buf,-1,BUF_LEN);
  
  if(0 == memcmp(temp_buf,InputBuf,BUF_LEN))
  {
    return (void*)0;
  }
  return InputBuf;
}

void ClearLcdStorData(void)
{
  memset(InputBuf,-1,sizeof(InputBuf));
}
//-------------------------------------------------------------------------------------------------
// End of file KS0108.c
//-------------------------------------------------------------------------------------------------





