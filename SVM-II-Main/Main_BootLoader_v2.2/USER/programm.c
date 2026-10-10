#include "crc32.h"
#include "programm.h"
#include "led.h"
//#include "stm32f10x.h"

iapfun jump2app; 

//------------------------------------------------------------------------------
// PROGRAMM_Unlock - unlock flash memory for write operations
//------------------------------------------------------------------------------
/*
void PROGRAMM_Unlock(void)
{
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
}
*/

//------------------------------------------------------------------------------
// PROGRAMM_Lock - lock flash memory for write operations
//------------------------------------------------------------------------------
/*
void PROGRAMM_Lock(void)
{
  FLASH->CR |= FLASH_CR_LOCK;
}
*/

//------------------------------------------------------------------------------
// PROGRAMM_EraseFlash - erase flash memory
//------------------------------------------------------------------------------
/*
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
*/
void PROGRAMM_EraseFlash(void)
{
	FLASH_EraseInitTypeDef FlashEraseInit;
	u32 SectorError=0;
	int i;
  
  HAL_FLASH_Unlock(); 									// 解锁	
  
	FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS; 				// 擦除类型，扇区擦除 
	FlashEraseInit.NbSectors=1; 															// 一次只擦除一个扇区
	FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3; 				// 电压范围，VCC=2.7~3.6V之间!!
	
	for(i=FLASH_SECTOR_3;i<=FLASH_SECTOR_8;i++)
	{
		FlashEraseInit.Sector=i; 																// 要擦除的扇区
		if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError) != HAL_OK)
		{
			break; 														// 发生错误了	
		}
		
		FLASH_WaitForLastOperation(FLASH_WAITETIME); 						// 等待上次操作完成
	}
	
  HAL_FLASH_Lock(); 										// 上锁
}

//------------------------------------------------------------------------------
// PROGRAMM_WriteFlash - write something to flash
//------------------------------------------------------------------------------
/*
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
*/
int PROGRAMM_WriteFlash(void* Src, void* Dst, int Len)
{
  int Ret = CMD_SUCCESS;
  uint32_t* SrcW = (uint32_t*)Src;
  volatile uint32_t* DstW = (uint32_t*)Dst;
  //uint32_t ADDR;
  
  //ADDR = DstW;

  HAL_FLASH_Unlock(); 									// 解锁	
  
  // programm the flash
  while(Len) 														// 写数据
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint32_t)DstW,*SrcW)!=HAL_OK) 	// 写入数据
		{ 
			Ret = CMD_ERROR;
      break;	//写入异常
		}
		
		DstW++;
		SrcW++;
		Len = Len - sizeof(uint32_t);
	}
	
  HAL_FLASH_Lock(); 										// 上锁
  
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
	
	//__disable_irq() ; //关闭总中断
	LED1 = 1;
  if (((*(__IO uint32_t*)USER_FLASH_START) & 0x2FFE0000 ) == 0x20000000)
  {
    JumpAddress = *(__IO uint32_t*) (USER_FLASH_START + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    __set_MSP(*(__IO uint32_t*) USER_FLASH_START);
    Jump_To_Application();
  }
  
  while(1);     
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000) // 检查栈顶地址是否合法.
	{
		jump2app=(iapfun)*(vu32*)(appxaddr+4); 				// 用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr); 										// 初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app(); 																	// 跳转到APP.
	}
}		 
