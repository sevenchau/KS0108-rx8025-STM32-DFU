/*
-  文件名称: protocol.c
-  文件描述: 
-  版本日期: v1.1 2012.04
-  作者署名: __Rangers
*/
#include "protocol.h"
#include "string.h"
#include "serve.h"

#define ERROR_PROTOCOL_BASE ERROR_BRGIN(1)       //协议部分基址

/*
*功能名称: flash_ProDataLearning 功能:flash 通信协议解析
*协议内容: |head|pow|spd|fre|idH|idL|NA.|NA.|NA.|NA.|NA.|NA.|NA.|NA.|CRC|0x7E|
*入口参数: data_buf-入口数据
*          redata_len-入口数据长度
*出口参数: FALSE 或 TRUE
*/
uierr flash_ProDataLearning(const INT8U* data_buf , const INT8U size)
{
  return TRUE;
}

/*
- 功能名称: flashInit_DataPackage flashc初始化存储协议
- 入口参数: buf 存储数据  buflen 存储长度
- 出口参数: FALSE TRUE
*/
uierr flashInit_DataPackage(INT8U* buf , INT8U buflen)
{
  return TRUE;
}

/*
- 功能名称: flash_DataPackage flash存储协议
- 入口参数: buf 存储数据  buflen 存储长度
- 出口参数: FALSE TRUE
*/
void flash_DataPackage(const INT8U* buf , INT8U* rebuf, INT8U buflen)
{
}
