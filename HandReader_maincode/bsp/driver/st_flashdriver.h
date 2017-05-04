#ifndef __ST_FLASH_H
#define __ST_FLASH_H

#include "stm32f10x.h"

#define FLASE_BASE_ADDR      0

#define BACK_LIGHT_TIME      (FLASE_BASE_ADDR+0)
  #define BACK_LIGHT0_DEFAULT   0x10
  #define BACK_LIGHT1_DEFAULT   0x00

#define SHUTOFF_TIME         (FLASE_BASE_ADDR+2) 
  #define SHUTOFF0_DEFAULT      0x10
  #define SHUTOFF1_DEFAULT      0x00

#define LOCA_ADDR            (FLASE_BASE_ADDR+4)
  #define LOCA_ADDR0_DEFAULT    0x00
  #define LOCA_ADDR1_DEFAULT    0x00
#define OPERATION_CODE       (FLASE_BASE_ADDR+6)
  #define OPERATION0_DEFAULT    0x00
  #define OPERATION1_DEFAULT    0x00
  #define OPERATION2_DEFAULT    0x00
  #define OPERATION3_DEFAULT    0x00

void FlashMsgInit(void);
uint8_t WriteMsgToFlash(uint8_t* buffer,uint8_t buffer_size,uint8_t block_name);
uint8_t ReadMsgFromFlash(uint8_t* buffer,uint8_t buffer_size,uint8_t block_name);

#endif