/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*
* Filename      : bsp_int.c
* Version       : V1.00
* Programmer(s) : 
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_INT_MODULE
#include <bsp.h>
#include "queue.h"
#include "string.h"
#include "serve.h"
#include  "rfdriver_reader.h"
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_INT_SRC_NBR                                 68


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

static  CPU_FNCT_VOID  BSP_IntVectTbl[BSP_INT_SRC_NBR];


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_IntHandler     (CPU_DATA  int_id);
static  void  BSP_IntHandlerDummy(void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              BSP_IntClr()
*
* Description : Clear interrupt.
*
* Argument(s) : int_id      Interrupt to clear.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) An interrupt does not need to be cleared within the interrupt controller.
*********************************************************************************************************
*/

void  BSP_IntClr (CPU_DATA  int_id)
{

}


/*
*********************************************************************************************************
*                                              BSP_IntDis()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id      Interrupt to disable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDis (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcDis(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}


/*
*********************************************************************************************************
*                                               BSP_IntEn()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id      Interrupt to enable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEn (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcEn(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectSet (CPU_DATA       int_id,
                      CPU_FNCT_VOID  isr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR   cpu_sr;
#endif


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntPrioSet()
*
* Description : Assign ISR priority.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               prio        Priority to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntPrioSet (CPU_DATA    int_id,
                      CPU_INT08U  prio)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR    cpu_sr;
#endif


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        CPU_IntSrcPrioSet(int_id + 16, prio);
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           INTERNAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              BSP_IntInit()
*
* Description : Initialize interrupts:
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntInit (void)
{
    CPU_DATA  int_id;


    for (int_id = 0; int_id < BSP_INT_SRC_NBR; int_id++) {
        BSP_IntVectSet(int_id, BSP_IntHandlerDummy);
    }
}


/*
*********************************************************************************************************
*                                        BSP_IntHandler####()
*
* Description : Handle an interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

//void  BSP_IntHandlerWWDG          (void)  { BSP_IntHandler(BSP_INT_ID_WWDG);            }
void  BSP_IntHandlerPVD           (void)  { BSP_IntHandler(BSP_INT_ID_PVD);             }
void  BSP_IntHandlerTAMPER        (void)  { BSP_IntHandler(BSP_INT_ID_TAMPER);          }
void  BSP_IntHandlerRTC           (void)  { BSP_IntHandler(BSP_INT_ID_RTC);             }
void  BSP_IntHandlerFLASH         (void)  { BSP_IntHandler(BSP_INT_ID_FLASH);           }
void  BSP_IntHandlerRCC           (void)  { BSP_IntHandler(BSP_INT_ID_RCC);             }
//void  BSP_IntHandlerEXTI0         (void)  { BSP_IntHandler(BSP_INT_ID_EXTI0);           }
//void  BSP_IntHandlerEXTI1         (void)  { BSP_IntHandler(BSP_INT_ID_EXTI1);           }
//void  BSP_IntHandlerEXTI2         (void)  { BSP_IntHandler(BSP_INT_ID_EXTI2);           }
void  BSP_IntHandlerEXTI3         (void)  { BSP_IntHandler(BSP_INT_ID_EXTI3);           }
void  BSP_IntHandlerEXTI4         (void)  { BSP_IntHandler(BSP_INT_ID_EXTI4);           }
//void  BSP_IntHandlerDMA1_CH1      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH1);        }
void  BSP_IntHandlerDMA1_CH2      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH2);        }
void  BSP_IntHandlerDMA1_CH3      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH3);        }
void  BSP_IntHandlerDMA1_CH4      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH4);        }
void  BSP_IntHandlerDMA1_CH5      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH5);        }
void  BSP_IntHandlerDMA1_CH6      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH6);        }
void  BSP_IntHandlerDMA1_CH7      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH7);        }
void  BSP_IntHandlerADC1_2        (void)  { BSP_IntHandler(BSP_INT_ID_ADC1_2);          }
void  BSP_IntHandlerCAN1_TX       (void)  { BSP_IntHandler(BSP_INT_ID_CAN1_TX);         }
void  BSP_IntHandlerCAN1_RX0      (void)  { BSP_IntHandler(BSP_INT_ID_CAN1_RX0);        }
void  BSP_IntHandlerCAN1_RX1      (void)  { BSP_IntHandler(BSP_INT_ID_CAN1_RX1);        }
void  BSP_IntHandlerCAN1_SCE      (void)  { BSP_IntHandler(BSP_INT_ID_CAN1_SCE);        }
void  BSP_IntHandlerEXTI9_5       (void)  { BSP_IntHandler(BSP_INT_ID_EXTI9_5);         }
void  BSP_IntHandlerTIM1_BRK      (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_BRK);        }
void  BSP_IntHandlerTIM1_UP       (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_UP);         }
void  BSP_IntHandlerTIM1_TRG_COM  (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_TRG_COM);    }
void  BSP_IntHandlerTIM1_CC       (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_CC);         }
//void  BSP_IntHandlerTIM2          (void)  { BSP_IntHandler(BSP_INT_ID_TIM2);            }
void  BSP_IntHandlerTIM3          (void)  { BSP_IntHandler(BSP_INT_ID_TIM3);            }
void  BSP_IntHandlerTIM4          (void)  { BSP_IntHandler(BSP_INT_ID_TIM4);            }
void  BSP_IntHandlerI2C1_EV       (void)  { BSP_IntHandler(BSP_INT_ID_I2C1_EV);         }
void  BSP_IntHandlerI2C1_ER       (void)  { BSP_IntHandler(BSP_INT_ID_I2C1_ER);         }
void  BSP_IntHandlerI2C2_EV       (void)  { BSP_IntHandler(BSP_INT_ID_I2C2_EV);         }
void  BSP_IntHandlerI2C2_ER       (void)  { BSP_IntHandler(BSP_INT_ID_I2C2_ER);         }
void  BSP_IntHandlerSPI1          (void)  { BSP_IntHandler(BSP_INT_ID_SPI1);            }
void  BSP_IntHandlerSPI2          (void)  { BSP_IntHandler(BSP_INT_ID_SPI2);            }
//void  BSP_IntHandlerUSART1        (void)  { BSP_IntHandler(BSP_INT_ID_USART1);          }
//void  BSP_IntHandlerUSART2        (void)  { BSP_IntHandler(BSP_INT_ID_USART2);          }
//void  BSP_IntHandlerUSART3        (void)  { BSP_IntHandler(BSP_INT_ID_USART3);          }
void  BSP_IntHandlerEXTI15_10     (void)  { BSP_IntHandler(BSP_INT_ID_EXTI15_10);       }
void  BSP_IntHandlerRTCAlarm      (void)  { BSP_IntHandler(BSP_INT_ID_RTCAlarm);        }
void  BSP_IntHandlerUSBWakeUp     (void)  { BSP_IntHandler(BSP_INT_ID_USBWakeUp);       }
void  BSP_IntHandlerTIM8_BRK      (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_BRK);        }
void  BSP_IntHandlerTIM8_UP       (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_UP);         }
void  BSP_IntHandlerTIM8_TRG_COM  (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_TRG_COM);    }
void  BSP_IntHandlerTIM8_CC       (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_CC);         }
void  BSP_IntHandlerADC3          (void)  { BSP_IntHandler(BSP_INT_ID_ADC3);            }
void  BSP_IntHandlerFSMC          (void)  { BSP_IntHandler(BSP_INT_ID_FSMC);            }
void  BSP_IntHandlerSDIO          (void)  { BSP_IntHandler(BSP_INT_ID_SDIO);            }
void  BSP_IntHandlerTIM5          (void)  { BSP_IntHandler(BSP_INT_ID_TIM5);            }
void  BSP_IntHandlerSPI3          (void)  { BSP_IntHandler(BSP_INT_ID_SPI3);            }
void  BSP_IntHandlerUART4         (void)  { BSP_IntHandler(BSP_INT_ID_UART4);           }
void  BSP_IntHandlerUART5         (void)  { BSP_IntHandler(BSP_INT_ID_UART5);           }
void  BSP_IntHandlerTIM6          (void)  { BSP_IntHandler(BSP_INT_ID_TIM6);            }
void  BSP_IntHandlerTIM7          (void)  { BSP_IntHandler(BSP_INT_ID_TIM7);            }
void  BSP_IntHandlerDMA2_CH1      (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH1);        }
void  BSP_IntHandlerDMA2_CH2      (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH2);        }
void  BSP_IntHandlerDMA2_CH3      (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH3);        }
void  BSP_IntHandlerDMA2_CH4      (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH4);        }
void  BSP_IntHandlerDMA2_CH5      (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH5);        }
void  BSP_IntHandlerETH           (void)  { BSP_IntHandler(BSP_INT_ID_ETH);             }           
void  BSP_IntHandlerETH_WKUP      (void)  { BSP_IntHandler(BSP_INT_ID_ETH_WKUP);        }      
void  BSP_IntHandlerCAN2_TX       (void)  { BSP_IntHandler(BSP_INT_ID_CAN2_TX);         }         
void  BSP_IntHandlerCAN2_RX0      (void)  { BSP_IntHandler(BSP_INT_ID_CAN2_RX0);        }        
void  BSP_IntHandlerCAN2_RX1      (void)  { BSP_IntHandler(BSP_INT_ID_CAN2_RX1);        }        
void  BSP_IntHandlerCAN2_SCE      (void)  { BSP_IntHandler(BSP_INT_ID_CAN2_SCE);        }        
void  BSP_IntHandlerOTG_FS        (void)  { BSP_IntHandler(BSP_INT_ID_OTG_FS);          }     


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          BSP_IntHandler()
*
* Description : Central interrupt handler.
*
* Argument(s) : int_id          Interrupt that will be handled.
*
* Return(s)   : none.
*
* Caller(s)   : ISR handlers.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandler (CPU_DATA  int_id)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR         cpu_sr;
#endif
    CPU_FNCT_VOID  isr;


    CPU_CRITICAL_ENTER();                                       /* Tell uC/OS-II that we are starting an ISR            */
    OSIntNesting++;
    CPU_CRITICAL_EXIT();

    if (int_id < BSP_INT_SRC_NBR) {
        isr = BSP_IntVectTbl[int_id];
        if (isr != (CPU_FNCT_VOID)0) {
            isr();
        }
    }

    OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR            */
}

/*
*********************************************************************************************************
*                                        BSP_IntHandlerDummy()
*
* Description : Dummy interrupt handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_IntHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandlerDummy (void)
{

}

void BSP_IntHandlerWWDG(void)
{
}

/*dma adc 采样中断 */
void BSP_IntHandlerDMA1_CH1(void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif
  OS_ENTER_CRITICAL();                                     
  OSIntNesting++;
  OS_EXIT_CRITICAL();
  
  if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
  {
    BSP_IntDis(BSP_INT_ID_DMA1_CH1);
  }
  
  OSIntExit();
}

void  BSP_IntHandlerEXTI0(void) //按键中断 行1
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif
  OS_ENTER_CRITICAL();                                     
  OSIntNesting++;
  OS_EXIT_CRITICAL();

  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line0);
    while(OS_ERR_NONE != OSSemPost (SemNormalTcb[KeyStandbySem]));
  }
  
  OSIntExit();
}

void  BSP_IntHandlerEXTI1(void) //按键中断 行2
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif
  OS_ENTER_CRITICAL();                                     
  OSIntNesting++;
  OS_EXIT_CRITICAL();
  
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
//    BSP_IntEn(BSP_INT_ID_EXTI1);
    EXTI_ClearITPendingBit(EXTI_Line1);
    OSSemPost (SemNormalTcb[KeyStandbySem]);
  }
  
  OSIntExit();
}

void BSP_IntHandlerEXTI2(void) // 按键中断 行3
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif
  OS_ENTER_CRITICAL();                                     
  OSIntNesting++;
  OS_EXIT_CRITICAL();
  
  if(EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
//    BSP_IntEn(BSP_INT_ID_EXTI2);
    EXTI_ClearITPendingBit(EXTI_Line2);
    OSSemPost (SemNormalTcb[KeyStandbySem]);
  }
  
  OSIntExit();
}

void BSP_IntHandlerTIM2(void)
{

#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif
  OS_ENTER_CRITICAL();                                     
  OSIntNesting++;
  OS_EXIT_CRITICAL();
  
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
  }
  
  OSIntExit();
}

void BSP_IntHandlerUSART1(void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif
  OS_ENTER_CRITICAL();                                     
  OSIntNesting++;
  OS_EXIT_CRITICAL();
  
  INT8U tmp;
  static INT8U rxbuf[RF_ELEMENT] = {0};
  static INT8U i = 0;
  static INT8U status = 0;
  
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收中断
  {
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    
    tmp = USART_ReceiveData(USART1);
    switch(status)
    {
    case 0:
      {
        if(0x68 == tmp) // 开始
        {
          status = 1;
        }
        else
        {
          break;
        }
      }
    case 1:
      {
        rxbuf[i++] = tmp;
        if(i > RF_ELEMENT) 
        {
          i = 0;
          status = 0;
          memset(rxbuf,0,sizeof(rxbuf));
          break;
        }
        if((0x68 == rxbuf[0])  && (0x68 ==rxbuf[7]) && (0x16 == rxbuf[rxbuf[9]+11])) // 结束
        {
          i = 0;
          status = 0;
          RfEventEnqueue(rxbuf);  
          memset(rxbuf,0,sizeof(rxbuf));
          OSSemPost (SemNormalTcb[RfRxIntSem]);// 无线接收中断事件
          break;
        }
        break;
      }
    default:
      break;
    }
  }
  
  OSIntExit();
}

void BSP_IntHandlerUSART2(void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif
  OS_ENTER_CRITICAL();                                     
  OSIntNesting++;
  OS_EXIT_CRITICAL();
  
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  }
  
  OSIntExit();
}

void BSP_IntHandlerUSART3(void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif
  OS_ENTER_CRITICAL();                                     
  OSIntNesting++;
  OS_EXIT_CRITICAL();
  
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  }
  
   OSIntExit();
}