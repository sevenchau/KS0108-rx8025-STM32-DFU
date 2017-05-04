#ifndef __UARTDRIVE_H
#define __UARTDRIVE_H

#include "stm32f10x.h"
#include "device.h"
#include "includes.h"

#define  INT_ID_USART1   BSP_INT_ID_USART1
#define  INT_ID_USART2   BSP_INT_ID_USART2
#define  INT_ID_USART3   BSP_INT_ID_USART3  

void Rf_Vdd_on(void);
void Rf_Vdd_Off(void);

void USART1_Config(void);
void USART2_Config(void);
void USART3_Config(void);

void USARTx_SetBaud(USART_TypeDef* channel, u32 baud);
void USARTx_IntDis(u8 channel);
void USARTx_IntEn(u8 channel);

void USARTx_SenBufMsg(USART_TypeDef* channel,const u8* pbuf_msg , u8 size);

//int fputc(int ch, FILE *f);

#endif
