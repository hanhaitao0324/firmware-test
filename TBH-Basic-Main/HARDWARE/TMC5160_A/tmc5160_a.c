/*
 * Change Logs:
 * Date           Author       Notes
 * 2024-05-27     Gavin        the first version
 */
 
#include "tmc5160_a.h"


#define WRITE_ACCESS		0x80	// Write access for spi communication spi通信的写入访问
#define READ_ACCESS			0x00	// Read access for spi communication spi通信的读取访问

// TODO: this is very specific for THT, make it generic in the future
// TODO: 这对THT来说是非常具体的，将来将其通用化
#define MAX_ENCODER_DEVIATION (512)  // tolerate at most 2 fullsteps (^= ~0.01mm)

#define TMC51X0_REG_GCONF                0x00 // GCONF: General configuration register
#define TMC51X0_REG_GLOBAL_STATUS        0x01 // GSTAT: Global status flags
#define TMC51X0_REG_SHORT_CONF           0x09 // SHORT_CONF: Short detection configuration
#define TMC51X0_REG_DRV_CONF             0x0A // DRV_CONF: Motor driver config
#define TMC51X0_REG_GLOBAL_SCALER        0x0B // GLOBAL_SCALER: Global scaling of motor current
#define TMC51X0_REG_IHOLD_IRUN           0x10 // IHOLD_IRUN: Driver current control
#define TMC51X0_REG_TPOWERDOWN           0x11 // TPOWERDOWN: Delay before power down in standstill
#define TMC51X0_REG_TSTEP                0x12 // TSTEP: Actual measured time between two microsteps
#define TMC51X0_REG_TPWM_THRS            0x13 // TPWMTHRS: Lower velocity threshold for stealthChop voltage PWM mode
#define TMC51X0_REG_TCOOL_THRS           0x14 // TCOOLTHRS: Lower threshold velocity for switching on smart energy coolStep and stallGuard feature
#define TMC51X0_REG_THIGH                0x15 // THIGH: Velocity threshold for switching on smart energy coolStep and stallGuard feature
#define TMC51X0_REG_RAMP_MODE            0x20 // RAMPMODE: Positioning, hold or velocity modes
#define TMC51X0_REG_POSITION_CURRENT     0x21 // XACTUAL: Actual motor position
#define TMC51X0_REG_VELOCITY_CURRENT     0x22 // VACTUAL: Actual motor velocity
#define TMC51X0_REG_VELOCITY_INITIAL     0x23 // VSTART: Motor start velocity.Should be <= VSTOP.
#define TMC51X0_REG_ACCELERATION_INITIAL 0x24 // A_1: Initial acceleration between VSTART and V1
#define TMC51X0_REG_VELOCITY_THRESHOLD   0x25 // V_1: Threshold to start accelerating from VSTART to VMAX
#define TMC51X0_REG_ACCELERATION_MAX     0x26 // AMAX: Acceleration between V1 and VMAX
#define TMC51X0_REG_VELOCITY_MAX         0x27 // VMAX: Target velocity of motor movements
#define TMC51X0_REG_DECELERATION_MAX     0x28 // DMAX: Deceleration between VMAX and V1
#define TMC51X0_REG_DECELERATION_INITIAL 0x2A // D_1: Deceleration between V1 and VSTOP. Should never be 0.
#define TMC51X0_REG_VELOCITY_STOP        0x2B // VSTOP: Motor stop velocity
#define TMC51X0_REG_TZERO_WAIT           0x2C // TZEROWAIT: Waiting time after ramp down before next ramp up
#define TMC51X0_REG_POSITION_TARGET      0x2D // XTARGET: Target position for ramp mode
#define TMC51X0_REG_VDC_MIN              0x33 // VDCMIN: Minimum DcStep velocity
#define TMC51X0_REG_SW_MODE              0x34 // SW_MODE: Switch mode configuration
#define TMC51X0_REG_RAMP_STATUS          0x35 // RAMP_STAT: Ramp status and switch event status
#define TMC51X0_REG_POSITION_LATCHED     0x36 // XLATCH: Ramp generator position latch, latches XACTUAL on switch event
#define TMC51X0_REG_ENCODER_MODE         0x38 // ENCMODE: Encoder mode
#define TMC51X0_REG_ENCODER_POSITION     0x39 // X_ENC: Encoder position
#define TMC51X0_REG_ENCODER_CONST        0x3A // ENC_CONST: Encoder accumulation constant
#define TMC51X0_REG_ENCODER_STATUS       0x3B // ENC_STATUS: Encoder status flags
#define TMC51X0_REG_ENCODER_LATCH        0x3C // ENC_LATCH: Encoder position latch
#define TMC51X0_REG_ENCODER_DEVIATION    0x3D // ENC_DEVIATION: Encoder max position deviation
#define TMC51X0_REG_CHOPCONF             0x6C // CHOPCONF: Chopper and driver configuration
#define TMC51X0_REG_COOLCONF             0x6D // COOLCONF: coolStep smart current control register
#define TMC51X0_REG_DCCTRL               0x6E // DCCTRL: dcStep configuration register
#define TMC51X0_REG_DRV_STATUS           0x6F // DRV_STATUS: StallGuard2 value, current, and driver error flags
#define TMC51X0_REG_PWMCONF              0x70 // PWMCONF: Voltage PWM and StealthChop settings
#define TMC51X0_REG_PWM_SCALE            0x71 // PWM_SCALE: PWM automatic amplitude scaling
#define TMC51X0_REG_PWM_AUTO             0x72 // PWM_AUTO: Automatically determined offset and gradient

uint32_t MTCF_max_velocity; 						// 最大速度
uint32_t MTCF_max_acceleration; 				// 最大加速度
uint32_t MTCF_start_acceleration; 			// 启动时的加速度
uint32_t MTCF_start_max_threshold; 			// 启动时的最大阈值
uint32_t MTCF_stealthchop_until_velocity; 	// ？

tmc51x0_gconf_t* MTCF_gconf;
tmc51x0_ihold_irun_t* MTCF_ihold_irun;
tmc51x0_pwmconf_t* MTCF_pwmconf;
tmc51x0_chopconf_t* MTCF_chopconf;
tmc51x0_coolconf_t* MTCF_coolconf;

// current scaling (only for TMC5160)
// 当前缩放（仅适用于TMC5160）
uint8_t MTCF_global_scaler; 
uint8_t MTCF_t_powerdown;

// optional setting (if encoder is present)
// 可选设置（如果存在编码器）
int16_t MTCF_encoder_pulses_per_revolution; 

// optional setting (currently used only for encoder resolution calculation)
// 可选设置（当前仅用于编码器分辨率计算）
int16_t MTCF_steps_per_revolution; 


tmc51x0_state_t MTDR_state; 						// 状态
//tmc51x0_config_t* config; 						// 配置指针

float MTDR_calibration_factor; 					// 校准系数
int32_t MTDR_position_actual; 					// 实际位置
int32_t MTDR_position_target; 					// 目标位置

// note: this can be lower than the hard limit in the config, it's the current actual max V
// 注意：这可能低于配置中的硬限制，它是当前实际的最大值
uint32_t MTDR_velocity_max; 

bool MTDR_standstill; 									// 静止不动
bool MTDR_position_reached; 						// 位置已获取
bool MTDR_velocity_reached; 						// 速度记录
bool MTDR_stallguard_active; 						// 失速防护激活
bool MTDR_driver_error; 								// 驱动报错
bool MTDR_circular_request_enabled; 		// 循环请求使能
bool MTDR_is_first_call; 								// 第一次呼叫

// we do not know where we stopped; in case it was in the other direction, it could result in a jerk when first moving
// 我们不知道停在哪里；如果它在另一个方向，第一次移动时可能会导致颠簸
bool MTDR_has_moved_before; 

bool MTDR_previous_direction_ccw; 			// 前一个状态的方向

static void tmc51x0_register_write(uint8_t address, uint32_t data);
static void tmc51x0_set_velocity_max_raw(uint32_t velocity);

void tmc51x0_init(void) 
{
	// make sure motor is completely turned off. 确保电机完全关闭。
	tmc51x0_register_write(TMC51X0_REG_VELOCITY_MAX, 0);
	//tmc51x0_set_chopper_config(driver, &(tmc51x0_chopconf_t) { .off_time_and_driver_enable = 0 });
	tmc51x0_register_write(TMC51X0_REG_CHOPCONF, 0);
	
	// in case the motor was running, give it some time to stop. 万一马达在运转，给它一点时间停下来。
	/*systime_t start = chVTGetSystemTimeX();
	tmc51x0_await_standstill(driver);
	systime_t end = chVTGetSystemTimeX();
	if (TIME_I2MS(chTimeDiffX(start, end)) > 1) 
		trace("needed %d ms to stop motor\n", TIME_I2MS(chTimeDiffX(start, end)));*/
	
	// wherever we are, this shall be position 0.
	// 无论我们在哪里，这都将是位置0。
	tmc51x0_register_write(TMC51X0_REG_POSITION_CURRENT, 0);
	MTDR_position_actual = 0;
	tmc51x0_register_write(TMC51X0_REG_POSITION_TARGET, 0);
	MTDR_position_target = 0;
	
	// deactivate while configuring
	// 配置时停用
	tmc51x0_register_write(TMC51X0_REG_ENCODER_DEVIATION, 0); 
	tmc51x0_register_write(TMC51X0_REG_ENCODER_POSITION, 0);
	
	tmc51x0_register_write(TMC51X0_REG_ENCODER_STATUS, 2); // clear bits
	//tmc51x0_calibrate(driver);
	
	MTDR_state = TMC51X0_STATE_OFF;
}


static void tmc51x0_register_write(uint8_t address, uint32_t data) 
{
	sendMotDat(address, data);
}

void tmc51x0_stop(void) 
{
	tmc51x0_set_velocity_max(0);
}

void tmc51x0_set_velocity_max(uint32_t velocity) 
{
	float driver_factor = 1.3981f;
	
	float max_velocity = (float)MTCF_max_velocity * MTDR_calibration_factor * driver_factor;
	float velocity_f = (float)velocity * MTDR_calibration_factor * driver_factor;
	
	if (velocity_f > max_velocity) 
	{
		velocity_f = max_velocity;
	}
	
	tmc51x0_set_velocity_max_raw(velocity_f);
	MTDR_velocity_max = velocity;
}

static void tmc51x0_set_velocity_max_raw(uint32_t velocity) 
{
	if (velocity > 0x7ffe00) 
	{
		// 2^23-512 is the maximum value for VMAX
		//log("warning: VMAX %u too high, limiting to 0x7ffe00\n", velocity);
		velocity = 0x7ffe00;
	}
	
	//trace("setting velocity max to %u at pos %d\n", velocity, driver->position_actual);
	tmc51x0_register_write(TMC51X0_REG_VELOCITY_MAX, velocity);
}



