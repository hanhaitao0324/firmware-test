#ifndef _M24XX_H
#define _M24XX_H

#include "platform.h"
#include "defines.h"

enum {
  M24XX_ERR_NONE,
  M24XX_ERR_PARAM,
  M24XX_ERR_BUS
};

#define M24XX_FLAG_TIMEOUT      (100000)
#define M24XX_I2C_SPEED         (100000)

#define M24XX_FLASH_SIZE        (32768)
#define M24XX_FLASH_PAGESIZE    (64)

#define M24XX_ADDRESS           (0xA0)

#define M24XX_GPIO              GPIOB
#define M24XX_PIN_SCL           GPIO_Pin_6
#define M24XX_PIN_SDA           GPIO_Pin_7

#define I2C_EE                  I2C1
#define RCC_APBPeriph_I2C_EE    RCC_APB1Periph_I2C1

extern void M24XX_Init(void);
extern uint8_t M24XX_ReadBuffer(uint16_t ReadAddr, uint16_t NumByteToRead, uint8_t *Buff);
extern uint8_t M24XX_ByteWrite(uint16_t WriteAddr, uint8_t Buff);
extern uint8_t M24XX_PageWrite(uint16_t WriteAddr, uint16_t NumByteToWrite, uint8_t *pBuffer);
extern uint8_t M24XX_BufferWrite(uint16_t WriteAddr, uint16_t NumByteToWrite, uint8_t *pBuffer);

#endif
