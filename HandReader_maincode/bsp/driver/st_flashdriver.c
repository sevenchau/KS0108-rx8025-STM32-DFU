#include "st_flashdriver.h"
#include "app.h"
#include "string.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/  

/* Private typedef -----------------------------------------------------------*/
#define FAILED  0
#define PASSED  1

/* Private define ------------------------------------------------------------*/
/* Define the STM32F10x FLASH Page Size depending on the used STM32 device */
//#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
//  #define FLASH_PAGE_SIZE    ((uint16_t)0x800) //2k/页
//#else
//  #define FLASH_PAGE_SIZE    ((uint16_t)0x400) //1k/页
//#endif
#define FLASH_PAGE_SIZE    ((uint16_t)0x800) //2k/页
#define BANK1_WRITE_START_ADDR  ((uint32_t)(0x08000000+126*2*0x400L))//
#define BANK1_WRITE_END_ADDR    ((uint32_t)(0x08000000+127*2*0x400L)) //大小16k 

#ifdef STM32F10X_XL
// #define BANK2_WRITE_START_ADDR   ((uint32_t)0x08088000)
// #define BANK2_WRITE_END_ADDR     ((uint32_t)0x0808C000)
#endif /* STM32F10X_XL */

uint32_t u32_NbrofPage = 0;
uint32_t u32_FlashAddr = 0;  
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

static void st_FlashInit(void)
{
  uint32_t EraseCounter = 0;
  uint32_t NbrOfPage = 0;
  
  /* Porgram FLASH Bank1 ********************************************************/       
  /* Unlock the Flash Bank1 Program Erase controller */
  FLASH_Unlock();
//  FLASH_UnlockBank1();
  
  /* Define the number of page to be erased */
  NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;
  
  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
  
  /* Erase the FLASH pages */
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && 
      (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + 
                                  (FLASH_PAGE_SIZE * EraseCounter));
  }
  
  /*start addr 起始地址*/
  u32_FlashAddr = BANK1_WRITE_START_ADDR;
  
  FLASH_Lock();
//  FLASH_LockBank1();
}

uint8_t st_FlashWrite(uint32_t* data_buff , uint8_t buff_lenght)
{
  uint8_t  length = 0;
  uint32_t temp_addr = 0;
  uint8_t  MemoryProgramStatus = PASSED;
  
  /* Porgram FLASH Bank1 ********************************************************/       
  /* Unlock the Flash Bank1 Program Erase controller */
//  FLASH_UnlockBank1();
  FLASH_Unlock();
  
  while((u32_FlashAddr < BANK1_WRITE_END_ADDR) && 
        (FLASHStatus == FLASH_COMPLETE) && 
          (length < buff_lenght))
  {
    FLASHStatus = FLASH_ProgramWord(u32_FlashAddr, data_buff[length]);
    u32_FlashAddr = u32_FlashAddr + 4;
    length ++;
  }
  
  if((length != buff_lenght) || (FLASHStatus != FLASH_COMPLETE))
  {
    MemoryProgramStatus = FAILED;
  }
  
//  FLASH_LockBank1();
  FLASH_Lock();
  
  length = 0;
  
  /* Program Flash Bank1 */
  temp_addr = u32_FlashAddr - (buff_lenght-1)<<2;
  
  while((temp_addr < BANK1_WRITE_END_ADDR) && 
        (MemoryProgramStatus != FAILED) && 
          length < buff_lenght)
  {
    if((*(__IO uint32_t*) temp_addr) != data_buff[buff_lenght])
    {
      MemoryProgramStatus = FAILED;
    }
    temp_addr += 4;
  }
  
  return MemoryProgramStatus;
}

uint8_t st_FlashRead(uint32_t start_addr , uint32_t* buff_data , 
                            uint32_t buf_len)
{
  uint8_t MemoryReadStatus = PASSED;
  uint8_t temp_buf = 0;
  
  if((start_addr*4+BANK1_WRITE_START_ADDR)%4)
  {
    MemoryReadStatus = FAILED;
  }
  
  while(((BANK1_WRITE_START_ADDR + start_addr*4) < 
         BANK1_WRITE_END_ADDR)&& temp_buf < buf_len)
  {
    buff_data[temp_buf] = *(__IO uint32_t*) (BANK1_WRITE_START_ADDR+start_addr*4);
    start_addr += 1;
    temp_buf ++ ;
  }
  
  return MemoryReadStatus;
}
INT8U flash_write_buf[50];
void FlashMsgInit(void)
{
  uint8_t no_value = 0;
  
  if(st_FlashRead(0, (uint32_t*)flash_write_buf, 10))
  {
    for(uint8_t i=0;i<10;i++)
    {
      if(0xff == flash_write_buf[i])
      {
        no_value = 0;
      }
      else
      {
        no_value = 1;
        break;
      }
    }
    if(!no_value) // 无值 需要做初始化
    {
      st_FlashInit();
      flash_write_buf[0] = BACK_LIGHT0_DEFAULT;
      flash_write_buf[1] = BACK_LIGHT1_DEFAULT;
      flash_write_buf[2] = SHUTOFF0_DEFAULT;
      flash_write_buf[3] = SHUTOFF1_DEFAULT;
      flash_write_buf[4] = LOCA_ADDR0_DEFAULT;
      flash_write_buf[5] = LOCA_ADDR1_DEFAULT;
      flash_write_buf[6] = OPERATION0_DEFAULT;
      flash_write_buf[7] = OPERATION1_DEFAULT;
      flash_write_buf[8] = OPERATION2_DEFAULT;
      flash_write_buf[9] = OPERATION3_DEFAULT;
      st_FlashWrite((uint32_t*)flash_write_buf ,10);
      if(st_FlashRead(0, (uint32_t*)flash_write_buf, (uint32_t)(sizeof(flash_write_buf))))
      {
        return ;
      }
    }
  }
}

uint8_t WriteMsgToFlash(uint8_t* buffer,uint8_t buffer_size,uint8_t block_name)
{
  if(buffer_size > 10) return 0;
  
  if(st_FlashRead(0, (uint32_t*)flash_write_buf, 10))
  {
    for(uint8_t i=0;i<buffer_size;i++)
    {
      flash_write_buf[block_name+i] = buffer[i];
    }
  }
  st_FlashInit();
  st_FlashWrite((uint32_t*)flash_write_buf , 10);
  st_FlashRead(0, (uint32_t*)flash_write_buf, 10);

  return 1;
}

uint8_t ReadMsgFromFlash(uint8_t* buffer,uint8_t buffer_size,uint8_t block_name)
{
  if(buffer_size > 10) return 0;
  
  for(uint8_t i=0;i<buffer_size;i++)
  {
    buffer[i] = flash_write_buf[block_name+i];
  }
  
//  if(st_FlashRead(block_name, (uint32_t*)buffer, buffer_size))
//  {
//    return 1;
//  }
  return 1;   
}

