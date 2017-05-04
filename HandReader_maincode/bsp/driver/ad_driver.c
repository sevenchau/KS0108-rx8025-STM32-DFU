#include "ad_driver.h"
#include "includes.h"
#include "string.h"
/*
* PA4 ------ AD12 IN4     RTC电池电量   3.6V 1:1电阻分压
* PA5 ------ AD12 IN5     供电电池电量  3V   1:1电阻分压
*/

//ADC_DMACmd(ADC1, ENABLE);
#define  Sample_Num 100
#define  Channel_Num 2
uint16_t ADC_ConvertedValue[Sample_Num][Channel_Num] = {0};

void ADC_DMA_Config(void)
{
 DMA_InitTypeDef DMA_InitStructure;
              
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
 DMA_DeInit(DMA1_Channel1);
 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//定义 DMA通道外设基地址
 DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;//定义DMA通道存储器地址
 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//指定外设为源地址
 DMA_InitStructure.DMA_BufferSize = Sample_Num*Channel_Num;//定义DMA缓冲区大小
 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址递增
 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//定义外设数据宽度16位
 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//定义存储器数据宽度16位
 DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA通道操作模式位环形缓冲模式
 DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA通道优先级高
 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//禁止DMA通道存储器到存储器传输
 DMA_Init(DMA1_Channel1, &DMA_InitStructure);
 DMA_Cmd(DMA1_Channel1,ENABLE);
 
// DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);//启动传输完成中断     
// BSP_IntVectSet(BSP_INT_ID_DMA1_CH1, BSP_IntHandlerDMA1_CH1); 
// BSP_IntPrioSet(BSP_INT_ID_DMA1_CH1,1);   
// BSP_IntEn(BSP_INT_ID_DMA1_CH1);
}

void ADC1_Config(void)
{
 GPIO_InitTypeDef GPIO_InitStructure;
 ADC_InitTypeDef ADC_InitStructure;
  
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
 ADC_DMA_Config();
  
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//RCC_APB2Periph_AFIO
// ADC_DeInit(ADC1);
 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1和ADC2工作在独立模式
 ADC_InitStructure.ADC_ScanConvMode = ENABLE;//多通道
 ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件启动转换
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换结果右对齐
 ADC_InitStructure.ADC_NbrOfChannel = Channel_Num;//通道数目
 ADC_Init(ADC1, &ADC_InitStructure); 
// ADC_TempSensorVrefintCmd(ENABLE);//使能片内温度传感器
  
 //STM32 的ADC 时钟频率范围：0.6MHz - 14MHz； 采样时间根据寄存器可设置为：1.5，.....239.5 个时钟周期；
// RCC_ADCCLKConfig(RCC_PCLK2_Div8); //ADC 最大为14M PCLK 6分频 72/6 = 12M 最小采样时间 1.5us
 ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);//通道，转换次序，转换时间
 ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_239Cycles5);
  
 ADC_DMACmd(ADC1, ENABLE);
 ADC_Cmd(ADC1, ENABLE);
 
 ADC_ResetCalibration(ADC1);
 while(ADC_GetResetCalibrationStatus(ADC1));
 ADC_StartCalibration(ADC1);//开始校准
 while(ADC_GetCalibrationStatus(ADC1));
 ADC_SoftwareStartConvCmd(ADC1, DISABLE);//使能ADC的软件转换启动功能
}

void CalculateSampleValues(float* ppower_electric,float* prtc_electric)
{
  uint32_t sum = 0;
  
  for(uint8_t i=0;i<Sample_Num;i++)
  {  
    sum += ADC_ConvertedValue[i][0];
  }
  *ppower_electric = ((float)((float)sum/(float)Sample_Num/4096.0))*3.0*2;
  
  sum = 0;
  for(uint8_t i=0;i<Sample_Num;i++)
  {  
    sum += ADC_ConvertedValue[i][1];
  }
  *prtc_electric = ((float)((float)sum/(float)Sample_Num/4096.0))*3.6*2;
}


// 将小数转成 整数部分和小数部分
void FloatChangeToInt(float value,uint16_t* integer, uint16_t* decimal)
{
  *integer = (uint16_t)value;
  *decimal = (uint16_t)((value - (*integer))*1000);
}