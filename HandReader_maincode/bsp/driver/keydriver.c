#include "keydriver.h"
#include "string.h"
#include "includes.h"

#define RCC_APB2Periph_KEY     RCC_APB2Periph_GPIOB
#define GPIO_KEY                GPIOB

#define KEYL1_PIN                 GPIO_Pin_0
#define KEYL2_PIN                 GPIO_Pin_1
#define KEYL3_PIN                 GPIO_Pin_2

#define KEYH1_PIN                 GPIO_Pin_10
#define KEYH2_PIN                 GPIO_Pin_11
#define KEYH3_PIN                 GPIO_Pin_12
#define KEYH4_PIN                 GPIO_Pin_13
#define KEYH5_PIN                 GPIO_Pin_14
#define KEYH6_PIN                 GPIO_Pin_15

void GPIO_KeyConfInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
//  EXTI_InitTypeDef EXTI_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_KEY|RCC_APB2Periph_AFIO , ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = KEYH1_PIN | KEYH2_PIN | KEYH3_PIN | KEYH4_PIN | KEYH5_PIN | KEYH6_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_KEY, &GPIO_InitStructure);
  
//  GPIO_InitStructure.GPIO_Pin = KEYL1_PIN | KEYL2_PIN | KEYL3_PIN ;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIO_KEY, &GPIO_InitStructure);
//  
//  GPIO_SetBits(GPIO_KEY, KEYL1_PIN | KEYL2_PIN | KEYL3_PIN);
  
  GPIO_InitStructure.GPIO_Pin = KEYL1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPD;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_KEY, &GPIO_InitStructure);
  
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//  EXTI_ClearFlag(EXTI_Line0);
//  
//  BSP_IntVectSet(BSP_INT_ID_EXTI0, BSP_IntHandlerEXTI0); 
//  BSP_IntPrioSet(BSP_INT_ID_EXTI0,1);   
//  BSP_IntDis(BSP_INT_ID_EXTI0);
  
  GPIO_InitStructure.GPIO_Pin = KEYL2_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;////GPIO_Mode_IPD;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_KEY, &GPIO_InitStructure);
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//  EXTI_ClearFlag(EXTI_Line1);
  
//  BSP_IntVectSet(BSP_INT_ID_EXTI1, BSP_IntHandlerEXTI1); 
//  BSP_IntPrioSet(BSP_INT_ID_EXTI1,1);   
//  BSP_IntDis(BSP_INT_ID_EXTI1);
  
  GPIO_InitStructure.GPIO_Pin = KEYL3_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPD;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_KEY, &GPIO_InitStructure);
  
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource2);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//  EXTI_ClearFlag(EXTI_Line2);
//  
//  BSP_IntVectSet(BSP_INT_ID_EXTI2, BSP_IntHandlerEXTI2); 
//  BSP_IntPrioSet(BSP_INT_ID_EXTI2,1);   
//  BSP_IntDis(BSP_INT_ID_EXTI2);
}

void KeyScanEnable(void)
{
//  BSP_IntEn(BSP_INT_ID_EXTI0);
//  BSP_IntEn(BSP_INT_ID_EXTI1);
//  BSP_IntEn(BSP_INT_ID_EXTI2);
}

void KeyScanDisable(void)
{
//  BSP_IntDis(BSP_INT_ID_EXTI0);
//  BSP_IntDis(BSP_INT_ID_EXTI1);
//  BSP_IntDis(BSP_INT_ID_EXTI2);
}


void GPIO_KeyScanLineInit(void) //设置为 输出高电平  以便扫描
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_KEY , ENABLE);
  
  KeyScanDisable();
  
  GPIO_InitStructure.GPIO_Pin = KEYL1_PIN | KEYL2_PIN | KEYL3_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_KEY, &GPIO_InitStructure);
  
  GPIO_SetBits(GPIO_KEY, KEYL1_PIN | KEYL2_PIN | KEYL3_PIN);
}

void GPIO_InterruptKeyLineInit(void) // 设置 为中断  以便捕捉按键中断
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_KEY|RCC_APB2Periph_AFIO , ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = KEYL1_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_KEY, &GPIO_InitStructure);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  EXTI_ClearFlag(EXTI_Line0);
  
  BSP_IntVectSet(BSP_INT_ID_EXTI0, BSP_IntHandlerEXTI0); 
  BSP_IntPrioSet(BSP_INT_ID_EXTI0,1);   
  BSP_IntEn(BSP_INT_ID_EXTI0);

  GPIO_InitStructure.GPIO_Pin = KEYL2_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_KEY, &GPIO_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  EXTI_ClearFlag(EXTI_Line1);
  
  BSP_IntVectSet(BSP_INT_ID_EXTI1, BSP_IntHandlerEXTI1); 
  BSP_IntPrioSet(BSP_INT_ID_EXTI1,1);   
  BSP_IntEn(BSP_INT_ID_EXTI1);

  GPIO_InitStructure.GPIO_Pin = KEYL3_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_KEY, &GPIO_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource2);
  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  EXTI_ClearFlag(EXTI_Line2);
  
  BSP_IntVectSet(BSP_INT_ID_EXTI2, BSP_IntHandlerEXTI2); 
  BSP_IntPrioSet(BSP_INT_ID_EXTI2,1);   
  BSP_IntEn(BSP_INT_ID_EXTI2);
}

INT16U CaptureKeyNum(fcb call_osdelay)
{
  INT8U i;
  INT16U pin_tcb[3] = {KEYL1_PIN,KEYL2_PIN,KEYL3_PIN};
  INT16U pin_value = 0;
  
//  KeyScanDisable();
//  GPIO_KeyScanLineInit();  // 初始化 该行向 GPIO 为推完输出高电平
//  call_osdelay(2);
  for(i=0;i<3;i++)
  {
    GPIO_SetBits(GPIO_KEY, KEYL1_PIN | KEYL2_PIN | KEYL3_PIN ); 
    call_osdelay(2);
    GPIO_ResetBits(GPIO_KEY,pin_tcb[i]);
    call_osdelay(2);
    pin_value = (GPIO_ReadInputData(GPIO_KEY)&0xFC00);//
    if(0xFC00 != pin_value)
    {  
      break;
    }
    else
    {
      pin_value = 0;
    }
  }
  
  if((0 != pin_value) && (0xFC00 != pin_value))
  {
    call_osdelay(15); // 挂机10ms 防抖处理 
    
    GPIO_SetBits(GPIO_KEY, KEYL1_PIN | KEYL2_PIN | KEYL3_PIN ); 
    call_osdelay(2);
    GPIO_ResetBits(GPIO_KEY,pin_tcb[i]);
    call_osdelay(2);
    
    if(pin_value == (GPIO_ReadInputData(GPIO_KEY)&0xFC00))
    {
      pin_value |= pin_tcb[i];
    }
  }

  GPIO_SetBits(GPIO_KEY, KEYL1_PIN | KEYL2_PIN | KEYL3_PIN ); 
//  GPIO_InterruptKeyLineInit(); // IO 口设置回 中断
//  KeyScanEnable();
  call_osdelay(2);
  
  return pin_value;
}
