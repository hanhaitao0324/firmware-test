#include <string.h>

// if you wish to override, these must be defined before including ste.h
//#define STE_DEBUG_ENABLED TRUE
//#define STE_TRACE_ENABLED TRUE

#include "ste.h"
#include "tmc51x0.h"

#define WRITE_ACCESS		0x80	// Write access for spi communication spi通信的写入访问
#define READ_ACCESS			0x00	// Read access for spi communication spi通信的读取访问

// TODO: this is very specific for THT, make it generic in the future
// TODO:这对THT来说是非常具体的，将来将其通用化
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

static tmc51x0_driver_t* current_driver = NULL;

typedef enum 
{
    TMC51X0_RAMP_MODE_POSITIONING = 0, // positioning mode (using all A, D and V paramters)
    TMC51X0_RAMP_MODE_VELOCITY_POS_VMAX = 1, // velocity mode to positive VMAX (using AMAX acceleration)
    TMC51X0_RAMP_MODE_VELOCITY_NEG_VMAX = 2, // velocity mode to negative VMAX (using AMAX acceleration)
    TMC51X0_RAMP_MODE_HOLD = 3 // hold mode (velocity remains unchanged, unless stop event occurs)
} tmc51x0_ramp_mode_t;


static uint32_t tmc51x0_register_read(tmc51x0_driver_t* driver, uint8_t address);
static void tmc51x0_register_write(tmc51x0_driver_t* driver, uint8_t address, uint32_t data);
static void tmc51x0_calibrate(tmc51x0_driver_t* driver);
static void tmc51x0_set_ramp_mode(tmc51x0_driver_t* driver, tmc51x0_ramp_mode_t mode);
static void tmc51x0_set_coolstep_config(tmc51x0_driver_t* driver, tmc51x0_coolconf_t* coolconf);
static void tmc51x0_set_pwm_config(tmc51x0_driver_t* driver, tmc51x0_pwmconf_t* pwmconf);
static void tmc51x0_set_chopper_config(tmc51x0_driver_t* driver, tmc51x0_chopconf_t* chopconf);
static void tmc51x0_set_general_config(tmc51x0_driver_t* driver, tmc51x0_gconf_t* gconf);
static void tmc51x0_set_ihold_irun(tmc51x0_driver_t* driver, tmc51x0_ihold_irun_t* ihold_irun);
static void tmc51x0_set_t_powerdown(tmc51x0_driver_t* driver, uint32_t t_powerdown);
static void tmc51x0_set_t_pwm_threshold(tmc51x0_driver_t* driver, uint32_t t_pwm_threshold);
static void tmc51x0_set_t_cool_threshold(tmc51x0_driver_t* driver, uint32_t t_coolthrs);
static void tmc51x0_set_t_high(tmc51x0_driver_t* driver, uint32_t t_high);
static void tmc51x0_set_velocity_max_raw(tmc51x0_driver_t* driver, uint32_t velocity);
#if USE_DCSTEP
static void tmc51x0_set_dc_ctrl(tmc51x0_driver_t* driver, tmc51x0_dc_ctrl_t* dc_ctrl);
#endif

void tmc51x0_init(tmc51x0_driver_t* driver, tmc51x0_config_t* config) 
{
    bool first_time = false;
    if (!driver->config) 
    {
        driver->config = config;
        first_time = true;
    }
    
    // make sure motor is completely turned off. 确保电机完全关闭。
    palSetLine(config->enable_line);
    tmc51x0_register_write(driver, TMC51X0_REG_VELOCITY_MAX, 0);
    tmc51x0_set_chopper_config(driver, &(tmc51x0_chopconf_t) 
    {
        .off_time_and_driver_enable = 0
    });
    
    // in case the motor was running, give it some time to stop. 万一马达在运转，给它一点时间停下来。
    systime_t start = chVTGetSystemTimeX();
    tmc51x0_await_standstill(driver);
    systime_t end = chVTGetSystemTimeX();
    if (TIME_I2MS(chTimeDiffX(start, end)) > 1) 
    	trace("needed %d ms to stop motor\n", TIME_I2MS(chTimeDiffX(start, end)));
    
    // wherever we are, this shall be position 0.
    // 无论我们在哪里，这都将是位置0。
    tmc51x0_register_write(driver, TMC51X0_REG_POSITION_CURRENT, 0);
    driver->position_actual = 0;
    tmc51x0_register_write(driver, TMC51X0_REG_POSITION_TARGET, 0);
    driver->position_target = 0;
    
    // deactivate while configuring
    // 配置时停用
    tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_DEVIATION, 0); 
    tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_POSITION, 0);
    
    if (first_time) 
    {
        tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_STATUS, 2); // clear bits
        tmc51x0_calibrate(driver);
    }
    
    driver->state = TMC51X0_STATE_OFF;
}

void tmc51x0_start(tmc51x0_driver_t* driver) 
{
    assert(driver->state == TMC51X0_STATE_OFF, "init driver first");
    
    // f_clk = internal (nominally 13.2 MHz for TMC5130, 12 MHz for TMC5160)
    
    /* in order to avoid ugly noises when the motors first magnetize, we want to gradually increase the current, as the
     * IC otherwise goes directly from i_hold to i_run and the motor makes a loud noise. When powering down, it's smart
     * enough to offer a gradual powerdown, so once we start moving, we restore the intended user settings and things
     * will be done correctly automatically. */
    tmc51x0_ihold_irun_t ihold_irun = 
    {
        .i_hold = 1,
        .i_run = 1,
        .i_hold_delay = 0
    };
    tmc51x0_set_ihold_irun(driver, &ihold_irun);
    
    // can happen after a software reset
    tmc51x0_drv_status_t driver_status = tmc51x0_get_driver_status(driver);
    if (!driver->has_moved_before) 
    {
        while (driver_status.motor_current_actual > 1) 
        {
            wait(1);
            driver_status = tmc51x0_get_driver_status(driver);
        }
    }
    
    tmc51x0_set_t_powerdown(driver, driver->config->t_powerdown);
    tmc51x0_set_pwm_config(driver, driver->config->pwmconf);
    // enable StealthChop only after setting PWM parameters
    tmc51x0_set_general_config(driver, driver->config->gconf);
    
    /* Note: The time base for the TMC51X0 speeds is actually t = 2^24 / f_clk, not 1 second. For the TMC5160, the nominal
     * f_clk is 12 MHz, for TMC5130, it's 13.2 MHz.
     * e.g.: calculation for 9000 steps per seond, 256 microstepping, TMC5160:
     * (9000*256)*((9000*256)/((9000*256)/((2^24)/12000000))) = 3221225
     * -> TMC5160 factor = 1,3981
     * -> TMC5130 factor = 1,271
     */
    
    float driver_factor = 1.271f;
    if (driver->config->type == TMC51X0_TYPE_TMC5160) 
    {
        driver_factor = 1.3981f;
    }
    
    float velocity_temp = (float)driver->config->max_velocity * driver->calibration_factor * driver_factor;
    
    uint32_t max_velocity = velocity_temp + 0.5f;
    uint32_t t_step_calculated = ((10000000.0f * 5.0f) / (velocity_temp * 3.0f)) + 0.5f;
    trace("max_velocity: %u vs. %u, t_step_calculated: %u\n",
          max_velocity, driver->config->max_velocity, t_step_calculated);
    
    if (driver->config->stealthchop_until_velocity) 
    {
        velocity_temp = (float)driver->config->stealthchop_until_velocity * driver->calibration_factor * driver_factor;
        /* the reason for the 0.97 is that the TMC5160 calculates slightly differently than us, so if the threshold
         * is configured sharply, we want to ensure it's actually working. */
        t_step_calculated = ((((10000000.0f * 5.0f) / (velocity_temp * 3.0f))) * 0.97f) + 0.5f;
        trace("stealthchop_until_velocity: %f vs. %u, t_step_calculated: %u\n",
            (double)velocity_temp, driver->config->stealthchop_until_velocity, t_step_calculated);
        tmc51x0_set_t_pwm_threshold(driver, t_step_calculated);
    } 
    else 
    {
        tmc51x0_set_t_pwm_threshold(driver, t_step_calculated);
    }
    
#if USE_DCSTEP
    tmc51x0_register_write(driver, TMC51X0_REG_VDC_MIN, max_velocity / 1.1);
    tmc51x0_set_dc_ctrl(driver, driver->config->dc_ctrl);
#else
    if (driver->config->type != TMC51X0_TYPE_TMC5160) 
    {
        tmc51x0_register_write(driver, TMC51X0_REG_VDC_MIN, 0x3fffff);
    }
#endif

#if USE_COOLSTEP
    tmc51x0_set_t_high(driver, t_step_calculated);
#else
    if (driver->config->type != TMC51X0_TYPE_TMC5160) 
    	tmc51x0_set_t_high(driver, 0);
#endif

#if USE_DCSTEP || USE_COOLSTEP
    tmc51x0_set_t_cool_threshold(driver, t_step_calculated * 2);
#else
    if (driver->config->type != TMC51X0_TYPE_TMC5160) 
    	tmc51x0_set_t_cool_threshold(driver, 0xffff);
#endif
    
    if (driver->config->type != TMC51X0_TYPE_TMC5160) 
    	tmc51x0_set_coolstep_config(driver, driver->config->coolconf);
    
    tmc51x0_register_write(driver, TMC51X0_REG_RAMP_MODE, TMC51X0_RAMP_MODE_POSITIONING);
    tmc51x0_register_write(driver, TMC51X0_REG_TZERO_WAIT, 10);
    tmc51x0_register_write(driver, TMC51X0_REG_VELOCITY_STOP, 10);
    tmc51x0_register_write(driver, TMC51X0_REG_VELOCITY_INITIAL, 0);
    tmc51x0_register_write(driver, TMC51X0_REG_ACCELERATION_INITIAL, driver->config->start_acceleration);
    tmc51x0_register_write(driver, TMC51X0_REG_VELOCITY_THRESHOLD,
                           (float)driver->config->start_max_threshold * driver->calibration_factor * driver_factor);
    tmc51x0_register_write(driver, TMC51X0_REG_ACCELERATION_MAX, driver->config->max_acceleration);
    tmc51x0_set_velocity_max_raw(driver, max_velocity);
    driver->velocity_max = driver->config->max_velocity;
    tmc51x0_register_write(driver, TMC51X0_REG_DECELERATION_MAX, driver->config->max_acceleration);
    tmc51x0_register_write(driver, TMC51X0_REG_DECELERATION_INITIAL, driver->config->start_acceleration);
    
    tmc51x0_set_chopper_config(driver, driver->config->chopconf);
    if (driver->config->type == TMC51X0_TYPE_TMC5160) 
    {
        uint32_t setting;
        
        setting = driver->config->global_scaler;
        tmc51x0_register_write(driver, TMC51X0_REG_GLOBAL_SCALER, setting);
        
        //        FILT_ISENSE DRVSTRENGTH OTSELECT    BBMCLKS    BBMTIME
        //setting = (4 << 20) | (2 << 18) | (1 << 16) | (4 << 8) | (22 << 0);
        // according to measurements, we need a higher break-before-make time than the default:
        // -> BBMTIME setting 22 (100*32/(32-20)) = ~320ns
        // -> BBMCLKS at 4 (4 * ~83ns) = ~332ns
        // the larger setting of BBMTIME vs BBMCLKS wins
        setting = (0 << 20) | (0 << 18) | (1 << 16) | (4 << 8) | (22 << 0);
        tmc51x0_register_write(driver, TMC51X0_REG_DRV_CONF, setting);
    }
    
    if (driver->config->encoder_pulses_per_revolution) 
    {
        /* Example for the following calculation:
         * The specification of the PKP266D28A2-R2F says 400 P/R. However, this refers to the pulses as such. The
         * motor driver IC, however, counts each level change (hi to lo/lo to high), so you have 4 edges per pulse,
         * so we have to calculate with 1600 P/R:
         * factor = (200 * 256) / 1600 = 32
         * Also, if the encoder resolution was specified as a negative, the normal motor direction is reversed from
         * the encoder's point of view, so we have to do the setting as negative, i.e. as two's complement:
         * (32 XOR 0xffff)+1 = 0xffe0
         */
        dbg("encoder mode: %08x, status: %08x\n",
            tmc51x0_register_read(driver, TMC51X0_REG_ENCODER_MODE),
            tmc51x0_register_read(driver, TMC51X0_REG_ENCODER_STATUS));
        
        float enc_res_denominator = driver->config->encoder_pulses_per_revolution * 4.0f;
        bool was_negative = false;
        if (enc_res_denominator < 0.0f) 
        {
            enc_res_denominator = 0.0f - enc_res_denominator;
            was_negative = true;
        }
        // number of configured microsteps per fullstep: 2 ^ (8 - microstep_resolution)
        float microsteps = 1 << (8 - driver->config->chopconf->microstep_resolution);
        float enc_res = (driver->config->steps_per_revolution * microsteps) / enc_res_denominator;
        /* We can choose whether the fractional part (lower 16 bits) of ENC_CONST shall be interpreted as base 2 or
         * base 10. We simplify this to "if it cannot be accurately represented as float, assume decimal works
         * better".
         * TODO: implement that if needed (or make the user choose explicitly)
         if (...) {
              tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_MODE, 1 << 10); // decimal mode
              dbg("encoder mode configured for decimal fraction\n");
          } else {
              tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_MODE, 0 << 10); // binary mode
          }
         */
        uint32_t enc_const = ((((uint16_t)enc_res) << 16) & 0xFFFF0000)
                             | (((uint16_t)((enc_res - (uint32_t)enc_res) * 10000.0f)) & 0xFFFF);
        if (was_negative) 
        {
            enc_const = ((((((uint16_t)enc_res) ^ 0xFFFF) + 1) << 16) & 0xFFFF0000)
                        | (((uint16_t)((enc_res - (uint32_t)enc_res) * 10000.0f)) & 0xFFFF);
        }
        dbg("ENC_CONST: factor %.2f -> setting 0x%08X\n",
            (double)((was_negative)? 0.0f - enc_res : enc_res), enc_const);
        tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_CONST, enc_const);
        tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_DEVIATION, MAX_ENCODER_DEVIATION);
        dbg("encoder mode: %08x, status: %08x\n",
            tmc51x0_register_read(driver, TMC51X0_REG_ENCODER_MODE),
            tmc51x0_register_read(driver, TMC51X0_REG_ENCODER_STATUS));
    }
    
    tmc51x0_enable_refl_latch(driver);
    
    palClearLine(driver->config->enable_line);
    
    driver->state = TMC51X0_STATE_IDLE;
}

void tmc51x0_enable_refl_latch(tmc51x0_driver_t* driver) 
{
    if (driver->config->latch_line) 
    {
        uint32_t sw_mode = (1 << 5); // latch_l_active: latch upon REFL going active
        tmc51x0_register_write(driver, TMC51X0_REG_SW_MODE, sw_mode);
        trace("enabling reference switch left as latch\n");
    }
}

void tmc51x0_enable_refl_stop(tmc51x0_driver_t* driver) 
{
    if (driver->config->latch_line) 
    {
        uint32_t sw_mode = (1 << 0); // enable auto stop on reference switch active;
        tmc51x0_register_write(driver, TMC51X0_REG_SW_MODE, sw_mode);
        trace("enabling reference switch left as stop trigger\n");
    }
}

tmc51x0_pwm_status_t tmc51x0_get_pwm_status(tmc51x0_driver_t* driver) 
{
    uint32_t pwm_scale = tmc51x0_register_read(driver, TMC51X0_REG_PWM_SCALE);
    uint32_t pwm_auto = tmc51x0_register_read(driver, TMC51X0_REG_PWM_AUTO);
    tmc51x0_pwm_status_t pwmstat = {0};
    pwmstat.pwm_scale_sum = pwm_scale & 0xFF;
    pwmstat.pwm_scale_auto = (pwm_scale & (0xff << 16)) >> 16;
    if (((pwm_scale & 1) << 24) != 0) 
    	pwmstat.pwm_scale_auto = -pwmstat.pwm_scale_auto;
    pwmstat.pwm_auto_offset = pwm_auto & 0xFF;
    pwmstat.pwm_auto_gradient = (pwm_auto & (0xff << 16)) >> 16;
    return pwmstat;
}

inline tmc51x0_driver_t* __attribute__((always_inline)) tmc51x0_get_current_driver(void) 
{
    /* Intentionally no lock, since it is called very often from an ISR callback. Since pointer reads and writes
     * are atomic on ARM, this should be safe. */
    tmc51x0_driver_t* tmp = current_driver;
    return tmp;
}

void tmc51x0_set_current_driver(tmc51x0_driver_t* driver) 
{
    assert(driver != NULL, "invalid driver");
    
#if STE_DEBUG_ENABLED
    if (current_driver != NULL && driver != current_driver) 
    {
        extern tmc51x0_driver_t MAGAZINE;
        extern tmc51x0_driver_t THICKNESS_GAUGE;
        extern tmc51x0_driver_t BREAKING_STATION;
        char* current_driver_name = "unknown";
        char* new_driver_name = "unknown";
        if (current_driver == NULL) 
        {
            current_driver_name = "NULL";
        } 
        else if (current_driver == &MAGAZINE) 
        {
            current_driver_name = "magazine";
        } 
        else if (current_driver == &THICKNESS_GAUGE) 
        {
            current_driver_name = "thickness gauge";
        } 
        else if (current_driver == &BREAKING_STATION) 
        {
            current_driver_name = "breaking station";
        }
        if (driver == &MAGAZINE) 
        {
            new_driver_name = "magazine";
        } 
        else if (driver == &THICKNESS_GAUGE) 
        {
            new_driver_name = "thickness gauge";
        } 
        else if (driver == &BREAKING_STATION) 
        {
            new_driver_name = "breaking station";
        }
        dbg("switching from %s to %s\n", current_driver_name, new_driver_name);
    }
#endif
    chSysLock();
    if (current_driver != NULL && driver != current_driver) 
    {
        if (current_driver->circular_request_enabled) 
        {
            current_driver->circular_request_enabled = false;
        }
        spiSynchronizeS(current_driver->config->spi_driver, TIME_INFINITE);
        spiUnselectI(current_driver->config->spi_driver);
        chSchRescheduleS();
    }
    current_driver = driver;
    ((SPIConfig*)current_driver->config->spi_driver->config)->ssline = driver->config->cs_line;
    chSysUnlock();
}

void tmc51x0_await_standstill(tmc51x0_driver_t* driver) 
{
    if (driver == NULL || driver != tmc51x0_get_current_driver()) 
    {
        return;
    }
    
    do 
    {
        if (!driver->circular_request_enabled) 
        {
            // start getting position, which will update the standstill flag, and will continue because the spi callback
            // will restart the request automatically
            tmc51x0_get_current_position(driver);
        } 
        else 
        {
            // await next result
            spiSynchronize(driver->config->spi_driver, TIME_MS2I(1));
        }
    } while (!driver->standstill);
}

void CC_FAST tmc51x0_spi_transaction_callback(SPIDriver* spip) 
{
    spiUnselectI(spip);
    
    tmc51x0_driver_t* driver = tmc51x0_get_current_driver();
    if (driver == NULL) 
    {
        return;
    }
    
    uint8_t* tx_buffer = driver->config->tx_buffer;
    uint8_t* rx_buffer = driver->config->rx_buffer;
    cacheBufferInvalidate(rx_buffer, 5);
    
    driver->driver_error = (rx_buffer[0] & (1 << 1)) >> 1;
    driver->stallguard_active = (rx_buffer[0] & (1 << 2)) >> 2;
    driver->standstill = (rx_buffer[0] & (1 << 3)) >> 3;
    driver->velocity_reached = (rx_buffer[0] & (1 << 4)) >> 4;
    driver->position_reached = (rx_buffer[0] & (1 << 5)) >> 5;
    
    if (driver->driver_error) 
    {
        bool was_enabled = !palReadLineLatch(driver->config->enable_line);
        palSetLine(driver->config->enable_line);
        if (was_enabled) 
        	logI("DRIVER ERROR\n");
    }
    
    if (driver->standstill && driver->state == TMC51X0_STATE_ACTIVE && driver->position_reached) 
    	driver->state = TMC51X0_STATE_IDLE;
    
    if (driver->circular_request_enabled && tx_buffer[0] == TMC51X0_REG_POSITION_CURRENT) 
    {
        if (unlikely(driver->is_first_call) == 1) 
        {
            driver->is_first_call = 0;
            return;
        }
        
        chSysLockFromISR();
        driver->position_actual = (rx_buffer[1] << 24) | (rx_buffer[2] << 16) | (rx_buffer[3] << 8) | (rx_buffer[4] << 0);
        spip->state = SPI_READY;
        spiSelectI(spip);
        spiStartExchangeI(spip, 5, tx_buffer, rx_buffer);
        chSysUnlockFromISR();
    }
}

static uint32_t tmc51x0_register_read(tmc51x0_driver_t* driver, uint8_t address) 
{
    /* Unfortunately, we can't easily use the native SPI circular mode built into the MCU, as the TMC51X0 must see a
     * toggle on CS, otherwise it returns nonsensical/stale data. (The STM32F3XX theoretically has the ability to do
     * hardware CS pulsing after a transfer, but we have 4 drivers on the same line, so even if that PIN were connected
     * it would help at most with 1 driver). To improve things a bit, we use a callback that takes care of unselecting
     * CS and fetching the status bits from the first response byte.
     *
     * On STM32F3XX, we can do even better than that: we need position polling to be as fast as possible at all because
     * we want to have the highest precision of the position when certain events occur (e.g. Magazine home detected, or
     * tablet contact when doing diameter measurements). However, we routed two lines to the REFL (reference switch
     * left) of the magazine and breaking station motor drivers. So we only need to pull these pins high, and the driver
     * will latch the current position for us, which we can read out at a convenient time. Then, it's sufficient to
     * update the position only once per millisecond, which lowers our interrupt rate from >50 kHz just for position
     * updates to ~1 kHz. */
    
    if (tmc51x0_get_current_driver() != driver) 
    {
        tmc51x0_set_current_driver(driver);
    }

#if STM32H7XX
    if (address == TMC51X0_REG_POSITION_CURRENT) 
    {
        if (driver->circular_request_enabled) 
        {
            return driver->position_actual;
        } 
        else 
        {
            driver->circular_request_enabled = 1;
            driver->is_first_call = 1;
        }
    } 
    else 
    {
        if (driver->circular_request_enabled) 
        {
            driver->circular_request_enabled = 0;
            spiSynchronize(current_driver->config->spi_driver, TIME_INFINITE);
        }
    }
#endif
    SPIDriver* spi_driver = driver->config->spi_driver;
    uint8_t* tx_buffer = driver->config->tx_buffer;
    uint8_t* rx_buffer = driver->config->rx_buffer;
    memset(tx_buffer, 0, 5);
    memset(rx_buffer, 0, 5);
    
    // we basically send two requests after one another, the first one sets up the read, the seconds gets the actual data
    tx_buffer[0] = address | READ_ACCESS;
    
    chSysLock();
    spiSelectI(spi_driver);
    msg_t result = spiStartExchangeI(spi_driver, 5, tx_buffer, rx_buffer);
    if (result == MSG_OK) 
    {
        spiSynchronizeS(spi_driver, TIME_INFINITE);
    } 
    else 
    {
        spiUnselectI(spi_driver);
        chSysUnlock();
        trace("spiStartExchangeI failed\n");
        return 0;
    }
    
    // trace only the setup phase
    if (driver->calibration_factor < 0.1f) 
    {
        cacheBufferInvalidate(&rx_buffer[0], 5);
        traceI("spiExchange -> 0x %02X 00 00 00 00\n", address);
        traceI("spiExchange <- 0x %02X %02X %02X %02X %02X\n", rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3], rx_buffer[4]);
    }
#if STM32H7XX
    if (address == TMC51X0_REG_POSITION_CURRENT && driver->circular_request_enabled && driver->is_first_call == 1) 
    {
        /* The callback will have restarted the communication already. To get a valid result, we simply need to wait
         * for the next SPI transaction to finish. We need to ignore the first result of a repeat request, too, as it
         * will be the result data of whatever request came before. */
        spiSynchronizeS(spi_driver, TIME_INFINITE);
        chSysUnlock();
        return driver->position_actual; // will have been updated by the callback at this point
    } 
    else 
    {
#endif
        spiSelectI(spi_driver);
        result = spiStartExchangeI(spi_driver, 5, tx_buffer, rx_buffer);
        if (result == MSG_OK) 
        {
            spiSynchronizeS(spi_driver, TIME_INFINITE);
        } 
        else 
        {
            spiUnselectI(spi_driver); // we really need to do this, otherwise received data is nonsensical
        }
        cacheBufferInvalidate(&rx_buffer[0], 5);
        uint32_t data = (rx_buffer[1] << 24) | (rx_buffer[2] << 16) | (rx_buffer[3] << 8) | (rx_buffer[4] << 0);
        chSysUnlock();
        
        if (driver->calibration_factor < 0.1f) 
        {
            trace("spiExchange -> 0x %02X 00 00 00 00\n", address);
            trace("spiExchange <- 0x %02X %02X %02X %02X %02X\n", rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3], rx_buffer[4]);
        }
        
        return data;
#if STM32H7XX
    }
#endif
}

static void tmc51x0_register_write(tmc51x0_driver_t* driver, uint8_t address, uint32_t data) 
{
    if (tmc51x0_get_current_driver() != driver) 
    {
        tmc51x0_set_current_driver(driver);
    }
    if (driver->circular_request_enabled) 
    {
        driver->circular_request_enabled = 0;
        spiSynchronize(current_driver->config->spi_driver, TIME_INFINITE);
    }
    
    SPIDriver* spi_driver = driver->config->spi_driver;
    uint8_t* tx_buffer = driver->config->tx_buffer;
    uint8_t* rx_buffer = driver->config->rx_buffer;
    memset(tx_buffer, 0, 5);
    memset(rx_buffer, 0, 5);
    
    tx_buffer[0] = address | WRITE_ACCESS;
    tx_buffer[1] = (data >> 24) & 0xFF;
    tx_buffer[2] = (data >> 16) & 0xFF;
    tx_buffer[3] = (data >> 8) & 0xFF;
    tx_buffer[4] = (data >> 0) & 0xFF;
    
    chSysLock();
    spiSelectI(spi_driver);
    msg_t result = spiStartExchangeI(spi_driver, 5, tx_buffer, rx_buffer);
    if (result == MSG_OK) 
    {
        spiSynchronizeS(spi_driver, TIME_INFINITE);
    } 
    else 
    {
        spiUnselectI(spi_driver); // we really need to do this, otherwise received data is nonsensical
    }
    chSysUnlock();
    
    if (driver->calibration_factor < 0.1f) 
    {
        cacheBufferInvalidate(&rx_buffer[0], 5);
        trace("spiExchange -> 0x %02X %02X %02X %02X %02X\n", tx_buffer[0], tx_buffer[1], tx_buffer[2], tx_buffer[3], tx_buffer[4]);
        trace("spiExchange <- 0x %02X %02X %02X %02X %02X\n", rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3], rx_buffer[4]);
    }
}

static void tmc51x0_ramp_up_current(tmc51x0_driver_t* driver) 
{
    /* in order to avoid ugly noises when the motors first magnetize, we want to gradually increase the current, as the
     * IC otherwise goes directly from i_hold to i_run and the motor makes a loud noise. When powering down, it's smart
     * enough to offer a gradual powerdown, so once we start moving, we restore the intended user settings and things
     * will be done correctly automatically. */
    if (!driver->has_moved_before) 
    	wait(driver->config->ihold_irun->i_hold_delay + 3);
    tmc51x0_drv_status_t driver_status = tmc51x0_get_driver_status(driver);
    
    trace("has_moved_before: %d, i_hold_delay: %d, i_run: %d, i_hold: %d, i_actual: %d\n",
        driver->has_moved_before, driver->config->ihold_irun->i_hold_delay, driver->config->ihold_irun->i_run,
        driver->config->ihold_irun->i_hold, driver_status.motor_current_actual);
    if (!driver->has_moved_before
        || (driver->config->ihold_irun->i_hold_delay > 0
            && driver->config->ihold_irun->i_run > driver->config->ihold_irun->i_hold
            && driver_status.motor_current_actual < driver->config->ihold_irun->i_run)) 
    {
        uint32_t i_hold_target = driver->config->ihold_irun->i_hold;
        uint32_t i_run_target = driver->config->ihold_irun->i_run;
        driver->config->ihold_irun->i_hold = driver_status.motor_current_actual;
        while (i_run_target > driver->config->ihold_irun->i_hold) 
        {
            driver->config->ihold_irun->i_hold += 1;
            tmc51x0_set_ihold_irun(driver, driver->config->ihold_irun);
            trace("i_hold %u -> %u\n", driver->config->ihold_irun->i_hold - 1, driver->config->ihold_irun->i_hold);
            wait(driver->config->ihold_irun->i_hold_delay + 3);
        }
        driver->config->ihold_irun->i_hold = i_hold_target;
    }
}

void tmc51x0_set_target_position(tmc51x0_driver_t* driver, int32_t target_position) 
{
    bool direction_ccw = (target_position < driver->position_target);
    // this will also update the standstill flag
    int32_t pos = tmc51x0_get_current_position(driver);
    
    /* in order to avoid ugly noises when the motors first magnetize, we want to gradually increase the current, as the
     * IC otherwise goes directly from i_hold to i_run and the motor makes a loud noise. When powering down, it's smart
     * enough to offer a gradual powerdown, so once we start moving, we restore the intended user settings and things
     * will be done correctly automatically. */
    tmc51x0_ramp_up_current(driver);
    
    // TMC5160 offers automatic tuning of StealthChop parameters. To enable it, it has to be at nominal I_RUN
    // current for max. 130ms while standing still. Then, after the next movement over >= 8 fullsteps the IC will
    // automatically tune parameters.
    if (driver->config->type == TMC51X0_TYPE_TMC5160 && !driver->has_moved_before) 
    {
        wait(130);
    }
    
    if (driver->config->encoder_pulses_per_revolution) 
    {
        /* After applying current to the coils, the position might have changed a bit, and we want to avoid that that
         * triggers the deviation warning. (This also realigns encoder and driver actual position.) */
        tmc51x0_encoder_clear_deviation_warning(driver);
    }
    
    if (driver->config->turnaround_adjustment
        && (!driver->has_moved_before || direction_ccw != driver->previous_direction_ccw)) 
    {
        
        if (!driver->standstill) 
        {
            do 
            {
                tmc51x0_get_driver_status(driver);
            } while (!driver->standstill);
            
            pos = tmc51x0_get_current_position(driver);
            
            trace("stopped at: %d\n", pos);
        }
        
        int offset = driver->config->turnaround_adjustment;
        if (driver->config->encoder_pulses_per_revolution) 
        {
            tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_DEVIATION, 0); // deactivate during adjustment
        }
        if (target_position < pos) 
        {
            tmc51x0_register_write(driver, TMC51X0_REG_POSITION_CURRENT, pos + offset);
            tmc51x0_register_write(driver, TMC51X0_REG_POSITION_TARGET, pos + offset);
            if (driver->config->encoder_pulses_per_revolution) 
            {
                tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_POSITION, pos + offset);
            }
            driver->position_target = pos + offset;
            driver->position_actual = pos + offset;
        } 
        else 
        {
            tmc51x0_register_write(driver, TMC51X0_REG_POSITION_CURRENT, pos - offset);
            tmc51x0_register_write(driver, TMC51X0_REG_POSITION_TARGET, pos - offset);
            if (driver->config->encoder_pulses_per_revolution) 
            {
                tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_POSITION, pos - offset);
            }
            driver->position_target = pos - offset;
            driver->position_actual = pos - offset;
        }
        if (driver->config->encoder_pulses_per_revolution) 
        {
            tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_STATUS, 2);
            tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_DEVIATION, MAX_ENCODER_DEVIATION);
        }
        trace("repos now\n");
        tmc51x0_set_velocity_max(driver, 2560);
        tmc51x0_register_write(driver, TMC51X0_REG_ACCELERATION_MAX, 256);
        tmc51x0_register_write(driver, TMC51X0_REG_POSITION_TARGET, pos);
        
        do 
        {
            tmc51x0_get_driver_status(driver);
        } while (!driver->standstill);
        
        trace("repos done\n");
        tmc51x0_set_velocity_max(driver, driver->config->max_velocity);
        tmc51x0_register_write(driver, TMC51X0_REG_ACCELERATION_MAX, driver->config->max_acceleration);
        
    }
    
    driver->state = TMC51X0_STATE_ACTIVE;
    tmc51x0_register_write(driver, TMC51X0_REG_POSITION_TARGET, target_position);
    driver->position_target = target_position;
    driver->previous_direction_ccw = direction_ccw;
    driver->has_moved_before = true;
    
    // we're moving now, so we can restore the original i_hold target so powerdown works automatically
    tmc51x0_set_ihold_irun(driver, driver->config->ihold_irun);
}

void tmc51x0_set_velocity_max(tmc51x0_driver_t* driver, uint32_t velocity) 
{
    float driver_factor = 1.271f;
    if (driver->config->type == TMC51X0_TYPE_TMC5160) 
    {
        driver_factor = 1.3981f;
    }
    float max_velocity = (float)driver->config->max_velocity * driver->calibration_factor * driver_factor;
    float velocity_f = (float)velocity * driver->calibration_factor * driver_factor;
    trace("max_vel=%f, vel=%f\n", (double)max_velocity, (double)velocity_f);
    if (velocity_f > max_velocity) 
    {
        velocity_f = max_velocity;
    }
    tmc51x0_set_velocity_max_raw(driver, velocity_f);
    driver->velocity_max = velocity;
}

static void tmc51x0_set_velocity_max_raw(tmc51x0_driver_t* driver, uint32_t velocity) 
{
    if (velocity > 0x7ffe00) 
    {
        // 2^23-512 is the maximum value for VMAX
        log("warning: VMAX %u too high, limiting to 0x7ffe00\n", velocity);
        velocity = 0x7ffe00;
    }
    trace("setting velocity max to %u at pos %d\n", velocity, driver->position_actual);
    tmc51x0_register_write(driver, TMC51X0_REG_VELOCITY_MAX, velocity);
}

uint32_t tmc51x0_get_velocity_max(tmc51x0_driver_t* driver) 
{
    return driver->velocity_max;
}

void tmc51x0_set_acceleration_max(tmc51x0_driver_t* driver, uint32_t acceleration) 
{
    if (acceleration > 0xFFFF) 
    {
        acceleration = 0xFFFF;
    }
    // TODO: this should be split into initial and max...
    tmc51x0_register_write(driver, TMC51X0_REG_ACCELERATION_INITIAL, acceleration);
    tmc51x0_register_write(driver, TMC51X0_REG_ACCELERATION_MAX, acceleration);
    tmc51x0_register_write(driver, TMC51X0_REG_DECELERATION_MAX, acceleration);
    tmc51x0_register_write(driver, TMC51X0_REG_DECELERATION_INITIAL, acceleration);
}

void tmc51x0_stop(tmc51x0_driver_t* driver) 
{
    tmc51x0_set_velocity_max(driver, 0);
}

int32_t tmc51x0_get_current_position(tmc51x0_driver_t* driver) 
{
    int32_t current_position = (int32_t)tmc51x0_register_read(driver, TMC51X0_REG_POSITION_CURRENT);
    //if (driver->calibration_factor < 0.1f) trace("current_position = %d\n", current_position);
    driver->position_actual = current_position;
    return current_position;
}

int32_t tmc51x0_get_latched_position(tmc51x0_driver_t* driver) 
{
    int32_t latched_position = (int32_t)tmc51x0_register_read(driver, TMC51X0_REG_POSITION_LATCHED);
    return latched_position;
}

void tmc51x0_adjust_nominal_position(tmc51x0_driver_t* driver, int32_t new_position) 
{
    if (driver->config->encoder_pulses_per_revolution) 
    {
        tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_DEVIATION, 0); // deactivate during adjustment
    }
    tmc51x0_register_write(driver, TMC51X0_REG_POSITION_CURRENT, new_position);
    tmc51x0_register_write(driver, TMC51X0_REG_POSITION_TARGET, new_position);
    if (driver->config->encoder_pulses_per_revolution)
    {
        tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_POSITION, new_position);
        tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_STATUS, 2);
        tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_DEVIATION, MAX_ENCODER_DEVIATION);
    }
    driver->position_target = new_position;
    driver->position_actual = new_position;
}

static void tmc51x0_calibrate(tmc51x0_driver_t* driver) 
{
    trace("starting calibration\n");
    // make sure motor will not actually move
    trace("setting TOFF=0\n");
    tmc51x0_set_chopper_config(driver, &(tmc51x0_chopconf_t){
        .off_time_and_driver_enable = 0
    });
    
    // fake a fast speed
    trace("setting VMAX=0\n");
    tmc51x0_register_write(driver, TMC51X0_REG_VELOCITY_MAX, 0);
    trace("setting AMAX=0xffff\n");
    tmc51x0_register_write(driver, TMC51X0_REG_ACCELERATION_MAX, 0xFFFF);
    trace("setting RAMPMODE=1\n");
    tmc51x0_set_ramp_mode(driver, TMC51X0_RAMP_MODE_VELOCITY_POS_VMAX);
    
    // let the motion controller do its thing with max speed
    trace("setting VMAX=0xffff\n");
    
    int prio_before = chThdGetPriorityX();
    chThdSetPriority(HIGHPRIO); // block other threads temporarily for this measurement to get accurate timings.
    tmc51x0_register_write(driver, TMC51X0_REG_VELOCITY_MAX, 0xffff);
    
    int32_t pos1, pos2;
    uint32_t start, end, diff;
    
    // wait until we reach the target velocity
    do 
    {
        pos1 = tmc51x0_get_current_position(driver);
        start = gptGetCounterX(&GPTD2);
    } while (!driver->velocity_reached);
    
    // now, measure the time we need to reach 1000 steps
    do 
    {
        pos2 = tmc51x0_get_current_position(driver);
        end = gptGetCounterX(&GPTD2);
    } while (pos2 - pos1 < 1000);
    
    if (end < start) 
    {
        // overflow has occurred
        diff = end + (0xFFFFFFFF - start);
    } 
    else 
    {
        diff = end - start;
    }
    
    // stop fake motor run
    trace("setting VMAX=0\n");
    tmc51x0_register_write(driver, TMC51X0_REG_VELOCITY_MAX, 0);
    chThdSetPriority(prio_before);
    do 
    {
        tmc51x0_get_current_position(driver); // dummy read to update standstill flag伪读取以更新静止标志
    } while (!driver->standstill);
    
    trace("setting POS_CURRENT=0\n");
    tmc51x0_register_write(driver, TMC51X0_REG_POSITION_CURRENT, 0);
    driver->position_actual = 0;
    trace("setting POS_TARGET=0\n");
    tmc51x0_register_write(driver, TMC51X0_REG_POSITION_TARGET, 0);
    driver->position_target = 0;
    
    // calculate calibration factor计算校准系数: (VMAX * dt) / (pos2 - pos1)
    //  (0xffff/(2^24/12000000)) = 46874.28474 (TMC5160)
    //  (0xffff/(2^24/13200000)) = 51561.71322 (TMC5130)
    if (driver->config->type == TMC51X0_TYPE_TMC5160) 
    {
        driver->calibration_factor = (46874.28474f * ((float)diff / 72000000.0f)) / ((float)(pos2 - pos1));
    } 
    else 
    {
        driver->calibration_factor = (51561.71322f * ((float)diff / 72000000.0f)) / ((float)(pos2 - pos1));
    }
    
    dbg("time: %u-%u=%u, pos: %d-%d=%d, calibration_factor = %f\n",
        end, start, diff,
        pos2, pos1, pos2-pos1,
        (double)driver->calibration_factor);
    
    assert(driver->calibration_factor > 0.1f && driver->calibration_factor < 1.9f, "invalid calibration factor");
    if (driver->calibration_factor < 0.1f || driver->calibration_factor > 1.9f) 
    	driver->calibration_factor = 1.0f;
}



/// @brief Set the general configuration of the TMC51X0
/// @param gconf The general configuration to set

// 简介：设置TMC51X0的常规配置
// 参数：gconf要设置的常规配置
static void tmc51x0_set_general_config(tmc51x0_driver_t* driver, tmc51x0_gconf_t* gconf) 
{
    uint32_t reg = (gconf->i_scale_analog << 0)
                   | (gconf->internal_rsense << 1)
                   | (gconf->enable_pwm_mode << 2)
                   | (gconf->encoder_commutation_mode << 3)
                   | (gconf->shaft_inverse << 4)
                   | (gconf->diag0 << 5)
                   | (gconf->small_hysteresis << 14)
                   | (gconf->stop_enable << 15)
                   | (gconf->direct_mode << 16)
                   | (gconf->test_mode << 17);
    tmc51x0_register_write(driver, TMC51X0_REG_GCONF, reg);
}

/// @brief Set the chopper configuration
/// @param chopconf The chopper configuration

// 简介：设置斩波器配置
// 参数：chopconf斩波器配置
static void tmc51x0_set_chopper_config(tmc51x0_driver_t* driver, tmc51x0_chopconf_t* chopconf) 
{
    uint32_t reg = (chopconf->off_time_and_driver_enable << 0)
                   | (chopconf->random_off_time << 13)
                   | (chopconf->chopper_mode << 14)
                   | (chopconf->blank_time_select << 15)
                   | (chopconf->sense_resistor_voltage_based_current_scaling << 17)
                   | (chopconf->high_velocity_fullstep_selection << 18)
                   | (chopconf->high_velocity_chopper_mode << 19)
                   | (chopconf->pwm_sync_clock << 20)
                   | (chopconf->microstep_resolution << 24)
                   | (chopconf->interpolation << 28)
                   | (chopconf->enable_double_edge_step_pulses << 29)
                   | (chopconf->disable_short_to_ground_protection << 30);
    if (chopconf->chopper_mode == 1) 
    {
        reg |= ((chopconf->fast_decay_time & 0x07) << 4)
               | ((chopconf->fast_decay_time & 0x08) << 11) // MSB at bit 11
               | (chopconf->fast_decay_mode << 7)
               | (chopconf->sine_wave_offset << 16);
    } 
    else 
    {
        reg |= (chopconf->hysteresis_start_value << 4)
               | (chopconf->hysteresis_low_value << 7);
    }
    tmc51x0_register_write(driver, TMC51X0_REG_CHOPCONF, reg);
}

/// @brief Sets the ramp mode.
/// @param mode  [0..3] 0: positioning mode (using all A, D and V paramters)
///                     1: velocity mode to positive VMAX (using AMAX acceleration)
///                     2: velocity mode to negative VMAX (using AMAX acceleration)
///                     3: hold mode (velocity remains unchanged, unless stop event occurs)

// 简介：设置渐变模式
// 参数：mode [0..3] 0：定位模式（使用所有A、D和V参数）
// 									 1：速度模式为正VMAX（使用AMAX加速度）
// 									 2：速度模式至负VMAX（使用AMAX加速度）
// 									 3：保持模式（速度保持不变，除非发生停止事件）
static void tmc51x0_set_ramp_mode(tmc51x0_driver_t* driver, tmc51x0_ramp_mode_t mode) 
{
    assert(mode <= 3, "mode out of range");
    tmc51x0_register_write(driver, TMC51X0_REG_RAMP_MODE, mode);
}

static void tmc51x0_set_coolstep_config(tmc51x0_driver_t* driver, tmc51x0_coolconf_t* coolconf) 
{
    uint32_t reg = (coolconf->enable_filtered_mode << 24)
                   | (coolconf->threshold << 16)
                   | (coolconf->minimum_current << 15)
                   | (coolconf->current_down_step_speed << 13)
                   | (coolconf->hysteresis << 8)
                   | (coolconf->current_up_step_width << 5)
                   | (coolconf->minimum << 0);
    tmc51x0_register_write(driver, TMC51X0_REG_COOLCONF, reg);
}

#if USE_DCSTEP
static void tmc51x0_set_dc_ctrl(tmc51x0_driver_t* driver, tmc51x0_dc_ctrl_t* dc_ctrl) 
{
    uint32_t reg = (dc_ctrl->dc_time << 0)
                   | (dc_ctrl->dc_stallguard << 16);
    tmc51x0_register_write(driver, TMC51X0_REG_DCCTRL, reg);
}
#endif

static void tmc51x0_set_pwm_config(tmc51x0_driver_t* driver, tmc51x0_pwmconf_t* pwmconf) 
{
    uint32_t reg = (pwmconf->pwm_auto_scale_limit << 28)
                   | (pwmconf->pwm_regulation_loop_gradient << 24)
                   | (pwmconf->freewheel << 20)
                   | (pwmconf->pwm_autograd << 19)
                   | (pwmconf->pwm_autoscale << 18)
                   | (pwmconf->pwm_frequency_selection << 16)
                   | (pwmconf->pwm_gradient << 8)
                   | (pwmconf->pwm_amplitude_offset << 0);
    if (pwmconf->pwm_autoscale && pwmconf->pwm_autograd) 
    {
        // write without autoscale & autograd bits at first, so that custom pwm_gradient and pwm_amplitude_offset get accepted
        // 在不使用自动缩放的情况下进行写入&首先使用autograd位，以便接受自定义pwm_gradient和pwm_amplitude_offset
        reg &= ~(3 << 18);
        tmc51x0_register_write(driver, TMC51X0_REG_PWMCONF, reg);
        reg |= (3 << 18);
    }
    tmc51x0_register_write(driver, TMC51X0_REG_PWMCONF, reg);
}

/// @brief Get the driver status
/// @return The driver status

// 简介：获取驱动器状态
// 返回：驱动器状态
tmc51x0_drv_status_t tmc51x0_get_driver_status(tmc51x0_driver_t* driver) 
{
    uint32_t reg = tmc51x0_register_read(driver, TMC51X0_REG_DRV_STATUS);
    tmc51x0_drv_status_t drv_status = {0};
    drv_status.standstill = (reg >> 31) & 0x01;
    drv_status.open_load_B = (reg >> 30) & 0x01;
    drv_status.open_load_A = (reg >> 29) & 0x01;
    drv_status.short_to_ground_B = (reg >> 28) & 0x01;
    drv_status.short_to_ground_A = (reg >> 27) & 0x01;
    drv_status.over_temperature_warning = (reg >> 26) & 0x01;
    drv_status.over_temperature_error = (reg >> 25) & 0x01;
    drv_status.stallguard_status = (reg >> 24) & 0x01;
    drv_status.motor_current_actual = (reg >> 16) & 0x1F;
    drv_status.full_step_active = (reg >> 15) & 0x01;
    drv_status.stealthchop_active = (reg >> 14) & 0x01;
    drv_status.short_to_supply_B = (reg >> 13) & 0x01;
    drv_status.short_to_supply_A = (reg >> 12) & 0x01;
    drv_status.stallguard_result = (reg >> 0) & 0x3FF;
    
    if (driver->driver_error != 0
        && (drv_status.open_load_A || drv_status.open_load_B
            || drv_status.short_to_ground_A || drv_status.short_to_ground_B
            || drv_status.over_temperature_warning || drv_status.over_temperature_error
            || drv_status.short_to_supply_A || drv_status.short_to_supply_B)) 
    {
        dbg("DRV_STATUS: standstill=%u, open_load_B=%u, open_load_A=%u, short_to_ground_A=%u, short_to_ground_B=%u, "
            "over_temperature_warning=%u, over_temperature_error=%u, stallguard_status=%u, motor_current_actual=%u,"
            "short_to_supply_A=%u, short_to_supply_B=%u, stealthchop=%u, full_step_active=%u, stallguard_result=%u\n",
            drv_status.standstill,
            drv_status.open_load_B,
            drv_status.open_load_A,
            drv_status.short_to_ground_A,
            drv_status.short_to_ground_B,
            drv_status.over_temperature_warning,
            drv_status.over_temperature_error,
            drv_status.stallguard_status,
            drv_status.motor_current_actual,
            drv_status.short_to_supply_A,
            drv_status.short_to_supply_B,
            drv_status.stealthchop_active,
            drv_status.full_step_active,
            drv_status.stallguard_result);
    }
    return drv_status;
}

bool tmc51x0_encoder_deviation_detected(tmc51x0_driver_t* driver) 
{
    if (!driver || !driver->config) 
    {
        // driver is not initialized yet, that's ok
        // 驱动程序尚未初始化，没关系
        return false;
    }
    uint32_t enc_status = tmc51x0_register_read(driver, TMC51X0_REG_ENCODER_STATUS);
#if STE_DEBUG_ENABLED
    static tmc51x0_driver_t* last_driver = NULL;
    static uint32_t last_enc_status = 0;
    static uint32_t last_standstill = 0;
    if (last_driver != driver || last_enc_status != enc_status || last_standstill != driver->standstill) 
    {
        last_driver = driver;
        last_enc_status = enc_status;
        last_standstill = driver->standstill;
        int32_t enc_pos = (int32_t)tmc51x0_register_read(driver, TMC51X0_REG_ENCODER_POSITION);
        dbg("########### encoder status: %08x, pos=%d vs %d (diff=%d)\n",
            enc_status, enc_pos, driver->position_actual, driver->position_actual - enc_pos);
    }
#endif
    
    if (enc_status & 2) 
    {
        return true;
    } 
    else 
    {
        return false;
    }
}

void tmc51x0_encoder_clear_deviation_warning(tmc51x0_driver_t* driver) 
{
    // note: if motor is not standing still, this might not work as expected, as reading and writing the register takes
    // some time during which the motor position will have already changed again
    // 注释：如果电机没有静止不动，这可能不会像预期的那样工作，因为读取和写入寄存器需要一些时间，在此期间电机位置将再次发生变化
    if (driver->standstill) 
    {
        int32_t pos = tmc51x0_get_current_position(driver);
#if STE_DEBUG_ENABLED
        int32_t enc_pos = (int32_t)tmc51x0_register_read(driver, TMC51X0_REG_ENCODER_POSITION);
        dbg("reset deviation to 0 (pos %d vs %d)\n", enc_pos, pos);
#endif
        tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_POSITION, pos);
    }
    tmc51x0_register_write(driver, TMC51X0_REG_ENCODER_STATUS, 2);

}

tmc51x0_ramp_status_t tmc51x0_get_ramp_status(tmc51x0_driver_t* driver) 
{
    uint32_t reg = tmc51x0_register_read(driver, TMC51X0_REG_RAMP_STATUS);
    tmc51x0_ramp_status_t ramp_status = {0};
    ramp_status.stop_by_stallguard = (reg >> 13) & 0x01;
    ramp_status.second_move = (reg >> 12) & 0x01;
    ramp_status.t_zerowait_active = (reg >> 11) & 0x01;
    ramp_status.vzero = (reg >> 10) & 0x01;
    ramp_status.position_reached = (reg >> 9) & 0x01;
    ramp_status.velocity_reached = (reg >> 8) & 0x01;
    ramp_status.event_position_reached = (reg >> 7) & 0x01;
    ramp_status.event_stop_by_stallguard = (reg >> 6) & 0x01;
    ramp_status.event_stop_r = (reg >> 5) & 0x01;
    ramp_status.event_stop_l = (reg >> 4) & 0x01;
    ramp_status.status_latch_r = (reg >> 3) & 0x01;
    ramp_status.status_latch_l = (reg >> 2) & 0x01;
    ramp_status.status_stop_r = (reg >> 1) & 0x01;
    ramp_status.status_stop_l = (reg >> 0) & 0x01;
    return ramp_status;
}



/// @brief Returns the global status of the TMC51X0
/// @return The global status of the TMC51X0

// 简介：返回TMC51X0的全局状态
// 返回：TMC51X0的全局状态
tmc51x0_gstat_t tmc51x0_get_global_status(tmc51x0_driver_t* driver) 
{
    uint32_t reg = tmc51x0_register_read(driver, TMC51X0_REG_GLOBAL_STATUS);
    tmc51x0_gstat_t gstat;
    gstat.reset = (reg >> 0) & 0x1;
    gstat.drv_err = (reg >> 1) & 0x1;
    gstat.uv_cp = (reg >> 2) & 0x1;
    return gstat;
}

/// @brief Clears the error flags in the global status of the TMC51X0
// 简介：清除TMC51X0全局状态中的错误标志
void tmc51x0_clear_errors(tmc51x0_driver_t* driver) 
{
    tmc51x0_register_write(driver, TMC51X0_REG_GLOBAL_STATUS, 0x07); // ^= 0b111
}

uint32_t tmc51x0_get_tstep_actual(tmc51x0_driver_t* driver) 
{
    uint32_t tstep = tmc51x0_register_read(driver, TMC51X0_REG_TSTEP);
    return tstep;
}

/// @brief Sets motor hold and run currents as well as the hold delay after motor movement
/// @param ihold_irun The i_hold, i_run and i_hold_delay values to set

// 简介：设置电机保持和运行电流以及电机移动后的保持延迟
// 参数：ihold_irun要设置的i_hold、i_run和i_hold_delay值
static void tmc51x0_set_ihold_irun(tmc51x0_driver_t* driver, tmc51x0_ihold_irun_t* ihold_irun) 
{
    uint32_t reg = (ihold_irun->i_hold << 0)
                   | (ihold_irun->i_run << 8)
                   | (ihold_irun->i_hold_delay << 16);
    tmc51x0_register_write(driver, TMC51X0_REG_IHOLD_IRUN, reg);
}

/// @brief Sets the delay time after stand still (stst) of the motor to motor current power down. Time range is about
///        0 to 4 seconds.
/// @param t_powerdown  [0..255] 0: no delay, 1: minimum delay, 2..255: (TPOWERDOWN-1) * 2^18 t_clk

// 简介：设置从电机静止（stst）到电机电流断电的延迟时间。时间范围约为0到4秒。
// 参数：t_powerdown[0..255] 0：无延迟，1：最小延迟，2..255:（TPOWERDOWN-1）*2^18 t_clk
static void tmc51x0_set_t_powerdown(tmc51x0_driver_t* driver, uint32_t t_powerdown) 
{
    assert(t_powerdown <= 0x100, "t_powerdown out of range");
    tmc51x0_register_write(driver, TMC51X0_REG_TPOWERDOWN, t_powerdown);
}

/// @brief Sets upper velocity for StealthChop voltage PWM mode.
/// @details TSTEP >= TPWMTHRS: StealthChop PWM mode is enabled, if configured; DcStep is disabled
/// @param t_pwm_threshold  [0..0xfffff] upper velocity for StealthChop voltage PWM mode

// 简介：设置StealthChop电压PWM模式的上限速度。
// 详情：TSTEP>=TPWMTHRS：如果配置，则启用StealthChop PWM模式；DcStep已禁用
// 参数：t_pwm_threshold[0..0xfffff]StealthChop电压pwm模式的上速度
static void tmc51x0_set_t_pwm_threshold(tmc51x0_driver_t* driver, uint32_t t_pwm_threshold) 
{
    assert(t_pwm_threshold <= 0x100000, "t_pwm_threshold out of range");
    tmc51x0_register_write(driver, TMC51X0_REG_TPWM_THRS, t_pwm_threshold);
}

/// @brief Sets the lower velocity for switching on smart energy coolStep and stallGuard feature.
/// @details Set this parameter to disable CoolStep at low speeds, where it cannot work reliably. The stop on stall
///          function (enable with sg_stop when using internal motion controller) and the stall output signal become
///          enabled when exceeding this velocity. In non-DcStep mode, it becomes disabled again once the velocity falls
///          below this threshold.
///          TCOOLTHRS >= TSTEP >= THIGH: CoolStep is enabled, if configured; StealthChop voltage PWM mode is disabled
///          TCOOLTHRS >= TSTEP: Stop on stall and stall output signal is enabled, if configured
/// @param t_coolthrs  [0..0xfffff] lower velocity for switching on smart energy coolStep and stallGuard feature

// 简介：设置打开智能能源coolStep和stallGuard功能的较低速度。
// 详情：设置此参数可在低速时禁用CoolStep，因为它无法可靠工作。失速停止功能（使用内部运动控制器时使用sg_stop启用）
// 			 和失速输出信号在超过该速度时启用。在非DcStep模式下，一旦速度降至该阈值以下，它将再次禁用。
// 			 TCOOLTHRS>=TSTEP>=THIGH：如果已配置，则启用CoolStep；StealthChop电压PWM模式被禁用
// 			 TCOOLTHRS>=TSTEP：如果配置，失速停止并启用失速输出信号
// 参数：t_coolthrs[0..0xfffff]降低开启智能能源coolStep和stallGuard功能的速度
static void tmc51x0_set_t_cool_threshold(tmc51x0_driver_t* driver, uint32_t t_coolthrs) 
{
    assert(t_coolthrs <= 0x100000, "t_coolthrs out of range");
    tmc51x0_register_write(driver, TMC51X0_REG_TCOOL_THRS, t_coolthrs);
}

/// @brief This velocity setting allows velocity dependent switching into a different chopper mode and fullstepping to
///        maximize torque.
/// @details The stall detection feature becomes switched off for 2-3 electrical periods whenever passing THIGH
///          threshold to compensate for the effect of switching modes.
//           TSTEP <= THIGH:
//           * CoolStep is disabled (motors runs with normal current scale)
//           * StealthChop voltage PWM mode is disabled
//           * If vhighchm is set, the chopper switches to chm=1 with TFD=0 (constant off time with slowdecay, only)
//           * chopSync2 is switched off (SYNC=0)
//           * If vhighfs is set, the motor operates in fullstep mode and the stall detection becomes switched over to
//             DcStep stall detection.

// 简介：此速度设置允许根据速度切换到不同的斩波器模式并全步进以最大化扭矩。
// 详情：只要超过THIGH阈值，失速检测功能就会关闭2-3个电周期，以补偿切换模式的影响。
// 			 TSTEP <= THIGH:
// 			 * CoolStep被禁用（电机在正常电流范围内运行）
// 			 * StealthChop电压PWM模式被禁用
// 			 * 如果设置了vhighchm，斩波器切换到chm=1，TFD=0（仅限慢衰减的恒定关断时间）
// 			 * chopSync2关闭（SYNC=0）
// 			 * 如果设置了vhighfs，则电机在全步进模式下运行，并且失速检测切换到DcStep失速检测。
static void tmc51x0_set_t_high(tmc51x0_driver_t* driver, uint32_t t_high) 
{
    assert(t_high <= 0x100000, "t_high out of range");
    tmc51x0_register_write(driver, TMC51X0_REG_THIGH, t_high);
}
