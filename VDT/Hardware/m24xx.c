/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    m24xx.c
 *      Purpose: External serial EEPROM driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2016 Erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
 
/**
 @file  m24xx.c
 @brief External serial EEPROM driver
 @date  10.04.2018
 @addtogroup Hardware
 @{
 @defgroup M24XX M24XX
 @brief External serial EEPROM driver
 @{
*/

#include "m24xx.h"
#include "timer.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

uint8_t M24XX_Address;
uint32_t M24XX_Errors = 0;

//------------------------------------------------------------------------------
/// @brief Simple delay function for non-RTOS environment
//------------------------------------------------------------------------------
void M24XX_DelayMs(uint32_t ms)
{
  volatile uint32_t nCount;
  RCC_ClocksTypeDef RCC_Clocks;

  RCC_GetClocksFreq (&RCC_Clocks);

  nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
  for (; nCount!=0; nCount--);
}

//------------------------------------------------------------------------------
/// @brief Initialize serial EEPROM module
//------------------------------------------------------------------------------
void M24XX_Init(void)
{
  I2C_InitTypeDef I2C_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  M24XX_PIN_SCL | M24XX_PIN_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(M24XX_GPIO, &GPIO_InitStructure);

	// I2C configuration
  I2C_DeInit(I2C_EE);
  M24XX_DelayMs(1);
  I2C_SoftwareResetCmd(I2C_EE, ENABLE);
  M24XX_DelayMs(1);
  I2C_SoftwareResetCmd(I2C_EE, DISABLE);
  M24XX_DelayMs(1);

  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = M24XX_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = M24XX_I2C_SPEED;
  I2C_Init(I2C_EE, &I2C_InitStructure);

  //I2C_Cmd(I2C_EE, ENABLE);
  //I2C_StretchClockCmd(I2C_EE, ENABLE);

  M24XX_Address = M24XX_ADDRESS;

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE);
}

//------------------------------------------------------------------------------
/// @brief Restore I2C from stall
//------------------------------------------------------------------------------
bool M24XX_Restore(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  //clear bus error flag
  I2C_ClearFlag(I2C_EE, I2C_FLAG_BERR);
  //disable I2C peripheral
  I2C_Cmd(I2C_EE, DISABLE);
  //configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level 
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  M24XX_PIN_SCL | M24XX_PIN_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  GPIO_SetBits(GPIOB, M24XX_PIN_SCL);
  GPIO_SetBits(GPIOB, M24XX_PIN_SDA);
  //check SCL and SDA High level
  if (GPIO_ReadInputDataBit(GPIOB, M24XX_PIN_SDA) == Bit_RESET) 
    return false;
  if (GPIO_ReadInputDataBit(GPIOB, M24XX_PIN_SCL) == Bit_RESET)
    return false;
  //configure the SDA I/O as General Purpose Output Open-Drain, Low level
  GPIO_ResetBits(GPIOB, M24XX_PIN_SDA);
  //check SDA Low level
  if (GPIO_ReadInputDataBit(GPIOB, M24XX_PIN_SDA) == Bit_SET)
    return false;
  //configure the SCL I/O as General Purpose Output Open-Drain, Low level
  GPIO_ResetBits(GPIOB, M24XX_PIN_SCL);
  //check SCL Low level
  if (GPIO_ReadInputDataBit(GPIOB, M24XX_PIN_SCL) == Bit_SET)
    return false;
  //configure the SCL I/O as General Purpose Output Open-Drain, High level
  GPIO_SetBits(GPIOB, M24XX_PIN_SCL);
  //check SCL High level
  if (GPIO_ReadInputDataBit(GPIOB, M24XX_PIN_SCL) == Bit_RESET)
    return false;  
  //configure the SDA I/O as General Purpose Output Open-Drain, High level
  GPIO_SetBits(GPIOB, M24XX_PIN_SDA);
  //check SDA High level
  if (GPIO_ReadInputDataBit(GPIOB, M24XX_PIN_SDA) == Bit_RESET)
    return false;    
  /*//configure the SCL and SDA I/Os as Alternate function Open-Drain
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  M24XX_PIN_SCL | M24XX_PIN_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  //set SWRST bit in I2Cx_CR1 register
  M24XX_DelayMs(1);
  I2C_SoftwareResetCmd(I2C_EE, ENABLE);
  //clear SWRST bit in I2Cx_CR1 register
  M24XX_DelayMs(1);
  I2C_SoftwareResetCmd(I2C_EE, DISABLE);
  //enable I2C peripheral
  M24XX_DelayMs(1);
  I2C_Cmd(I2C_EE, ENABLE);*/
  M24XX_Init();
  
  return true;
}

//------------------------------------------------------------------------------
/// @brief Wait for EEPROM I2C event
//------------------------------------------------------------------------------
bool M24XX_WaitForEvent(uint32_t I2C_Event)
{
  uint16_t ticks = 3000;

  while ((!I2C_CheckEvent(I2C_EE, I2C_Event)) && ticks)
    ticks--;
  
  if (ticks==0)
  {
    M24XX_Errors++;
    //reinit I2C
    M24XX_Restore();
    return false;
  }
  
  return true;
}

//------------------------------------------------------------------------------
/// @brief Wait for EEPROM I2C flag
//------------------------------------------------------------------------------
bool M24XX_WaitForFlag(uint32_t I2C_Flag)
{
  uint16_t ticks = 10000;

  //while ((!I2C_CheckEvent(I2C_EE, I2C_Event)) && ticks)
  while (I2C_GetFlagStatus(I2C_EE, I2C_Flag) && ticks)
    ticks--;
  
  if (ticks==0)
  {
    M24XX_Errors++;
    //reinit I2C
    M24XX_Restore();
    return false;
  }
  
  return true;
}

//------------------------------------------------------------------------------
/// @brief Wait for EEPROM ready status
//------------------------------------------------------------------------------
void M24XX_WaitEepromStandbyState(void) 
{
  /*RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);   

  do 
  {
    I2C_ClearFlag(I2C_EE, I2C_FLAG_AF);
  
    I2C_GenerateSTART(I2C_EE, ENABLE);
    while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT)); 

    I2C_Send7bitAddress(I2C_EE, M24XX_Address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C_EE, 0x00070080) && !I2C_CheckEvent(I2C_EE, 0x00030400));  

  } while (I2C_GetFlagStatus(I2C_EE, I2C_FLAG_AF)==SET);
  
  I2C_GenerateSTOP(I2C_EE, ENABLE);  

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE);*/
  
  /*do 
  {
    I2C_GenerateSTART(I2C_EE, ENABLE);
    I2C_ReadRegister(I2C_EE, I2C_Register_SR1);
    I2C_Send7bitAddress(I2C_EE, M24XX_Address, I2C_Direction_Transmitter);
  } while (!(I2C_ReadRegister(I2C_EE, I2C_Register_SR1) & 0x0002));
  
  I2C_ClearFlag(I2C_EE, I2C_FLAG_AF);
  
  I2C_GenerateSTOP(I2C_EE, ENABLE);*/
  
  M24XX_DelayMs(5);
}

//------------------------------------------------------------------------------
/// @brief      Write one byte to serial EEPROM
/// @param [in] writeAddr Address to write data
/// @param [in] value Value to write, byte
/// @return     Error code
//------------------------------------------------------------------------------
uint8_t M24XX_ByteWrite(uint16_t writeAddr, uint8_t value)
{
  if (writeAddr>=M24XX_FLASH_SIZE)
    return M24XX_ERR_PARAM;


  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);   

  I2C_GenerateSTART(I2C_EE, ENABLE);
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return M24XX_ERR_BUS;

  I2C_Send7bitAddress(I2C_EE, M24XX_Address, I2C_Direction_Transmitter);
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return M24XX_ERR_BUS;

  I2C_SendData(I2C_EE, (uint8_t)(writeAddr >> 8));
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return M24XX_ERR_BUS;

  I2C_SendData(I2C_EE, (uint8_t)(writeAddr & 0x00FF));
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return M24XX_ERR_BUS;

  I2C_SendData(I2C_EE, value); 
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return M24XX_ERR_BUS;
  
  I2C_GenerateSTOP(I2C_EE, ENABLE);

  M24XX_WaitEepromStandbyState();

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE);
  
  return M24XX_ERR_NONE;
}

//------------------------------------------------------------------------------
/// @brief      Write page to serial EEPROM
/// @param [in] writeAddr Address to write data
/// @param [in] numByteToWrite Number of bytes to write
/// @param [in] pBuffer Data buffer to write
/// @return     Error code
//------------------------------------------------------------------------------
uint8_t M24XX_PageWrite(uint16_t writeAddr, uint16_t numByteToWrite, uint8_t *pBuffer) 
{ 
  if (!numByteToWrite)
    return M24XX_ERR_NONE;

  if (writeAddr>=M24XX_FLASH_SIZE)
    return M24XX_ERR_PARAM;

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);   

  /*counter = 10000;
  while (I2C_GetFlagStatus(I2C_EE, I2C_FLAG_BUSY) && counter)
    counter--;
  if (counter==0)
  {
    M24XX_Restore();
    return M24XX_ERR_BUS;
  }*/
  if (!M24XX_WaitForFlag(I2C_FLAG_BUSY))
    return M24XX_ERR_BUS;
  
  I2C_GenerateSTART(I2C_EE, ENABLE);
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT)); 
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return M24XX_ERR_BUS;
  
  I2C_Send7bitAddress(I2C_EE, M24XX_Address, I2C_Direction_Transmitter);
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return M24XX_ERR_BUS;

  I2C_SendData(I2C_EE, (uint8_t)(writeAddr >> 8));
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return M24XX_ERR_BUS;

  I2C_SendData(I2C_EE, (uint8_t)(writeAddr & 0x00FF));
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return M24XX_ERR_BUS;

  while (numByteToWrite--)
  {
    I2C_SendData(I2C_EE, *pBuffer); 
    pBuffer++; 
    //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
      return M24XX_ERR_BUS;
  }

  I2C_GenerateSTOP(I2C_EE, ENABLE);
  
  M24XX_WaitEepromStandbyState();

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE); 
  
  return M24XX_ERR_NONE;
}

//------------------------------------------------------------------------------
/// @brief      Write buffer to serial EEPROM
/// @param [in] writeAddr Address to write data
/// @param [in] numByteToWrite Number of bytes to write
/// @param [in] pBuffer Data buffer to write
/// @return     Error code
//------------------------------------------------------------------------------
uint8_t M24XX_BufferWrite(uint16_t writeAddr, uint16_t numByteToWrite, uint8_t *pBuffer)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, count = 0;
  uint16_t Addr = 0;
  uint8_t ErrCode;

  if (!numByteToWrite)
    return M24XX_ERR_NONE;

  if (writeAddr>=M24XX_FLASH_SIZE)
    return M24XX_ERR_PARAM;

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);   

  Addr = writeAddr % M24XX_FLASH_PAGESIZE;
  count = M24XX_FLASH_PAGESIZE - Addr;
  NumOfPage =  numByteToWrite / M24XX_FLASH_PAGESIZE;
  NumOfSingle = numByteToWrite % M24XX_FLASH_PAGESIZE;
 
  if (Addr == 0) 
  {
   	if (NumOfPage == 0)
    {
      ErrCode = M24XX_PageWrite(writeAddr, NumOfSingle, pBuffer);
      if (ErrCode!=M24XX_ERR_NONE)
        return ErrCode;
    } else
    {
      while (NumOfPage--) 
      {
       ErrCode = M24XX_PageWrite(writeAddr, M24XX_FLASH_PAGESIZE, pBuffer);
        if (ErrCode!=M24XX_ERR_NONE)
          return ErrCode;
        writeAddr +=  M24XX_FLASH_PAGESIZE;
        pBuffer += M24XX_FLASH_PAGESIZE;
      }

      if(NumOfSingle!=0)
      {
        ErrCode = M24XX_PageWrite(writeAddr, NumOfSingle, pBuffer);
        if (ErrCode!=M24XX_ERR_NONE)
          return ErrCode;        
      }
    }
  }	else 
  {
    if (NumOfPage==0)
    {
      if (numByteToWrite > count) 
      {
        ErrCode = M24XX_PageWrite(writeAddr, count, pBuffer);
        if (ErrCode!=M24XX_ERR_NONE)
          return ErrCode;      
        ErrCode = M24XX_PageWrite((writeAddr + count), (numByteToWrite - count), (uint8_t*)(pBuffer + count));
        if (ErrCode!=M24XX_ERR_NONE)
          return ErrCode;        
      }	else 
      {
        ErrCode = M24XX_PageWrite(writeAddr, NumOfSingle, pBuffer);
        if (ErrCode!=M24XX_ERR_NONE)
          return ErrCode;        
      }     
    } else 
    {
      numByteToWrite -= count;
      NumOfPage =  numByteToWrite / M24XX_FLASH_PAGESIZE;
      NumOfSingle = numByteToWrite % M24XX_FLASH_PAGESIZE;
      
      if (count != 0) 
      {  
        ErrCode = M24XX_PageWrite(writeAddr, count, pBuffer);
        if (ErrCode!=M24XX_ERR_NONE)
          return ErrCode;        
        writeAddr += count;
        pBuffer += count;
      } 
  
      while (NumOfPage--) 
      {
        ErrCode = M24XX_PageWrite(writeAddr, M24XX_FLASH_PAGESIZE, pBuffer);
        if (ErrCode!=M24XX_ERR_NONE)
          return ErrCode;        
        writeAddr +=  M24XX_FLASH_PAGESIZE;
        pBuffer += M24XX_FLASH_PAGESIZE;  
      }
      if (NumOfSingle != 0)
      {
        ErrCode = M24XX_PageWrite(writeAddr, NumOfSingle, pBuffer); 
        if (ErrCode!=M24XX_ERR_NONE)
          return ErrCode;        
      }
    }
  }  
  
  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE);   
  
  return M24XX_ERR_NONE;
}


//------------------------------------------------------------------------------
/// @brief       Read buffer from serial EEPROM
/// @param [in]  readAddr Address to read data
/// @param [in]  numByteToRead Number of bytes to read
/// @param [out] pBuffer Data buffer to read
/// @return      Error code
//------------------------------------------------------------------------------
uint8_t M24XX_ReadBuffer(uint16_t readAddr, uint16_t numByteToRead, uint8_t *pBuffer)
{
  if (!numByteToRead)
    return M24XX_ERR_NONE;

  if (readAddr>=M24XX_FLASH_SIZE)
    return M24XX_ERR_PARAM;

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, ENABLE);   

  I2C_GenerateSTART(I2C_EE, ENABLE);
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return M24XX_ERR_BUS;
   
  I2C_Send7bitAddress(I2C_EE, M24XX_Address, I2C_Direction_Transmitter);
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return M24XX_ERR_BUS;
  
  I2C_Cmd(I2C_EE, ENABLE);

  I2C_SendData(I2C_EE, (uint8_t)(readAddr >> 8));    
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return M24XX_ERR_BUS;
  
  I2C_SendData(I2C_EE, (uint8_t)(readAddr & 0x00FF));    
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return M24XX_ERR_BUS;

  I2C_GenerateSTART(I2C_EE, ENABLE);
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return M24XX_ERR_BUS;
  
  I2C_Send7bitAddress(I2C_EE, M24XX_Address, I2C_Direction_Receiver);
  //while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  if (!M24XX_WaitForEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    return M24XX_ERR_BUS;

  while (numByteToRead)
  {
    if (numByteToRead==1)
    {
    	I2C_AcknowledgeConfig(I2C_EE, DISABLE);
      I2C_GenerateSTOP(I2C_EE, ENABLE);
    }

    if (I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_RECEIVED)) 
    {
     *pBuffer = I2C_ReceiveData(I2C_EE);
      pBuffer++; 
      numByteToRead--;        
    }   
  }

  I2C_AcknowledgeConfig(I2C_EE, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APBPeriph_I2C_EE, DISABLE); 
  
  return M24XX_ERR_NONE;
}

/// @}
/// @}
