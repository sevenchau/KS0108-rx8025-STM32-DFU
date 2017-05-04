#include  "reboot.h"
#include "usartdriver.h"
#include "KS0108.h"

#define RCC_APB2Periph_WATCHDOG     RCC_APB2Periph_GPIOA
#define GPIOWATCHDOG                GPIOA
#define WATCHDOGPIN                 GPIO_Pin_12

void  reboot (void)
{
  while(1)
  {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    WWDG_SetWindowValue(65);
    WWDG_Enable(0x3f); 
    WWDG_SetWindowValue(65);
    WWDG_Enable(0x3f);
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    WWDG_SetWindowValue(65);
    WWDG_Enable(0x3f); 
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    WWDG_SetWindowValue(65);
    WWDG_Enable(0x3f);
  }
}

void Wdt_Init(void)
{
  // Enable write access to IWDG_PR and IWDG_RLR registers 
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //IWDG->KR = 0x5555
  // IWDG counter clock: 40KHz(LSI) / 64 = 0.625 KHz 
  IWDG_SetPrescaler(IWDG_Prescaler_32);   
  // Set counter reload value to 1250 
  IWDG_SetReload(0xFFF);           //IWDG->RLR =0xFFF
  //Red IWDG counter
  IWDG_ReloadCounter();   //IWDG->KR = 0xAAAA
  // Enable IWDG (the LSI oscillator will be enabled by hardware) 
  IWDG_Enable();   //IWDG->KR = 0xCCCC
  
}

//喂狗 
void Kick_Dog(void)
{
  //Reload IWDG counter 
  IWDG_ReloadCounter();   //IWDG->KR = 0xAAAA          
} 

///*
//SP706 是 Exar（原 Sipex）公司推出的低功耗、高可靠、低价格的 MCU复位监控芯片。
//以下是其关键特性： 
//z  分为 4 个子型号：SP706P、SP706R、SP706S、SP706T 
//z  复位输出：P为高电平有效，R/S/T 为低电平有效 
//z  精密的低电压监控：P/R为 2.63V、S为 2.93V、T为 3.08V 
//z  复位脉冲宽度：200ms（额定值） 
//z  独立的看门狗定时器：1.6 秒超时（额定值） 
//z  去抖 TTL/CMOS 手动复位输入（/MR 管脚） 
//*/
//void WDG_SP706_GPIO_Configration(void)
//{
//  GPIO_InitTypeDef  gpio_init;
//  
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_WATCHDOG, ENABLE);
//  
//  gpio_init.GPIO_Pin   = WATCHDOGPIN;
//  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
//  gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(GPIOWATCHDOG, &gpio_init);	
//}
//
//void WDG_SP706_Enable(void)
//{
//  GPIO_InitTypeDef  gpio_init;
//  
//  gpio_init.GPIO_Pin   = WATCHDOGPIN;
//  gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
//  GPIO_Init(GPIOWATCHDOG, &gpio_init);
//}
//
//void WDG_SP706_Feed(void)
//{
//  u8 reg_val;
//  reg_val = GPIO_ReadOutputDataBit(GPIOWATCHDOG , WATCHDOGPIN);
//  if(reg_val == 1)
//  {
//    GPIO_ResetBits(GPIOWATCHDOG, WATCHDOGPIN);
//    return;
//  }
//  GPIO_SetBits(GPIOWATCHDOG, WATCHDOGPIN);   
//}

void Sys_Standby(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //使能PWR外设时钟
  PWR_WakeUpPinCmd(ENABLE); //使能唤醒管脚功能
  PWR_EnterSTANDBYMode();  //进入待命（STANDBY）模式
}

//系统进入待机模式
void Sys_Enter_Standby(void)
{ 
  KS0108_LightOff();
  GLCD_WriteCommand(0x3E, 0);
  GLCD_WriteCommand(0x3E, 1);
  GLCD_WriteCommand(0x3E, 0);
  GLCD_WriteCommand(0x3E, 1);
  GLCD_WriteCommand(0x3E, 0);
  GLCD_WriteCommand(0x3E, 1);
  Rf_Vdd_Off();
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,DISABLE);  //复位所有 IO 口
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_USART1,DISABLE);  
  WKUP_Init();
  Sys_Standby();
}

//检测 WKUP 脚的信号
//返回值 1:连续按下 3s 以上
// 0:错误的触发
u8 IntCheckWakeUp(void)
{
  u8 t=0;
  u8 tx=0;//记录松开的次数
  while(1)
  {
    if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))//已经按下了
    {
      t++;
      tx=0;
    }
    else
    {
      tx++; //超过 1000ms 内没有 WKUP 信号
      if(tx>5)
      {
        return 0;//错误的按键,按下次数不够
      }
    }
    OSTimeDly (30);
//    delay_ms(30);
    if(t>=100)//按下超过 3 秒钟
    {
      return 1; //按下 3s 以上了
    }
  }
}

//PA0 WKUP 唤醒初始化
void WKUP_Init(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//使能 GPIOA 和复用功能时钟

  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;  //PA.0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//上拉输入GPIO_Mode_IN_FLOATING;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化 IO
  //使用外部中断方式
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
  //中断线 0 连接 GPIOA.0
  EXTI_InitStructure.EXTI_Line = EXTI_Line0; //设置按键所有的外部线路
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //设外外部中断模
  //式:EXTI 线路为中断请求
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Rising; //上升沿触发EXTI_Trigger_Rising_Falling;//
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); // 初始化外部中断
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //使能按键所在的外部中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //先占优先级 2 级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //从优先级 2 级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化 NVIC
  
//  BSP_IntVectSet(BSP_INT_ID_EXTI0, BSP_IntHandlerEXTI0); 
//  BSP_IntPrioSet(BSP_INT_ID_EXTI0,2);   
//  BSP_IntEn(BSP_INT_ID_EXTI0);
//  
//  PWR_ClearFlag(PWR_FLAG_SB);//StandBy flag
  
//  if(IntCheckWakeUp()==0) 
//  {
//    Sys_Standby(); //不是开机,进入待机模式
//  }
}

void StandbyIntEnable(void)
{
  BSP_IntEn(BSP_INT_ID_EXTI0);
}