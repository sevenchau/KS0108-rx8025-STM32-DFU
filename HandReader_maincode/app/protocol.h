#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "includes.h"

/*设备  控制*/
#define RESET             0      //  设备 复位
#define HEART             1      //  设备 心跳
#define EQU_STATUES       2      //  设备 状态查询
#define QUERY_EN          3      //  设备 查询开启
#define QUERY_DIS         4      //  设备 查询结束

#define NEW_RESET         5      //  复位设备(新协议)
#define RESET_FACTORY     6      //  恢复出厂设置

#define DOWNTRAINS        10     //  下传转发
#define UPTRAINS          20     //  上传转发

/*开关量控制命令*/
//#define SWITCH            30     //  开关量控制
#define OPEN_LF125_1      31     //  打开125K
#define CLOSE_LF125_1     32     //  关闭125K
#define OPEN_LF125_2      33     //  打开125K
#define CLOSE_LF125_2     34     //  关闭125K
#define OPEN_RF433_1      35     //  打开433M
#define CLOSE_RF433_1     36     //  关闭433M
#define OPEN_RF433_2      37     //  打开433M
#define CLOSE_RF433_2     38     //  关闭433M
#define CLOSE_ALL         39     //  关闭所有

#define OPEN_RESTORE      40     //  恢复默认
#define OPEN_RX_UART_1    41     //  打开串口1
#define CLOSE_RX_UART_1   42     //  关闭串口1
#define OPEN_RX_UART_2    43     //  打开串口2
#define CLOSE_RX_UART_2   44     //  关闭串口2
#define OPEN_RX_UART_3_1  45     //  打开串口3
#define CLOSE_RX_UART_3_1 46     //  关闭串口3
#define OPEN_RX_UART_3_2  47     //  打开串口3
#define CLOSE_RX_UART_3_2 48     //  关闭串口3
#define OPEN_BUZZER       49     //  开启蜂鸣器
#define CLOSE_BUZZER      50     //  关闭蜂鸣器

/*写终端参数配置命令*/
#define WRCONFIG          60     //  写配置信息
#define WR_ID             61     //  写ID
#define WR_SPWR           62     //  配置发送功率
#define WR_SSPD           63     //  配置发送速率
#define WR_SFRE           64     //  配置发送频点
#define WR_RSPD           65     //  配置接收速率
#define WR_RFRE           66     //  配置接收频点
#define WR_HTIM           67     //  配置心跳时间间隔
#define WR_LTIM           68     //  配置定位时间间隔
#define WR_BUAD           69     //  配置串口波特率
#define WRCFGOK           79     //  配置完成

/*读终端参数配置命令*/
#define RDCONFIG          80     //  读配置信息
#define RD_ID             81     //  获取本机ID
#define RD_SPWR           82     //  读取发送功率
#define RD_SSPD           83     //  读取发送速率
#define RD_SSFRE          84     //  读取发送频点
#define RD_RSPD           85     //  读取接收速率
#define RD_RSFRE          86     //  读取接收频点
#define RD_HTIM           87     //  读取心跳时间
#define RD_LTIM           88     //  读取定位时间
/*读取参数扩展指令*/
#define RD_CFGMSG         100    //  读取设备配置参数信息
#define RD_DEVMSG         101    //  读取设备基本信息

/*************************************************************/
/**************************数据解析***************************/
/*************************************************************/
/*flash 数据解析*/
uierr flash_ProDataLearning(const INT8U* data_buf , const INT8U size);

/*flash 信息存储打包*/
uierr flashInit_DataPackage(INT8U* buf , INT8U buflen);
void flash_DataPackage(const INT8U* buf , INT8U* rebuf, INT8U buflen);

#endif