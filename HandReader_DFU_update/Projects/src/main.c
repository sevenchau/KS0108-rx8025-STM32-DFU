/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Device Firmware Upgrade(DFU) demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/

#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_pwr.h"
#include "dfu_mal.h"

#include "usartdriver.h"
#include "timerdriver.h"
#include "KS0108.h"
#include "bmp.h"
/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
uint8_t DeviceState;
uint8_t DeviceStatus[6];
pFunction Jump_To_Application;
uint32_t JumpAddress;

uint32_t DataBarFlag = 0;
uint32_t DataBarCnt = 0;
uint32_t OnOffMode  = 0;
uint32_t DataUpgradeOk = 0;

typedef struct __PROGRESS_BAR
{
  uint8_t tim_number;
  uint8_t text_start_x;
  uint8_t text_start_y;
  uint8_t text_end_x;
  uint8_t cur_x;
  uint8_t clear_flag;
  uint8_t color_negated;
  char* pchr;
  uint32_t* pdata_cnt;
  uint32_t time_out;
}BAR_TypeDef;

BAR_TypeDef barPCToDevice =
{
  .pchr = ">",
  .text_start_x = 9,
  .text_end_x   = 13,
  .text_start_y = 2,
  .time_out     = 200,// 1ms为单位
  .tim_number   = 0,
  .cur_x        = 9,
  .clear_flag   = 0,
  .color_negated = 0,
  .pdata_cnt    = &OnOffMode
};

BAR_TypeDef barDeviceToPC =
{
  .pchr = "<",
  .text_start_x = 13,
  .text_end_x   = 9,
  .text_start_y = 3,
  .time_out     = 200,// 1ms为单位
  .tim_number   = 0,
  .cur_x        = 13,
  .clear_flag   = 0,
  .color_negated = 0,
  .pdata_cnt    = &OnOffMode
};

BAR_TypeDef barDataUpgrade =
{
  .pchr = " ",
  .text_start_x = 0,
  .text_end_x   = 21,
  .text_start_y = 6,
  .time_out     = 10,// 1ms为单位
  .tim_number   = 0,
  .cur_x        = 0,
  .clear_flag   = 0,
  .color_negated = 1,
  .pdata_cnt    = &DataBarCnt
};

void ProgressBarInit(BAR_TypeDef* pbar);
void ProgressBarRefresh(BAR_TypeDef* pbar);
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint8_t value=0;
uint8_t usb_timeout_num;
void DelayMs(u16 time);
int main(void)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;

//  SystemInit();
  
#if defined (USE_STM32L152D_EVAL)
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_OPTVERRUSR);
#endif
//  DFU_Button_Config();
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//使能 GPIOA 和复用功能时钟

  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;  //PA.0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//上拉输入GPIO_Mode_IPD;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化 IO
  
  
  /* Check if the Key push-button on STM3210x-EVAL Board is pressed */
  value = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
  if (value != 0x00)
  { /* Test if user code is programmed starting from address 0x8003000 */
    if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
    { /* Jump to user application */

      JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);//4
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) ApplicationAddress);
      Jump_To_Application();
    }
  } /* Otherwise enters DFU mode to allow user to program his application */

  /* Enter DFU mode */
  DeviceState = STATE_dfuERROR;
  DeviceStatus[0] = STATUS_ERRFIRMWARE;
  DeviceStatus[4] = DeviceState;

  Set_System();
  Set_USBClock();
  USB_Init();  
  
  USART1_Config();
  
  GLCD_Initialize();
  GLCD_ClearScreen();
  GLCD_Bitmap((char*)gImage_Computer,0,0,48,48);
  GLCD_BitmapAntiColor((char*)gImage_MOBILE,80,0,48,48);
  
  SysTickConfiguration();
  
  ProgressBarInit(&barPCToDevice);
  ProgressBarInit(&barDeviceToPC);
  ProgressBarInit(&barDataUpgrade);
  
  TimeAStartSet(1000,&usb_timeout_num,(void*)0,(void*)0,TIME_MODE_CYCLE);
  /* Main loop */
  while (1)
  {
    if(0 == OnOffMode)continue;
    ProgressBarRefresh(&barPCToDevice);
    ProgressBarRefresh(&barDeviceToPC);
    ProgressBarRefresh(&barDataUpgrade);    
    
    if(2 == DataUpgradeOk)
    {
      DataUpgradeOk = 0;
      DataBarFlag   = 0;
      GLCD_TextGoTo(barDataUpgrade.cur_x, barDataUpgrade.text_start_y);
      for(uint8_t i=barDataUpgrade.cur_x;i<=barDataUpgrade.text_end_x;i++)
        GLCD_WriteAntiColorString(barDataUpgrade.pchr);
      GLCD_TextGoTo(0, 7);
      GLCD_WriteString("Upgrade Success !!");
    }
    else if(1 == DataBarFlag)
    {
      GLCD_ClearScreenLiney(7);
    }
    
    if(DataBarFlag)
    {
      TimexEn(usb_timeout_num , 1000);
      
      if(GetTimeOutFlag(usb_timeout_num))
      {
        DataBarFlag   = 0;
        GLCD_ClearScreenLiney(7);
        GLCD_TextGoTo(0, 7);
        GLCD_WriteString("Upgrade Fail !!");
      }
    }
    else
    {
      TimexDis(usb_timeout_num);
    }
  }
}

void ErrorWaring(void)
{
  if(DataBarFlag)
  {
    DataBarFlag   = 0;
//    GLCD_ClearScreenLiney(7);
    GLCD_TextGoTo(0, 7);
    GLCD_WriteString("Upgrade Fail !!");
    DelayMs(60000);DelayMs(60000);
    DelayMs(60000);DelayMs(60000);
    DelayMs(60000);DelayMs(60000);
    DelayMs(60000);DelayMs(60000);
    return;
  }
  GLCD_ClearScreenLiney(6);
  GLCD_ClearScreenLiney(7);
}

void ProgressBarParaInit(void)
{
  barPCToDevice.cur_x        = 9;

  barDeviceToPC.cur_x        = 13;

  barDataUpgrade.cur_x        = 0;
//  
  DataBarFlag = 0;
  DataBarCnt = 0;
  OnOffMode  = 0;
  DataUpgradeOk = 0;
}

void ProgressBarInit(BAR_TypeDef* pbar)
{
  TimeAStartSet(pbar->time_out,&pbar->tim_number,(void*)0,(void*)0,TIME_MODE_CYCLE);
}

void ProgressBarRefresh(BAR_TypeDef* pbar)
{
  if(GetTimeOutFlag(pbar->tim_number)) // 超时需要更新进度条
  {
    if(pbar->tim_number == barPCToDevice.tim_number)
    {
      if(0 == *(pbar->pdata_cnt)) return;
    }
    else if(pbar->tim_number == barDeviceToPC.tim_number)
    {
      if(0 == *(pbar->pdata_cnt)) return;
    }
    else if(pbar->tim_number == barDataUpgrade.tim_number)
    {
      if(0 == *(pbar->pdata_cnt)) return;
      if(((void*)0 == pbar->pdata_cnt)||(0 == *(pbar->pdata_cnt)))
        return;
      *(pbar->pdata_cnt) /= 5;
    }

    if(pbar->cur_x == pbar->text_start_x)
    {
      if(!pbar->clear_flag)
      {
        if(pbar->text_start_x < pbar->text_end_x)// 往前递增
        {
          GLCD_TextGoTo(pbar->text_start_x, pbar->text_start_y);
          for(uint8_t i=pbar->text_start_x;i<=pbar->text_end_x;i ++ )
            GLCD_WriteString(" ");
//          GLCD_ClearScreenLiney(7);
        }
        else
        {
          GLCD_TextGoTo(pbar->text_end_x, pbar->text_start_y);
          for(uint8_t i=pbar->text_end_x;i<=pbar->text_start_x;i ++ )
            GLCD_WriteString(" ");
//          GLCD_ClearScreenLiney(7);
        }
        pbar->clear_flag = 1;
        return;
      }
      pbar->clear_flag = 0;
    }
    GLCD_TextGoTo(pbar->cur_x, pbar->text_start_y);
    if(1 == pbar->color_negated)
    {
      GLCD_WriteAntiColorString(pbar->pchr);
    }
    else
    {
      GLCD_WriteString(pbar->pchr);
    }
    
    if(pbar->text_start_x < pbar->text_end_x)// 往前递增
    {
      pbar->cur_x ++;
      if(pbar->cur_x > pbar->text_end_x) pbar->cur_x = pbar->text_start_x;
    }
    else
    {
      pbar->cur_x --;
      if(pbar->cur_x < pbar->text_end_x) pbar->cur_x = pbar->text_start_x;
    }
  }
}

void DelayMs(u16 time)
{         
  u16 i=0;      
  for(u16 j=0;j<time;j++)  
  {        
    for(i=0;i<60000;i++);
  } 
}
#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
