#include "i2c_rx8025tdriver.h"
#include "includes.h"

#define RCC_APB2Periph_RTC     RCC_APB2Periph_GPIOB

#define GPIO_RTC                GPIOB

#define PIN_SCL                 GPIO_Pin_6
#define PIN_SDA                 GPIO_Pin_7

#define I2C2_SLAVE_ADDRESS7     0x7f

#define READ_MSG                0x65
#define WRITE_MSG               0x64

#define REGADDR_SEC             0x00
#define REGADDR_MIN             0x01
#define REGADDR_HOUR            0x02
#define REGADDR_WEEK            0x03
#define REGADDR_DAY             0x04
#define REGADDR_MONTH           0x05
#define REGADDR_YEAR            0x06
#define REGADDR_MIN_ALARM       0x07
#define REGADDR_HOUR_ALARM      0x08
#define REGADDR_WEEK_ALARM      0x09
#define REGADDR_DAY_ALARM       0x0A
#define REGADDR_TIM_CNT0        0x0B
#define REGADDR_TIM_CNT1        0x0C
#define REGADDR_EXTEN           0x0D
  #define EXTEN_TEST            (1<<7)
  #define EXTEN_WADA            (1<<6)
  #define EXTEN_USEL            (1<<5)
  #define EXTEN_TE              (1<<4)
  #define EXTEN_FSEL1           (1<<3)
  #define EXTEN_FSEL0           (1<<2)
  #define EXTEN_TSEL1           (1<<1)
  #define EXTEN_TSEL0           (1<<0)
#define REGADDR_FLAG            0x0E
  #define FLAG_UF               (1<<5)
  #define FLAG_TF               (1<<4)
  #define FLAG_AF               (1<<3)
  #define FLAG_VLF              (1<<1)
  #define FLAG_VDET             (1<<0)
#define REGADDR_CONTROL         0x0F
  #define CONTR_CSEL1           (1<<7)
  #define CONTR_CSEL0           (1<<6)
  #define CONTR_UIE             (1<<5)
  #define CONTR_TIE             (1<<4)
  #define CONTR_AIE             (1<<3)
  #define CONTR_RESET           (1<<0)

/**********************************************************************
                           IIC配置
*********************************************************************/
void I2C_ConfigInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  I2C_InitTypeDef I2C_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  /* Configure IO connected to IIC*********************/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIO_RTC, &GPIO_InitStructure);
  
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2C2_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000;

  I2C_Init(I2C1, &I2C_InitStructure);
  
  I2C_Cmd(I2C1, ENABLE); 
}
/*********************************************************************
IIC 写一个字节
*********************************************************************/
void I2C_WriteByte(unsigned char id,unsigned char write_address,unsigned char byte)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif 
  
  OS_ENTER_CRITICAL();
  
  I2C_GenerateSTART(I2C1,ENABLE);
  //产生起始条件
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  //等待ACK
  I2C_Send7bitAddress(I2C1,id,I2C_Direction_Transmitter);
  //向设备发送设备地址
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  //等待ACK
  I2C_SendData(I2C1, write_address);
  //寄存器地址
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  //等待ACK
  I2C_SendData(I2C1, byte);
  //发送数据
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  //发送完成
  I2C_GenerateSTOP(I2C1, ENABLE);
  //产生结束信号
  
  OS_EXIT_CRITICAL();
}
/*********************************************************************
IIC读数据
*********************************************************************/
unsigned char I2C_ReadByte(unsigned char  id, unsigned char read_address)
{  
  unsigned char temp; 	
  
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
  CPU_SR         cpu_sr;
#endif 
  
  OS_ENTER_CRITICAL();
  
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
  //等待I2C
  I2C_GenerateSTART(I2C1, ENABLE);
  //产生起始信号
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  //EV5
  I2C_Send7bitAddress(I2C1, id, I2C_Direction_Transmitter);
  //发送地址
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  //EV6
  I2C_Cmd(I2C1, ENABLE);
  //重新设置可以清楚EV6
  I2C_SendData(I2C1, read_address);  
  //发送读得地址
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  //EV8 
  I2C_GenerateSTART(I2C1, ENABLE);
  //重新发送
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  //EV5
  I2C_Send7bitAddress(I2C1, id, I2C_Direction_Receiver);
  //发送读地址
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  //EV6  
  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_GenerateSTOP(I2C1, ENABLE);
  //关闭应答和停止条件产生
  while(!(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)));
  
  temp = I2C_ReceiveData(I2C1);
  
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  
  OS_EXIT_CRITICAL();
  
  return temp;
}

INT8U RX8025T_Init(void)
{
  I2C_ConfigInit();
  
  if(FLAG_VLF == (I2C_ReadByte(READ_MSG,REGADDR_FLAG)&FLAG_VLF))
  {
    
    
    return 1;
  }
  
  I2C_WriteByte(WRITE_MSG,REGADDR_FLAG,0x00);
  I2C_WriteByte(WRITE_MSG,REGADDR_CONTROL,0x40);//0100 0000
  
  return 0;
}

INT8U ReadTimeReg(INT8U* ptimtcb)
{
  ByteTypeDef tmp_tcb[3];
  
  if(FLAG_UF == (I2C_ReadByte(READ_MSG,REGADDR_FLAG)&FLAG_UF)) // 有时间更新
  {
    tmp_tcb[0].byte = I2C_ReadByte(READ_MSG,REGADDR_HOUR);
    tmp_tcb[1].byte = I2C_ReadByte(READ_MSG,REGADDR_MIN);
    tmp_tcb[2].byte = I2C_ReadByte(READ_MSG,REGADDR_SEC);
    *(ptimtcb+0) = tmp_tcb[0].bit4.bith + 0x30;
    *(ptimtcb+1) = tmp_tcb[0].bit4.bitl + 0x30;
    *(ptimtcb+2) = tmp_tcb[1].bit4.bith + 0x30;
    *(ptimtcb+3) = tmp_tcb[1].bit4.bitl + 0x30;
    *(ptimtcb+4) = tmp_tcb[2].bit4.bith + 0x30;
    *(ptimtcb+5) = tmp_tcb[2].bit4.bitl + 0x30;
    
    return 1;
  }
  
  return 0;
}

INT8U ReadDateReg(INT8U* pdatetcb)
{
  ByteTypeDef date_tcb[3];
  
  if(FLAG_UF == (I2C_ReadByte(READ_MSG,REGADDR_FLAG)&FLAG_UF)) // 有时间更新
  {
    date_tcb[0].byte = I2C_ReadByte(READ_MSG,REGADDR_YEAR);
    date_tcb[1].byte = I2C_ReadByte(READ_MSG,REGADDR_MONTH)&0x1f;
    date_tcb[2].byte = I2C_ReadByte(READ_MSG,REGADDR_DAY)&0x3f;
    *(pdatetcb+0) = date_tcb[0].bit4.bith + 0x30;
    *(pdatetcb+1) = date_tcb[0].bit4.bitl + 0x30;
    *(pdatetcb+2) = date_tcb[1].bit4.bith + 0x30;
    *(pdatetcb+3) = date_tcb[1].bit4.bitl + 0x30;
    *(pdatetcb+4) = date_tcb[2].bit4.bith + 0x30;
    *(pdatetcb+5) = date_tcb[2].bit4.bitl + 0x30;
    
    return 1;
  }
  return 0;
}

INT8U SetTimeReg(INT8U* pwtimtcb)
{
  ByteTypeDef tmp_value;
  
  tmp_value.bit4.bith = *(pwtimtcb+0)-0x30;
  tmp_value.bit4.bitl = *(pwtimtcb+1)-0x30;
  if(tmp_value.byte > 0x24) return 1;
  I2C_WriteByte(WRITE_MSG,REGADDR_HOUR,tmp_value.byte);
  
  tmp_value.bit4.bith = *(pwtimtcb+2)-0x30;
  tmp_value.bit4.bitl = *(pwtimtcb+3)-0x30;
  if(tmp_value.byte > 0x60) return 1;
  I2C_WriteByte(WRITE_MSG,REGADDR_MIN,tmp_value.byte);
  
  tmp_value.bit4.bith = *(pwtimtcb+4)-0x30;
  tmp_value.bit4.bitl = *(pwtimtcb+5)-0x30;
  if(tmp_value.byte > 0x60) return 1;
  I2C_WriteByte(WRITE_MSG,REGADDR_SEC,tmp_value.byte);
  
  return 0;
}

INT8U WriteDateReg(INT8U* pwdatetcb)
{
  ByteTypeDef tmp_value;
  
  tmp_value.bit4.bith = *(pwdatetcb+0)-0x30;
  tmp_value.bit4.bitl = *(pwdatetcb+1)-0x30;
  if(tmp_value.byte > 0x99)return 1;
  I2C_WriteByte(WRITE_MSG,REGADDR_YEAR,tmp_value.byte);
 
  tmp_value.bit4.bith = *(pwdatetcb+2)-0x30;
  tmp_value.bit4.bitl = *(pwdatetcb+3)-0x30;
  if(tmp_value.byte > 0x12) return 1;
  I2C_WriteByte(WRITE_MSG,REGADDR_MONTH,tmp_value.byte);
  
  tmp_value.bit4.bith = *(pwdatetcb+4)-0x30;
  tmp_value.bit4.bitl = *(pwdatetcb+5)-0x30;
  if(tmp_value.byte > 0x31) return 1;
  I2C_WriteByte(WRITE_MSG,REGADDR_DAY,tmp_value.byte);
  
  return 0;
}