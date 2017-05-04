#include "usartdriver.h"
#include <stdio.h>

static void Rf_Vdd_onoffInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

//----------------------UART1 GPIOConfig-------------//
static void USART1_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable Usart GPIO clock A口及功能复用*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE); 
  // Configure USARTx_Tx as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // Configure USARTx_Rx as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//----------------------UART2 GPIOConfig-------------//
static void USART2_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
  
  // Configure USARTx_Tx as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // Configure USARTx_Rx as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//----------------------UART3 GPIOConfig-------------//
static void USART3_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
  
  // Configure USARTx_Tx as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // Configure USARTx_Rx as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*    USART Initialization    */
void USART1_Config(void)
{
  USART_InitTypeDef	USART_InitStructure;
  
  USART1_GPIOConfig();
  Rf_Vdd_onoffInit();
  
  USART_InitStructure.USART_BaudRate= 9600;//;//
  USART_InitStructure.USART_WordLength=USART_WordLength_8b;
  USART_InitStructure.USART_StopBits=USART_StopBits_1;
  USART_InitStructure.USART_Parity=USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
  USART_Init(USART1,&USART_InitStructure);
  /* Enable the USART Transmoit interrupt: this interrupt is generated when the
  USART1 transmit data register is empty */
//  USART_ITConfig(USART1, USART_IT_TC, DISABLE);
  /* Enable the USART Receive interrupt: this interrupt is generated when the
  USART1 receive data register is not empty */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1,ENABLE);				//Enable USART1
  
//  BSP_IntVectSet(BSP_INT_ID_USART1, BSP_IntHandlerUSART1); 
//  BSP_IntPrioSet(BSP_INT_ID_USART1,1);   
//  BSP_IntDis(BSP_INT_ID_USART1);
}

void USART2_Config(void)
{
  USART_InitTypeDef	USART_InitStructure;
  
  USART2_GPIOConfig();
  
  USART_InitStructure.USART_BaudRate= 9600;//115200;
  USART_InitStructure.USART_WordLength=USART_WordLength_8b;
  USART_InitStructure.USART_StopBits=USART_StopBits_1;
  USART_InitStructure.USART_Parity=USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
  USART_Init(USART2,&USART_InitStructure);
  /* Enable the USART Transmoit interrupt: this interrupt is generated when the
  USART2 transmit data register is empty */
//  USART_ITConfig(USART2, USART_IT_TC, ENABLE);
  /* Enable the USART Receive interrupt: this interrupt is generated when the
  USART2 receive data register is not empty */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART2,ENABLE);				//Enable USART2
  
//  BSP_IntVectSet(BSP_INT_ID_USART2, BSP_IntHandlerUSART2); 
//  BSP_IntPrioSet(BSP_INT_ID_USART2,1);   
//  BSP_IntDis(BSP_INT_ID_USART2);
}

void USART3_Config(void)
{
  USART_InitTypeDef	USART_InitStructure;
 
  USART3_GPIOConfig();
  
  USART_InitStructure.USART_BaudRate= 9600;//115200;//
  USART_InitStructure.USART_WordLength=USART_WordLength_8b;
  USART_InitStructure.USART_StopBits=USART_StopBits_1;
  USART_InitStructure.USART_Parity=USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
  USART_Init(USART3,&USART_InitStructure);
  /* Enable the USART Transmoit interrupt: this interrupt is generated when the
  USART2 transmit data register is empty */
//  USART_ITConfig(USART3, USART_IT_TC, ENABLE);
  /* Enable the USART Receive interrupt: this interrupt is generated when the
  USART2 receive data register is not empty */
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART3,ENABLE);				//Enable USART3
  
//  BSP_IntVectSet(BSP_INT_ID_USART3, BSP_IntHandlerUSART3); 
//  BSP_IntPrioSet(BSP_INT_ID_USART3,10);   
//  BSP_IntDis(BSP_INT_ID_USART3);
}

void USARTx_SetBaud(USART_TypeDef* channel, u32 baud)
{
  USART_InitTypeDef	USART_InitStructure;
  
  USART_InitStructure.USART_BaudRate=baud;
  USART_InitStructure.USART_WordLength=USART_WordLength_8b;
  USART_InitStructure.USART_StopBits=USART_StopBits_1;
  USART_InitStructure.USART_Parity=USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
  
  USART_Init(channel,&USART_InitStructure);
}

static void delay_us(u16 time)
{         
  u16 i=0;      
  while(time--)    
  {        
    i=10;  //自己定义       
    while(i--);
  } 
}

void Rf_Vdd_on(void)
{
  GPIO_SetBits(GPIOC, GPIO_Pin_12);
  GPIO_WriteBit(GPIOC, GPIO_Pin_12,Bit_SET);
  delay_us(500);
}

void Rf_Vdd_Off(void)
{
  GPIO_ResetBits(GPIOC, GPIO_Pin_12);
  GPIO_WriteBit(GPIOC, GPIO_Pin_12,Bit_RESET);
  delay_us(500);
}

void USARTx_IntDis(u8 channel)
{
//  BSP_IntDis(channel);
}

void USARTx_IntEn(u8 channel)
{
//  BSP_IntEn(channel);
}

void USARTx_SenBufMsg(USART_TypeDef* channel,const u8* pbuf_msg , u8 size)
{
  for(u8 i=0;i<size;i++)
  {
    while (USART_GetFlagStatus(channel, USART_FLAG_TC) == RESET);
    USART_SendData(channel, (uint8_t)(pbuf_msg[i]));
  }
}

//int fputc(int ch, FILE *f)
//{
//  USART_SendData(USART3, (uint8_t) ch);
//  while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
//  return ch;
//}

