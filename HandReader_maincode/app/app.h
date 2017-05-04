#ifndef  __APP_H
#define  __APP_H

#define __DEBUG 1

#include "includes.h"

/*错误信息报告*/
typedef INT16U 			uierr;

#define TRUE                    0
#define FALSE                   1

#define MODULE_BITS             7       
#define ERROR_BITS              8       

#define ERROR_BIT               1<<(MODULE_BITS+ERROR_BITS)

#define ERROR_BRGIN(_module_id) ((_module_id)<<ERROR_BITS)

#define ERROR_T(_module_error)  (ERROR_BIT | (_module_error))

#define MODULE_ERROR(_error_t)  ((_error_t)&((1<<ERROR_BITS)-1))
#define MODULE_ID(_error_t)     (((_error_t)& ~(ERROR_BIT))>>ERROR_BITS)

typedef union 
{
  INT8U byte;
  struct
  {
    INT8U bitl:4;
    INT8U bith:4;
  }bit4;
}ByteTypeDef;

unsigned char CRC8(unsigned char * pData, unsigned char len);
void HexToBCD(INT8U* pdata,INT8U dlen,INT8U* prdata,INT8U* rdlen);

#endif