#ifndef TMC51X0_H
#define TMC51X0_H


/* Unfortunately, with this current setup (9 full motor rotations for a full magazine rotation, max speed without
 * stalling < 1 */
/* From the TMC51x0 datasheets:
   "StallGuard2 does not operate reliably at extreme motor velocities: Very low motor velocities (for many
   motors, less than one revolution per second) generate a low back EMF and make the measurement
   unstable and dependent on environment conditions (temperature, etc.)." */
// only acceptable at much higher speeds, but even then it sounds terrible
#define USE_DCSTEP 0
// does not result in proper current savings without mini-stalls, maybe try when aluminium components are installed
#define USE_COOLSTEP 0
#define HIGH_POWER 1

// address: 0x00
typedef struct __attribute__((__packed__)) 
{
    uint32_t i_scale_analog : 1; // 0: normal operation, 1: use voltage supplied to AIN as current reference
    uint32_t internal_rsense : 1; // 0: use external Rsense, 1: use internal Rsense
    uint32_t enable_pwm_mode : 1; // 1: StealthChop voltage PWM mode enabled
    uint32_t encoder_commutation_mode : 1; // 1: enable commutation by full step encoder (do not use, leave at 0)
    uint32_t shaft_inverse : 1; // 0: normal operation, 1: inverse motor direction
    uint32_t diag0 : 9; // only in SD_MODE=1, don't care
    uint32_t small_hysteresis : 1; // 0: hysterisis for step frequency comparison is 1/16 1: 1/32
    uint32_t stop_enable : 1; // 0: normal operation, 1: motor stops when switch ENCA_DCIN is active
    uint32_t direct_mode : 1; // 0: normal operation, 1: motor coil currenty and polarity directly programmed via serial interface
    uint32_t test_mode : 1; // 0: normal operation, 1: test mode (do not use, leave at 0)
} tmc51x0_gconf_t;

// address: 0x01
typedef struct __attribute__((__packed__)) 
{
    uint32_t reset : 1; // 1: indicated that the IC has been reset since the last read access to GSTAT
    uint32_t drv_err : 1; // 1: driver error detected
    uint32_t uv_cp : 1; // 1: undervoltage on charge pump detected
} tmc51x0_gstat_t;

typedef struct __attribute__((__packed__)) 
{
    uint32_t pwm_scale_sum : 8;
    uint32_t pwm_auto_offset : 8;
    int16_t pwm_auto_gradient;
    int16_t pwm_scale_auto;
} tmc51x0_pwm_status_t;

// address: 0x10
typedef struct __attribute__((__packed__)) 
{
    uint32_t i_hold : 5; // standstill current (0 = 1/32, 1: 2/32, ... 31 = 32/32). In combination with StealthChop mode, setting IHOLD=0 allows to choose freewheeling or coil short circuit for motor stand still
    uint32_t i_run : 5; // motor run current (0 = 1/32, 1: 2/32, ... 31 = 32/32). Hint: Choose sense resistors in a way that normal IRUN is 16 to 31 for best microstep performance
    uint32_t i_hold_delay : 4; // controls the number of clock cycles for motor power down after a motion as soon as standstill is detected (stst=1) and TPOWERDOWN has expired. The smooth transition avoids a motor jerk upon power down. 0: instant power down, 1..15: delay per current reduction step in multiple of 2^18 clocks
} tmc51x0_ihold_irun_t;

// address 0x35: RAMP_STAT - Ramp & Reference Switch Status Register
typedef struct __attribute__((__packed__)) 
{
    uint32_t stop_by_stallguard : 1; // 1: Signals active StallGuard2 input from the CoolStep driver or from the DcStep unit, if enabled.
    // Hint: When polling this flag, stall events may be missed - activate sg_stop to be sure not to miss the stall event.
    uint32_t second_move : 1; // 1: Signals that the automatic ramp required moving back in the opposite direction, e.g. due to on-the-fly prameter change (Flag is cleared upon reading).
    uint32_t t_zerowait_active : 1; // 1: Signals that TZEROWAIT is active after a motor stop. During this time, the motor is in standstill.
    uint32_t vzero : 1; // 1: Signals that the actual velocity is 0.
    uint32_t position_reached : 1; // 1: Signals that the target position is reached. This flag becomes set while XACTUAL and XTARGET match.
    uint32_t velocity_reached : 1; // 1: Signals that the target velocity is reached. This flag becomes set while VACTUAL and VMAX match.
    uint32_t event_position_reached : 1; // 1: Signals that the target position has been reached (position_reached becoming active). Flag and interrupt condition are cleared upon reading. This bit is ORed to the interrupt output signal.
    uint32_t event_stop_by_stallguard : 1; // 1: Signals an active StallGuard2 stop event. Reading the register will clear the stall condition and the motor may re-start motion unless the motion controller has been stopped. Flag and interrupt condition are cleared upon reading. This bit is ORed to the interrupt output signal.
    uint32_t event_stop_r : 1; // 1: Signals an active stop right condition due to stop switch.
    uint32_t event_stop_l : 1; // 1: Signals an active stop left condition due to stop switch.
    uint32_t status_latch_r : 1; // 1: Latch right is ready
    uint32_t status_latch_l : 1; // 1: Latch left is ready
    uint32_t status_stop_r : 1; // 1: Reference switch right is active
    uint32_t status_stop_l : 1; // 1: Reference switch left is active
} tmc51x0_ramp_status_t;

// address: 0x6C: CHOPCONF
typedef struct __attribute__((__packed__)) 
{
    uint32_t disable_short_to_ground_protection : 1; // 0: short to ground protection active, 1: short to ground protection disabled
    uint32_t enable_double_edge_step_pulses : 1; // 1: enable step impulse at each step edge to reduce step frequency requirement
    uint32_t interpolation : 1; // 1: the actual microstep resolution (MRES) becomes extrapolated to 256 microsteps for smoothest motor operation (useful for STEP/DIR operation, only)
    uint32_t microstep_resolution : 4; // 0b0000: native 256 microstep setting. Normally use this setting with the internal motion controller.
    // 0b0001..0b1000: 128, 64, 32, 16, 8, 4, 2 , FULLSTEP
    // Reduced microstep resolution, esp. for STEP/DIR operation. The resolution gives the number of microstep entries per sine quarter wave.
    // The driver automatically uses microstep positions which result in a symmetrical wave, when choosing a lower microstep resolution.
    // step width = 2^MRES [microstep]
    union __attribute__((__packed__)) 
    {
        uint32_t pwm_sync_clock : 4; // This register allows synchronization of the chopper for both phases of a two-phase motor in order to avoid the occurrence of a beat, especially at low motor velocities. It is automatically switched off above VHIGH.
        uint32_t passive_fast_decay_time : 4; // "TPFD": 0b0000..0b1111: N_clk = 128 * TFD (0 = disable)
    };
    // 0b0000: Chopper sync function chopSync off
    // 0b0001..0b1111: Sync with f_sync = f_clk/(sync*64)
    uint32_t high_velocity_chopper_mode : 1; // This bit enables switching to chm=1 and fd=0, when VHIGH is exceeded. This way, a higher velocity can be achieved. Can be combined with vhighfs=1. If set, the TOFF setting automatically becomes doubled during high velocity operation in order to avoid doubling of the chopper frequency.
    uint32_t high_velocity_fullstep_selection : 1; // This bit enabled switching to fullstep, when VHIGH is exceeded. Switching takes place only at 45掳 position. The fullstep target current uses the current value from the microstep table at the 45掳 position.
    uint32_t sense_resistor_voltage_based_current_scaling : 1; // 0: low sensitivity, high sense resistor voltage
    // 1: high sensitivity, low sense resistor voltage
    uint32_t blank_time_select : 2; // "TBL", 0b00..0b11: set comparator blank time to 16, 24, 36 or 54 clocks. Hint: 0b01 or 0b10 is recommended for most applications.
    uint32_t chopper_mode : 1; // "CHM": 0: standard mode (SpreadCycle), 1: constant off time with fast decay time. Fast decay time is also terminated when the negative nominal current is reached. Fast decay is on after on time.
    uint32_t random_off_time : 1; // 0: fixed off time as set in register TOFF, 1: random off time, modulated by -24..+6 clocks
    uint32_t fast_decay_mode : 1; // chm=1: 1 disables the current comparator usage for termination of the fast decay cycle
    union __attribute__((__packed__)) 
    {
        uint32_t hysteresis_low_value : 4; // chm=0: "HEND": 0b0000..0b1111: Hysteresis is -3, -2, -1, 0, 1, .. 12 (1/512 of this setting adds to current setting). This is the hysteresis value which becomes used for the hysteresis chopper.
        uint32_t sine_wave_offset : 4; // chm=1: "OFFSET": 0b0000..0b1111: Offset is -3, -2, -1, 0, 1, .. 12 This is the sine wave offset and 1/512 of the value becomes added to the absolute value of each sine wave entry.
    };
    union __attribute__((__packed__)) 
    {
        uint32_t hysteresis_start_value : 3; // chm=0: "HSTRT": 0b000..0b111: Add 1, 2, 3, .. 8 to hysteresis low value HEND (1/512 of this setting adds to current setting).
        // Attention: Effective HEND+HSTRT must be <= 16.
        // Hint: Hysteresis decrement is done each 16 clocks.
        uint32_t fast_decay_time : 4; // chm=1: "TFD": Fast decay time setting.
        // 0b0000..0b1111: N_clk = 32 * TFD (0 being slow decay only)
    };
    uint32_t off_time_and_driver_enable : 4; // "TOFF": Off time setting controls duration of slow decay phase
    // N_clk = 24 + 32 * TOFF
    // 0b0000: Driver disable, all bridges off
    // 0b0001: 1: use only with TBL >= 2
    // 0b0010..0b1111: 2..15
} tmc51x0_chopconf_t;

// adress: 0x6D: COOLCONF
typedef struct __attribute__((__packed__)) 
{
    uint32_t enable_filtered_mode : 1; // 0: Standard mode, high time resolution for StallGuard2, 1: Filtered mode, StallGuard2 signal updated for each four fullsteps (resp. six fullsteps for 3 phase motors) only to compensate for motor pole tolerances
    int32_t threshold : 7; // "SGT": This signed value controls StallGuard2 level for stall output and sets the optimum measurement range for readout. a lower value gives a higher sensitivity. Zero is the starting value working with moist motors.
    // -64..63: A higher value makes StallGuard2 less sensitive and requires more torque to indicate a stall.
    uint32_t minimum_current : 1; // "SEIMIN": minimum current for smart current control, 0: 1/2 of current setting (IRUN), 1: 1/4 of IRUN
    uint32_t current_down_step_speed : 2; // "SEDN": 0b00: For each 32 StallGuard2 values, decrease by one. 0b01: Each 8, 0b10: each 2, 0b11: Each.
    uint32_t hysteresis : 4; // "SEMAX": StallGuard2 hysteresis value for smart current control. If the StallGuard2 result is >= (SEMIN+SEMAX+1)*32, the motor current becomes decreased to save energy.
    // 0b0000..0b1111: 0..15
    uint32_t current_up_step_width : 2; // "SEUP": Current increment steps per measured StallGuard2 value
    // 0b00..0b11: 1, 2, 4, 8
    uint32_t minimum : 4; // "SEMIN": minimum StallGuard2 value for smart current control and smart current enable. If the StallGuard2 result falls below SEMIN*32, the motor current becomes increased to reduce motor load angle.
    // 0b0000..0b1111: 0..15
} tmc51x0_coolconf_t;

// address 0x6E: DCCTRL
typedef struct __attribute__((__packed__)) 
{
    uint32_t dc_time : 10; // "DC_TIME": Upper PWM on time limit for commutation (DC_TIME * 1/f_clk). Set slightly above effective blank time TBL.
    uint32_t dc_stallguard : 8; // "DC_SG": Max. PWM on time for step loss detection using DcStep StallGuard2 in DcStep mode. (DC_SG * 16 / f_clk) Set slightly higher than DC_TIMEI16
} tmc51x0_dc_ctrl_t;

// address 0x70: PWMCONF
typedef struct __attribute__((__packed__)) 
{
    uint32_t pwm_auto_scale_limit : 4; // "PWM_LIM": Limit for PWM_SCALE_AUTO when switching back from SpreadCycle to StealthChop. This value defines the upper limit for bits 7 to 4 of the automatic current control when switching back. It can be set to reduce the current jerk during mode change back to StealthChop. It does not limit PWM_GRAD or PWM_GRAD_AUTO offset. Default: 12. (Only TMC5160)
    uint32_t pwm_regulation_loop_gradient : 4; // "PWM_REG": User defined maximum PWM amplitude change per half wave when using pwm_autoscale=1 (1..15). (Only TMC5160)
    // 1: 0.5 increments (slow regulation)
    // 2: 1 increment
    // 3: 1.5 increments
    // 4: 2. increments (Reset default)
    // ...
    // 8: 4 increments
    // ...
    // 15: 7.5 increments (fastest regulation)
    uint32_t freewheel : 2; // Allows different standstill modes when motor current setting is zero (IHOLD=0).
    // 0b00: normal operation
    // 0b01: Freewheeling. All phases are turned off which allows the motor to rotate freely.
    // 0b10: Coil shorted using LS drivers.
    // 0b11: Coil shorted using HS drivers.
    union __attribute__((__packed__)) 
    {
        uint32_t pwm_symmetric : 1; // 0: standard mode, the PWM value may change within each PWM cycle. 1: A symmetric PWM cycle is enforced
        uint32_t pwm_autograd : 1; // 0: Fixed value for PWM_GRAD (PWM_GRAD_AUTO = PWM_GRAD). 1: Automatic tuning (only with pwm_autoscale=1) (Reset default). PWM_GRAD_AUTO is initialized with PWM_GRAD while pwm_autograd=0 and becomes optimized automatically during motion. (TMC5160 only)
    };
    uint32_t pwm_autoscale : 1; // automatic PWM amplitutude scaling. 0: user defined PWM amplitude, the current settings have no influence. 1: enable automatic control
    uint32_t pwm_frequency_selection : 2; // "PWM_FREQ": 0b00: f_pwm=2/1024*f_clk
    // 0b01: f_pwm=2/683*f_clk
    // 0b10: f_pwm=2/512*f_clk
    // 0b11: f_pwm=2/410*f_clk
    uint32_t pwm_gradient : 8; // "PWM_GRAD": pwm_autoscale=0: velocity dependent gradient for PWM amplitude: PWM_GRAD * 256 / TSTEP is added to PWM_AMPL
    // pwm_autoscale=1: User defined maximum PWM amplitude change per half wave (1..15)
    uint32_t pwm_amplitude_offset : 8; // pwm_autoscale=0: User defined PWM amplitude offset (0..255). The resulting amplitude is: PWM_AMPL + PWM_GRAD * 256 / TSTEP (cutoff at 255)
    // pwm_autoscale=1: User defined maximum PWM amplitude when switching back from current chopper mode to voltage PWM mode (switch over velocity defined by TPWMTHRS). Do not set too low values, as the regulation cannot measure the current when the actual PWM value goes below a setting specific value. Settings above 0x40 are recommended.
} tmc51x0_pwmconf_t;

// address 0x6F: DRV_STATUS
typedef struct __attribute__((__packed__)) 
{
    uint32_t standstill : 1; // This flag indicates motor stand still in each operation mode. This occurs 2^20 clocks after the last step pulse.
    uint32_t open_load_B : 1; // 1: Open load detected on phase B.
    // Hint: This is just an informative flag. The driver takes no action upon it. False detection may occur in fast motion and standstill. Check during slow motion only.
    uint32_t open_load_A : 1; // 1: Open load detected on phase A.
    uint32_t short_to_ground_A : 1; // 1: Short to GND detected on phase A. The driver becomes disabled. The flags stay active, until the driver is disabled by software (TOFF=0) or by the ENN input.
    uint32_t short_to_ground_B : 1; // 1: Short to GND detected on phase B.
    uint32_t over_temperature_warning : 1; // 1: Overtemperature pre-warning threshold is exceeded. Common for both bridges A and B.
    uint32_t over_temperature_error : 1; // 1: Overtemperature limit has been reached. Drivers become disabled until otpw is also cleared due to cooling down of the IC. Common for both bridges A and B.
    uint32_t stallguard_status : 1; // 1: Motor stall detected (SG_RESULT=0) or DcStep stall in DcStep mode.
    uint32_t motor_current_actual : 5; // Actual current control scaling, for monitoring smart energy current scaling controlled via settings in register COOLCONF, or for monitoring the function of the automatic current scaling.
    uint32_t full_step_active : 1; // 1: The driver has switched to fullstep as defined by chopper mode settings and velocity thresholds.
    uint32_t stealthchop_active : 1; // 1: Driver operates in StealthChop mode
    uint32_t short_to_supply_B : 1; // 1: Short to supply detected on phase B. The driver becomes disabled.
    uint32_t short_to_supply_A : 1; // 1: Short to supply detected on phase B. The driver becomes disabled.
    uint32_t stallguard_result : 10; // StallGuard2 result respectively PWM on time for coil A in standstill for motor temperature detection.
    // Mechanical load measurement:
    // The StallGuard2 result gives a means to measure mechanical
    // motor load. A higher value means lower mechanical load. A
    // value of 0 signals highest load. With optimum SGT setting,
    // this is an indicator for a motor stall. The stall detection
    // compares SG_RESULT to 0 to detect a stall. SG_RESULT is used
    // as a base for CoolStep operation, by comparing it to a pro-
    // grammable upper and a lower limit. It is not applicable in
    // StealthChop mode.
    // SG_RESULT is ALSO applicable when DcStep is active.
    // StallGuard2 works best with microstep operation.
    // Temperature measurement:
    // In standstill, no StallGuard2 result can be obtained. SG_RESULT
    // shows the chopper on-time for motor coil A instead. If the
    // motor is moved to a determined microstep position at a
    // certain current setting, a comparison of the chopper on-time
    // can help to get a rough estimation of motor temperature. As
    // the motor heats up, its coil resistance rises, and the chopper
    // on-time increases.
} tmc51x0_drv_status_t;

typedef enum 
{
    TMC51X0_STATE_UNINIT = 0,
    TMC51X0_STATE_OFF,
    TMC51X0_STATE_IDLE,
    TMC51X0_STATE_ACTIVE,
    TMC51X0_STATE_ERROR
} tmc51x0_state_t;

typedef enum 
{
    TMC51X0_TYPE_TMC5130,
    TMC51X0_TYPE_TMC5160
} tmc51x0_type_t;

typedef struct tmc51x0_config 
{
    uint32_t max_velocity; 												// 最大速度
    uint32_t max_acceleration; 										// 最大加速度
    uint32_t start_acceleration; 									// 启动时的加速度
    uint32_t start_max_threshold; 								// 启动时的最大阈值
    uint32_t stealthchop_until_velocity; 					// ？
    tmc51x0_gconf_t* gconf;
    tmc51x0_ihold_irun_t* ihold_irun;
    tmc51x0_pwmconf_t* pwmconf;
    tmc51x0_chopconf_t* chopconf;
    tmc51x0_coolconf_t* coolconf;
#if USE_DCSTEP
    tmc51x0_dc_ctrl_t* dc_ctrl;
#endif

		// current scaling (only for TMC5160)
		// 当前缩放（仅适用于TMC5160）
    uint8_t global_scaler; 
    uint8_t t_powerdown;
    
    // optional setting (if encoder is present)
    // 可选设置（如果存在编码器）
    int16_t encoder_pulses_per_revolution; 
    
    // optional setting (currently used only for encoder resolution calculation)
    // 可选设置（当前仅用于编码器分辨率计算）
    int16_t steps_per_revolution; 
    
    // 对应的SPI驱动接口
    SPIDriver* spi_driver;
    //const
    SPIConfig* spi_config;
    uint8_t* tx_buffer;
    uint8_t* rx_buffer;
    ioline_t cs_line;
    ioline_t enable_line;
    ioline_t latch_line;
    int32_t turnaround_adjustment;
    tmc51x0_type_t type;
} tmc51x0_config_t;

typedef struct tmc51x0_driver 
{
    tmc51x0_state_t state; 							// 状态
    tmc51x0_config_t* config; 					// 配置指针
    float calibration_factor; 					// 校准系数
    int32_t position_actual; 						// 实际位置
    int32_t position_target; 						// 目标位置
    
    // note: this can be lower than the hard limit in the config, it's the current actual max V
    // 注意：这可能低于配置中的硬限制，它是当前实际的最大值
    uint32_t velocity_max; 
    
    bool standstill; 										// 静止不动
    bool position_reached; 							// 位置已获取
    bool velocity_reached; 							// 速度记录
    bool stallguard_active; 						// 失速防护激活
    bool driver_error; 									// 驱动报错
    bool circular_request_enabled; 			// 循环请求使能
    bool is_first_call; 								// 第一次呼叫
    
    // we do not know where we stopped; in case it was in the other direction, it could result in a jerk when first moving
    // 我们不知道停在哪里；如果它在另一个方向，第一次移动时可能会导致颠簸
    bool has_moved_before; 
    
    bool previous_direction_ccw; 				// 前一个状态的方向
} tmc51x0_driver_t;

void tmc51x0_init(tmc51x0_driver_t* driver, tmc51x0_config_t* config);
void tmc51x0_start(tmc51x0_driver_t* driver);
void tmc51x0_set_target_position(tmc51x0_driver_t* driver, int32_t target_position);
void tmc51x0_set_velocity_max(tmc51x0_driver_t* driver, uint32_t velocity);
void tmc51x0_set_acceleration_max(tmc51x0_driver_t* driver, uint32_t acceleration);
uint32_t tmc51x0_get_velocity_max(tmc51x0_driver_t* driver);
void tmc51x0_stop(tmc51x0_driver_t* driver);
int32_t tmc51x0_get_current_position(tmc51x0_driver_t* driver);
int32_t tmc51x0_get_latched_position(tmc51x0_driver_t* driver);
void tmc51x0_adjust_nominal_position(tmc51x0_driver_t* driver, int32_t offset);
void tmc51x0_await_standstill(tmc51x0_driver_t* driver);
void tmc51x0_enable_refl_stop(tmc51x0_driver_t* driver);
void tmc51x0_enable_refl_latch(tmc51x0_driver_t* driver);
tmc51x0_drv_status_t tmc51x0_get_driver_status(tmc51x0_driver_t* driver);
bool tmc51x0_encoder_deviation_detected(tmc51x0_driver_t* driver);
void tmc51x0_encoder_clear_deviation_warning(tmc51x0_driver_t* driver);
tmc51x0_gstat_t tmc51x0_get_global_status(tmc51x0_driver_t* driver);
void tmc51x0_clear_errors(tmc51x0_driver_t* driver);
void tmc51x0_spi_transaction_callback(SPIDriver* spip);
tmc51x0_driver_t* tmc51x0_get_current_driver(void);
tmc51x0_ramp_status_t tmc51x0_get_ramp_status(tmc51x0_driver_t* driver);
uint32_t tmc51x0_get_tstep_actual(tmc51x0_driver_t* driver);
tmc51x0_pwm_status_t tmc51x0_get_pwm_status(tmc51x0_driver_t* driver);

#endif //TMC51X0_H
