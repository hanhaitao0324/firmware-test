#include "crc32.h"
#include "programm.h"
#include "stm32f10x.h"

//------------------------------------------------------------------------------
// PROGRAMM_Unlock - unlock flash memory for write operations
//------------------------------------------------------------------------------
void PROGRAMM_Unlock(void)
{
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
}

//------------------------------------------------------------------------------
// PROGRAMM_Lock - lock flash memory for write operations
//------------------------------------------------------------------------------
void PROGRAMM_Lock(void)
{
  FLASH->CR |= FLASH_CR_LOCK;
}

//------------------------------------------------------------------------------
// PROGRAMM_EraseFlash - erase flash memory
//------------------------------------------------------------------------------
void PROGRAMM_EraseFlash(void)
{
  int i;
  
  PROGRAMM_Unlock();
  
  for (i=CHECKSUM_START; i<USER_FLASH_END; i+=FLASH_PAGE_SIZE)
  {
    //page erase 
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = (uint32_t)i; 
    //start erase 
    FLASH->CR|= FLASH_CR_STRT;
    //wait for end of erase
    while ((FLASH->SR & FLASH_SR_BSY) != 0);
  }

  FLASH->CR &= ~FLASH_CR_PER;

  PROGRAMM_Lock();
}

//------------------------------------------------------------------------------
// PROGRAMM_WriteFlash - write something to flash
//------------------------------------------------------------------------------
int PROGRAMM_WriteFlash(void* Src, void* Dst, int Len)
{
  int Ret = CMD_SUCCESS;
  uint16_t* SrcW = (uint16_t*)Src;
  volatile uint16_t* DstW = (uint16_t*)Dst;

  PROGRAMM_Unlock();

  // programm the flash
  FLASH->CR |= FLASH_CR_PG;
  while (Len)
  {
    *DstW = *SrcW;
    while ((FLASH->SR & FLASH_SR_BSY) != 0);
    if (*DstW != *SrcW)
    {
      Ret = CMD_ERROR;
      break;
    }
    DstW++;
    SrcW++;
    Len = Len - sizeof(uint16_t);
  }

  FLASH->CR &= ~FLASH_CR_PG;
  
  PROGRAMM_Lock();

  return Ret;
}

//------------------------------------------------------------------------------
// PROGRAMM_CalcChecksum - calculate checksum of the user flash
//------------------------------------------------------------------------------
int PROGRAMM_CalcChecksum(void)
{
  int i;
  int crc32;

  //if (!mcu_type)
  //  i = USER_FLASH_END_1 - USER_FLASH_START + 1;
  //else
  //  i = USER_FLASH_END_2 - USER_FLASH_START + 1;
  i = USER_FLASH_END - USER_FLASH_START;
  crc32 = CRC32_Calc((TByte*)USER_FLASH_START, i);

  return crc32;
}

//------------------------------------------------------------------------------
// PROGRAMM_CodeChecksum - calculate checksum of the user flash and check it
//------------------------------------------------------------------------------
int PROGRAMM_CodeChecksum(void)
{
  unsigned int * ptr;
  int crc32;

  ptr =(unsigned int *)CHECKSUM_START;  

  crc32 = PROGRAMM_CalcChecksum();

  if (crc32==*ptr)
    return 1;
  else
    return 0;
}

//------------------------------------------------------------------------------
// PROGRAMM_ExecuteCode - execute user code in flash
//------------------------------------------------------------------------------
void PROGRAMM_ExecuteCode(void)
{
	/*typedef void (*pFunction)(void);

  pFunction JumpToApplication;
  uint32_t jumpAddress;

  // Initialize the user application Stack Pointer
  __set_MSP(*(__IO uint32_t*) USER_FLASH_START);

  // Jump to the user application
  // The stack pointer lives at USER_FLASH_START
  // The reset vector is at USER_FLASH_START + 4
  jumpAddress = *(__IO uint32_t*) (USER_FLASH_START + 4);
  JumpToApplication = (pFunction) jumpAddress;
  JumpToApplication();*/

  typedef  void (*pFunction)(void);
  pFunction Jump_To_Application;
  uint32_t JumpAddress;

  if (((*(__IO uint32_t*)USER_FLASH_START) & 0x2FFE0000 ) == 0x20000000)
  {
    JumpAddress = *(__IO uint32_t*) (USER_FLASH_START + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    __set_MSP(*(__IO uint32_t*) USER_FLASH_START);
    Jump_To_Application();
  }
  while(1);     
}
