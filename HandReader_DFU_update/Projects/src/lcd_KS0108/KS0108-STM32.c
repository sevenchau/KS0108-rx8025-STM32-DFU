//-------------------------------------------------------------------------------------------------
// Universal KS0108 driver library
// STM32 MCU low-level driver
//-------------------------------------------------------------------------------------------------
#include "stm32f10x.h"
#include "usartdriver.h"

#define KS0108_PORT_CTR         GPIOA
#define KS0108_RS               GPIO_Pin_1
#define KS0108_RW               GPIO_Pin_2
#define KS0108_EN               GPIO_Pin_3
#define KS0108_RSTB             GPIO_Pin_8
#define KS0108_LIGHT            GPIO_Pin_7

#define KS0108_PORT             GPIOC
#define KS0108_CS1              GPIO_Pin_9
#define KS0108_CS2              GPIO_Pin_8
// DB   PC0 - PC7

#define KS0108_D0               0

#define DISPLAY_REST            0x10
#define DISPLAY_STATUS_BUSY	0x80

#define DELAY                   100

extern unsigned char screen_x;
extern unsigned char screen_y;

GPIO_InitTypeDef GPIO_InitStructure;

void delay_us(u16 time)
{         
  u16 i=0;      
  while(time--)    
  {        
    i=10;  //自己定义       
    while(i--);
  } 
}

//-------------------------------------------------------------------------------------------------
// Delay function /for 8MHz/
//-------------------------------------------------------------------------------------------------
void GLCD_Delay(void)
{
  asm("nop");asm("nop");asm("nop");asm("nop");
//  asm("nop");asm("nop");asm("nop");asm("nop");
//  asm("nop");asm("nop");asm("nop");asm("nop");
//  asm("nop");asm("nop");asm("nop");asm("nop");
}

//-------------------------------------------------------------------------------------------------
// Enalbe Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_EnableController(unsigned char controller)
{
  switch(controller)
  {
  case 0 : GPIO_ResetBits(KS0108_PORT, KS0108_CS1); break;
  case 1 : GPIO_ResetBits(KS0108_PORT, KS0108_CS2); break;
  }
}
//-------------------------------------------------------------------------------------------------
// Disable Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_DisableController(unsigned char controller)
{
  switch(controller)
  {
  case 0 : GPIO_SetBits(KS0108_PORT, KS0108_CS1); break;
  case 1 : GPIO_SetBits(KS0108_PORT, KS0108_CS2); break;
  }
}
//-------------------------------------------------------------------------------------------------
// Read Status byte from specified controller (0-2)
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadStatus(unsigned char controller)
{
  unsigned char status;
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = 0xFF << KS0108_D0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;//
  GPIO_Init(KS0108_PORT, &GPIO_InitStructure);
  
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_RW);
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_RS);
  GLCD_EnableController(controller);
  GLCD_Delay();//delay_us(DELAY);
  
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_EN);
  GLCD_Delay();//delay_us(DELAY);
  status = ((GPIO_ReadInputData(KS0108_PORT) >> KS0108_D0) & 0xFF);
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_EN);
  GLCD_DisableController(controller);
  
  return status;
}

//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
  unsigned int cnt = 10000;
 
  while(GLCD_ReadStatus(controller)&DISPLAY_STATUS_BUSY)
  {
    cnt --;
    if(!cnt) break;
  }
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin  = (0xFF << KS0108_D0);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(KS0108_PORT, &GPIO_InitStructure);
  
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_RS | KS0108_RW);
  GLCD_Delay();//delay_us(DELAY);
  GLCD_EnableController(controller);
  GLCD_Delay();//delay_us(DELAY);

  GPIO_SetBits(KS0108_PORT, (commandToWrite << KS0108_D0));
  commandToWrite ^= 0xFF;
  GPIO_ResetBits(KS0108_PORT, (commandToWrite << KS0108_D0));
  GLCD_Delay();//delay_us(DELAY);
  
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_EN);
  GLCD_Delay();//delay_us(DELAY);
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_EN);
  GLCD_Delay();//delay_us(DELAY);
  GLCD_DisableController(controller);
}

//-------------------------------------------------------------------------------------------------
// Read data from current position
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
{
  unsigned int cnt = 10000;
  unsigned char tmp;
  while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY)
  {
    cnt --;
    if(!cnt) break;
  }

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = 0xFF << KS0108_D0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;////GPIO_Mode_IN_FLOATING;
  GPIO_Init(KS0108_PORT, &GPIO_InitStructure);
  
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_RS | KS0108_RW);
  
  GLCD_EnableController(screen_x / 64);
  GLCD_Delay();//delay_us(DELAY);
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_EN);
  GLCD_Delay();//delay_us(DELAY);
  tmp = ((GPIO_ReadInputData(KS0108_PORT) >> KS0108_D0) & 0xFF);
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_EN);
  GLCD_DisableController(screen_x / 64);
  screen_x++;
  
  return tmp;
}
//-------------------------------------------------------------------------------------------------
// Write data to current position
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite)
{
  unsigned int cnt = 10000;
  while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY)
  {
    cnt --;
    if(!cnt) break;
  }

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = (0xFF << KS0108_D0);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(KS0108_PORT, &GPIO_InitStructure);
  
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_RW);
  GLCD_Delay();//delay_us(DELAY);
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_RS);
  GLCD_Delay();//delay_us(DELAY);
  GPIO_SetBits(KS0108_PORT, (dataToWrite << KS0108_D0));
  dataToWrite ^= 0xFF;
  GPIO_ResetBits(KS0108_PORT, (dataToWrite << KS0108_D0));
  GLCD_Delay();//delay_us(DELAY);
  GLCD_EnableController(screen_x / 64);
  GLCD_Delay();//delay_us(DELAY);
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_EN);
  GLCD_Delay();//delay_us(DELAY);
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_EN);
  GLCD_Delay();//delay_us(DELAY);
  GLCD_DisableController(screen_x / 64);
  screen_x++;
}

//-------------------------------------------------------------------------------------------------
// 图标操作
//-------------------------------------------------------------------------------------------------
void GLCD_WriteIcon(unsigned char dataToWrite)
{
  unsigned int cnt = 10000;
  while(GLCD_ReadStatus(1)&DISPLAY_STATUS_BUSY)
  {
    cnt --;
    if(!cnt) break;
  }
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = (0xFF << KS0108_D0);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(KS0108_PORT, &GPIO_InitStructure);
  
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_RW);
  GLCD_Delay();//delay_us(DELAY);
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_RS);
  GLCD_Delay();//delay_us(DELAY);
  GPIO_SetBits(KS0108_PORT, (dataToWrite << KS0108_D0));
  dataToWrite ^= 0xFF;
  GPIO_ResetBits(KS0108_PORT, (dataToWrite << KS0108_D0));
  GLCD_Delay();//delay_us(DELAY);
  GLCD_EnableController(1);
  GLCD_Delay();//delay_us(DELAY);
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_EN);
  GLCD_Delay();//delay_us(DELAY);
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_EN);
  GLCD_Delay();//delay_us(DELAY);
  GLCD_DisableController(1);
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitializePorts(void)
{
vu32 i;

RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);

GPIO_StructInit(&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_2MHz;
GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_PP;
GPIO_Init(KS0108_PORT, &GPIO_InitStructure);

GPIO_StructInit(&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin   =  KS0108_RSTB | KS0108_RS | KS0108_RW | KS0108_EN|KS0108_LIGHT;
GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_2MHz;
GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_PP;
GPIO_Init(KS0108_PORT_CTR, &GPIO_InitStructure);

GPIO_StructInit(&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin   =  KS0108_LIGHT;
GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_2MHz;
GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_PP;//GPIO_Mode_IN_FLOATING;//
GPIO_Init(KS0108_PORT_CTR, &GPIO_InitStructure);

GPIO_Write(KS0108_PORT, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
GPIO_Write(KS0108_PORT_CTR,KS0108_RSTB|KS0108_RS | KS0108_RW|KS0108_EN|KS0108_LIGHT);//
}

void KS0108_LightOn(void)
{
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_LIGHT);//GPIO_ResetBits
//  delay_us(500);
}

void KS0108_LightOff(void)
{  
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_LIGHT);//GPIO_SetBits
  GPIO_WriteBit(KS0108_PORT_CTR, KS0108_LIGHT,Bit_RESET);
  delay_us(500);
}

void GLCD_ReBoot(void)
{
  GPIO_ResetBits(KS0108_PORT_CTR, KS0108_RSTB);
  
  delay_us(500);
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_RSTB);
  GPIO_SetBits(KS0108_PORT_CTR, KS0108_LIGHT);//GPIO_ResetBits
  Rf_Vdd_on();
  delay_us(500);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
  return *(ptr);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
