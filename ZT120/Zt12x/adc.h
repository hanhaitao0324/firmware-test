#ifndef _ADC_H
  #define _ADC_H

#include "defines.h"
                    
#define ADC_BUFFER_SIZE       (4)
#define ADC_BYPASS            (4)
#define ADC_MAX_TRY           (5)

#define ADC_CONF_DELAY        (100)

#define ADC_EMPTY_BYTE        (0xFF)      // empty byte for reading SPI（读取SPI的空字节）

#define ADC_ID                (0x0A)

#define ADC_IGNORE_VALUE      (0x0100)

enum ADC_CHANNEL
  {
    ADC_CHNL_EXTERNAL,
    ADC_NUM_CHANNELS
  };

enum CAL_REGISTER_TYPE
  { 
    CAL_REG_OFFSET,
    CAL_REG_GAIN
  };

// bits of mode register of ADC（模数转换的位寄存器）
#define MD2         (0x8000)
#define MD1         (0x4000)
#define MD0         (0x2000)
#define CLK1        (0x0080)
#define CLK0        (0x0040)
#define FS3         (0x0008)
#define FS2         (0x0004)
#define FS1         (0x0002)
#define FS0         (0x0001)

#define ADC_MODE_CONTINUOUS        (0x0000)
#define ADC_MODE_SINGLE            (MD0)
#define ADC_MODE_IDLE              (MD1)
#define ADC_MODE_POWERDOWN         (MD1|MD0)
#define ADC_MODE_INTZERO           (MD2)
#define ADC_MODE_INTFULL           (MD2|MD0)
#define ADC_MODE_SYSZERO           (MD2|MD1)
#define ADC_MODE_SYSFULL           (MD2|MD1|MD0)

#define ADC_CLOCK_INTERNAL         (0x0000)
#define ADC_CLOCK_INTERNAL_OUT     (CLK0)
#define ADC_CLOCK_EXTERNAL64       (CLK1)
#define ADC_CLOCK_EXTERNAL         (CLK1|CLK0)

// bits of configuration register of ADC（模数转换的配置寄存器）
#define VBIAS1      (0x8000)
#define VBIAS0      (0x4000)
#define BO          (0x2000)
#define U_B         (0x1000)
#define BOOST       (0x0800)
#define G2          (0x0400)
#define G1          (0x0200)
#define G0          (0x0100)
#define REFSEL      (0x0080)
#define BUF         (0x0010)
#define CH2         (0x0004)
#define CH1         (0x0002)
#define CH0         (0x0001)

#define ADC_VBIAS_DISABLED    (0x0000)
#define ADC_VBIAS_CH1         (VBIAS0)
#define ADC_VBIAS_CH2         (VBIAS1)

#define ADC_GAIN_1            (0x0000)
#define ADC_GAIN_2            (G0)
#define ADC_GAIN_4            (G1)
#define ADC_GAIN_8            (G1|G0)
#define ADC_GAIN_16           (G2)
#define ADC_GAIN_32           (G2|G0)
#define ADC_GAIN_64           (G2|G1)
#define ADC_GAIN_128          (G2|G1|G0)

#define ADC_CHANNEL1          (0x0000)
#define ADC_CHANNEL2          (CH0)
#define ADC_CHANNEL3          (CH1)

// bits of input/output mode register of ADC（模数转换的输入/输出寄存器）
#define IEXCDIR1    (0x08)
#define IEXCDIR0    (0x04)
#define IEXCEN1     (0x02)
#define IEXCEN0     (0x01)

#define ADC_CURDIR_NORMAL       (0x00)
#define ADC_CURDIR_REVERSE      (IEXCDIR0)
#define ADC_CURDIR_DOUBLE1      (IEXCDIR1)
#define ADC_CURDIR_DOUBLE2      (IEXCDIR1|IEXCDIR0)

#define ADC_CURRENT_DISABLE     (0x00)
#define ADC_CURRENT_10uA        (IEXCEN0)
#define ADC_CURRENT_210uA       (IEXCEN1)
#define ADC_CURRENT_1mA         (IEXCEN1|IEXCEN0)

// bits of status register of ADC（模数转换的状态寄存器）
#define RDY         (0x80)
#define ERR         (0x40)
#define STS_CH2     (0x04)
#define STS_CH1     (0x02)
#define STS_CH0     (0x01)

// bits of communications register of ADC（模数转换的通信寄存器）
#define WEN         (0x80)    // 写入使能位
#define R_W         (0x40)    // 读写控制位
#define RS2         (0x20)    // 寄存器地址位2
#define RS1         (0x10)    // 寄存器地址位1
#define RS0         (0x08)    // 寄存器地址位0
#define CREAD       (0x04)    // 连续读取数据寄存器控制位

//registers of AD7792（AD7792的寄存器）
#define REG_COMMUNICATION     0x00                // 写操作时为通信寄存器
#define REG_STATUS            0x00                // 都操作时为状态寄存器
#define REG_MODE              (RS0)               // 模式寄存器
#define	REG_CONFIG            (RS1)               // 配置寄存器
#define REG_DATA              (RS1|RS0)           // 数据寄存器
#define REG_ID                (RS2)               // ID寄存器
#define REG_IO                (RS2|RS0)           // IO寄存器
#define REG_OFFSET            (RS2|RS1)           // 失调寄存器
#define REG_FULLSCALE         (RS2|RS1|RS0)       // 满量程寄存器

#define ADC_READ_DATA_REG     (R_W|REG_DATA)      // read data register（读数据寄存器）
#define ADC_WRITE_CONF_REG    (REG_CONFIG)        // write ADC configuration register（写ADC配置寄存器）
#define ADC_WRITE_MODE_REG    (REG_MODE)          // write ADC mode（写ADC模式）
#define ADC_WRITE_IO_REG      (REG_IO)            // write ADC input/output mode（写ADC输入/输出模式）
#define ADC_READ_STATUS_REG   (R_W|REG_STATUS)    // read ADC status（读ADC状态）
#define ADC_READ_ID_REG       (R_W|REG_ID)        // read ADC identificator（读ADC识别器）

// current 1mA to both outputs, normal direction （两路输出电流1mA，默认方向）
#define REG_IO_DATA                (ADC_CURDIR_NORMAL|ADC_CURRENT_1mA) 

// continuous conversion mode （连续转换模式）
#define REG_MODE_DATA              (ADC_MODE_CONTINUOUS|ADC_CLOCK_INTERNAL|FS3|FS1) 

// internal zero calibration （内部零校准）
#define REG_MODE_INTZERO_DATA      (ADC_MODE_INTZERO|ADC_CLOCK_INTERNAL|FS3|FS1) 

// configuration, gain 1x, channel 1, buffered, ext. reference, unipolar （配置：获得1X，通道1，缓冲，外部参考，单极性）
#define REG_CONF_DATA              (ADC_VBIAS_DISABLED|U_B|ADC_GAIN_16|BUF|ADC_CHANNEL1) 

enum TTemperatureCannel
{
  TEMPERATURE_START,
  TEMPERATURE_OFFSET_EXT,
  TEMPERATURE_EXTERNAL
};

//initialization code（初始化代码）
extern void ADC_Init(void);

// ADC_IntHandler - ADC timer handler（ADC定时处理程序）
extern TBool ADC_IntHandler(void);

// ADC_GetValue - get measured value（获得测量值）
extern TWord ADC_GetValue(void);

#endif

