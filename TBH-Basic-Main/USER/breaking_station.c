#include <stdlib.h>

#include "ste.h"
#include "breaking_station.h"
#include "units.h"

tmc51x0_driver_t BREAKING_STATION = {0};

static virtual_timer_t* breaking_station_power_off_timer = NULL;
static uint32_t cooldown_start_time = 0;
static uint32_t deviation_start_time = 0;

typedef enum 
{
    BREAKING_STATION_INITIAL = 0,
    BREAKING_STATION_INITIALIZE,
    BREAKING_STATION_DISABLED,
    BREAKING_STATION_READY,
    BREAKING_STATION_START,
    BREAKING_STATION_HOMING_FIND_HOME,
    BREAKING_STATION_HOMING_LEAVE_HOME,
    BREAKING_STATION_HOMING_LEAVE_HOME_STOP,
    BREAKING_STATION_HOMING_AWAIT_HOME_STOP,
    BREAKING_STATION_IDLE,
    BREAKING_STATION_START_MOVEMENT,
    BREAKING_STATION_BUSY,
    BREAKING_STATION_STOP_MOVEMENT,
    BREAKING_STATION_STOPPING
} breaking_station_state_t;

typedef enum 
{
    BREAKING_STATION_FLAG_NONE = 0,
    BREAKING_STATION_FLAG_MEASURE_DIAMETER = (1 << 0),
    BREAKING_STATION_FLAG_MEASURE_FORCE = (1 << 1),
    BREAKING_STATION_FLAG_SAVE_AS_MAX_POS = (1 << 2),
    BREAKING_STATION_FLAG_AUTO_INIT = (1 << 3),
    BREAKING_STATION_FLAG_TARGET_AFTER_HOME = (1 << 4),
    BREAKING_STATION_FLAG_HOME_AFTER_TARGET = (1 << 5),
    BREAKING_STATION_FLAG_SLOW = (1 << 6),
    BREAKING_STATION_FLAG_CONTACT = (1 << 7),
    BREAKING_STATION_FLAG_BREAK = (1 << 8),
    BREAKING_STATION_FLAG_MOVING_TO_SAFE_POSITION = (1 << 9),
    BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE = (1 << 10),
    BREAKING_STATION_FLAG_ERROR_NO_TABLET = (1 << 11),
} breaking_station_flags_t;

static breaking_station_state_t breaking_station_state = BREAKING_STATION_INITIAL;

static void ensure_sane_force_distance_curve(uint32_t breaking_force);

void breaking_station_init(void) 
{
    tmc51x0_init(&BREAKING_STATION, &breaking_station_config);
}

void breaking_station_start(void) 
{
    tmc51x0_start(&BREAKING_STATION);
}

void breaking_station_power_on(void) 
{
    ioportmask_t line_before = palReadLineLatch(LINE_STENA1);
    palClearLine(LINE_STENA1); // enable breaking station motor
    if (line_before != palReadLineLatch(LINE_STENA1)) dbg("breaking station power on\n");
}

void breaking_station_power_off(void) 
{
    ioportmask_t line_before = palReadLineLatch(LINE_STENA1);
    palSetLine(LINE_STENA1); // disable breaking station motor
    if (BREAKING_STATION.config) 
    {
        /*tmc51x0_drv_status_t driver_status = tmc51x0_get_driver_status(&MAGAZINE);
        dbg("standstill: %d, temp_warn: %d, temp_err: %d, sg_status: %d, motor_current: %d, fullstep: %u, sg_result: %d\n",
            MAGAZINE.standstill, driver_status.over_temperature_warning, driver_status.over_temperature_error,
            driver_status.stallguard_status, driver_status.motor_current_actual, driver_status.full_step_active,
            driver_status.stallguard_result);*/
        if (BREAKING_STATION.driver_error) 
        {
            tmc51x0_gstat_t gstat = tmc51x0_get_global_status(&BREAKING_STATION);
            (void) gstat;
        }
    }
    
    if (line_before != palReadLineLatch(LINE_STENA1)) 
    	dbg("breaking station power off\n");
    breaking_station_power_off_timer = NULL;
}

static int32_t get_position_max(void) 
{
    int32_t position_max = state.breaking_station.position_max;
    if (state.breaking_station.position_max == 0) 
    {
        position_max = BREAKING_STATION_TOTAL_LENGTH;
    }
    return position_max;
}

void breaking_station_go_to_position(int32_t position) 
{
    if (position == state.breaking_station.position_actual) 
    {
        dbg("breaking station: already at position %d/%d\n", position, state.breaking_station.position_actual);
        return;
    }
    // during initial homing and determining position_max, negative positions are allowed
    if (breaking_station_state >= BREAKING_STATION_IDLE && state.breaking_station.position_max != 0) 
    {
        if (position < 0 || position > get_position_max()) 
        {
            dbg("breaking station: invalid position %d\n", position);
            return;
        }
    }
    
    if (breaking_station_power_off_timer) 
    {
        timer_cancel(breaking_station_power_off_timer);
        breaking_station_power_off_timer = NULL;
    }
    breaking_station_power_on();
#if STE_DEBUG_ENABLED
    // in debug mode, this has a logging side effect
    tmc51x0_encoder_deviation_detected(&BREAKING_STATION);
#endif
    tmc51x0_set_target_position(&BREAKING_STATION, position);
}

/// @brief This function calculates the position where max position where there should not be a tablet yet, i.e. where it is
///        safe to go to fast. Meant to be used to go very slightly further than that, notice the point the position that
///        this function calculated is passed, set the slow speed and then set the final target.
/// @return The farthest position where the tablet safely should NOT be, in steps.
static int32_t get_slow_down_pos(void) 
{
    int32_t slow_down_pos = get_position_max();
    int32_t prebrake_distance = state.breaking_station.prebrake_distance;
    assert_or_fix(prebrake_distance >= 500, "prebrake distance too small", prebrake_distance = 500);
    slow_down_pos -= convert_to(UNIT_BREAKING_POSITION_STEPS,
                                state.breaking_station.nominal_diameter + prebrake_distance);
    
    dbg("breaking station: slow down pos: %d of %d steps / %d before end (%.2f of %.2f mm / %.2f mm)\n",
        slow_down_pos, get_position_max(), get_position_max() - slow_down_pos,
        convertf_to(UNIT_MM, convertf_from(UNIT_BREAKING_POSITION_STEPS, slow_down_pos)),
        convertf_to(UNIT_MM, convertf_from(UNIT_BREAKING_POSITION_STEPS, get_position_max())),
        convertf_to(UNIT_MM, convertf_from(UNIT_BREAKING_POSITION_STEPS, get_position_max() - slow_down_pos)));
    
    return slow_down_pos;
}

#if STE_DEBUG_ENABLED
static char* breaking_station_state_name(breaking_station_state_t bstate) 
{
    switch (bstate) 
    {
        case BREAKING_STATION_INITIAL: return "INITIAL";
        case BREAKING_STATION_INITIALIZE: return "INITIALIZE";
        case BREAKING_STATION_DISABLED: return "DISABLED";
        case BREAKING_STATION_READY: return "READY";
        case BREAKING_STATION_START: return "START";
        case BREAKING_STATION_HOMING_FIND_HOME: return "HOMING_FIND_HOME";
        case BREAKING_STATION_HOMING_LEAVE_HOME: return "HOMING_LEAVE_HOME";
        case BREAKING_STATION_HOMING_LEAVE_HOME_STOP: return "HOMING_LEAVE_HOME_STOP";
        case BREAKING_STATION_HOMING_AWAIT_HOME_STOP: return "HOMING_AWAIT_HOME_STOP";
        case BREAKING_STATION_IDLE: return "IDLE";
        case BREAKING_STATION_START_MOVEMENT: return "START_MOVEMENT";
        case BREAKING_STATION_BUSY: return "BUSY";
        case BREAKING_STATION_STOP_MOVEMENT: return "STOP_MOVEMENT";
        case BREAKING_STATION_STOPPING: return "STOPPING";
        default: return "UNKNOWN";
    }
}
#endif

// TODO: replace this crude helper
static uint32_t block_until = 0;
#define DELAY_FSM_FOR_MS(ms)                \
if (block_until == 0) 						\
{                     						\
    block_until = ticks_ms() + ms;          \
    continue;                               \
} 											\
else 										\
{                                    		\
    if (ticks_ms() < block_until) continue; \
    block_until = 0;                        \
}

// TODO:
// * cache settings so they are constant during measurement?

void breaking_station_fsm(void) 
{
    static tmc51x0_driver_t *driver = &BREAKING_STATION;
    static char *name = "breaking";
    static breaking_station_flags_t flags = 0;
    static int32_t postponed_target = 0;
    static int32_t contact_position = 0;
    static uint32_t breaking_force = 0;
    static int32_t slow_down_pos = 0;
    
    breaking_station_state_t state_at_start = breaking_station_state;
    breaking_station_state_t previous_state = state_at_start;
    /* This FSM is built as a loop that keeps executing while state changes occur, in order to react as quickly as
     * possible (e.g. state transition to a stop state due to some error). */
    do 
    {
        state_at_start = breaking_station_state;
        
        /******************************************************************************
         ******************************************************************************
         * Safety checks (checked every loop iteration):
         ******************************************************************************
         *****************************************************************************/
        bool movement_prohibited = false;
        bool breaking_station_active_or_idle = false;
        if (in_range(breaking_station_state,
                     BREAKING_STATION_DISABLED + 1,BREAKING_STATION_STOP_MOVEMENT - 1)) 
        {
            breaking_station_active_or_idle = true;
        }
        
        // cannot move safely if initiators are disconnected, short circuited or if the cover is open
        bool initiators_disconnected = (state.cover_closed < 0 || state.breaking_home < 0 || state.magazine_home < 0)?
            true : false;
        
        // cannot move safely if other motors are not idle. In theory, we should be able to coexist with the thickness
        // gauge, but this has not been tested yet.
        bool other_motors_idle = (state.magazine.is_idle && state.thickness_gauge.is_idle)? true : false;
        
        // check if the force sensor is giving valid readings yet, as we use this to detect excess force (and we need it
        // for useful breaking station measurements anyway)
        bool force_not_ready = false;
        if (state.force_raw == 0 || !(state.system_conditions & SYSTEM_CONDITION_FORCE_READING_VALID)) 
        {
            force_not_ready = true;
        }
        
        // Use the force sensor to detect if a too high force is applied. If so, stop the motor and wait for the
        // operator to clear the condition.
        bool excess_force_block = false;
        if ((!force_not_ready && state.force >= state.breaking_station.force_threshold_upper)
            || (flags & BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE)) 
        {
            if (breaking_station_active_or_idle) 
            {
                if (!(flags & BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE)) 
                {
                    flags |= BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE;
                    if (state.force > breaking_force) breaking_force = state.force;
                }
                if (!(flags & BREAKING_STATION_FLAG_MOVING_TO_SAFE_POSITION)) 
                {
                    if (!driver->standstill) 
                    {
                        // movement is in progress, stop it (exception: moving to safe position)
                        breaking_station_state = BREAKING_STATION_STOP_MOVEMENT;
                    } 
                    else 
                    {
                        // go directly to disabling state
                        breaking_station_state = BREAKING_STATION_INITIALIZE;
                    }
                } 
                else 
                {
                    // flag is set, which means we are currently moving into a safe position, and the STOP_MOVEMENT
                    // state will take care of going to INITIALIZE next
                }
                
            } 
            else 
            {
                excess_force_block = true;
            }
        }
        
        // This motor has an encoder, so we use it to detect step loss and stop the motor if necessary. The operator
        // must then clear the condition before we can continue.
        bool encoder_deviation_detected = false;
        if (other_motors_idle) 
        {
            // if the other motors (resp. their FSMs), we omit this check, otherwise it needlessly switches the
            // currently active motor driver back and forth
            if (tmc51x0_encoder_deviation_detected(driver)) 
            {
                if (!deviation_start_time) 
                {
                    deviation_start_time = ticks_ms();
                    tmc51x0_encoder_clear_deviation_warning(driver);
                    dbg("encoder deviation detected, starting timer\n");
                } 
                else 
                {
                    /* We want to be forgiving for very brief transients. During the first initial homing, the driver is
                     * still optimizing its PWM auto settings, during which we may see larger deviances, so we want to be
                     * even more forgiving, as long as they correct themselves in due time (i.e. no actual step loss has
                     * occurred). */
                    uint32_t leniency_ms = 10;
                    if (in_range(breaking_station_state,
                                 BREAKING_STATION_START + 1, BREAKING_STATION_IDLE - 1)) 
                    {
                        leniency_ms = 1500; // ~the time it takes to leave home when starting at home, i.e. PWM adjust time
                    }
                    
                    if (deviation_start_time + leniency_ms < ticks_ms()) 
                    {
                        if (in_range(breaking_station_state,
                                     BREAKING_STATION_START + 1, BREAKING_STATION_STOP_MOVEMENT - 1)) 
                        {
                            // movement is in progress, stop it
                            dbg("deviation timer expired\n");
                            breaking_station_state = BREAKING_STATION_STOP_MOVEMENT;
                        } 
                        else if (breaking_station_state <= BREAKING_STATION_START) 
                        {
                            encoder_deviation_detected = true;
                        }
                    }
                }
            } 
            else 
            {
                if (deviation_start_time) 
                {
                    dbg("encoder deviation is gone\n");
                    deviation_start_time = 0;
                }
            }
        }
        
        // not all above checks immediately lead to a disabled state (e.g. upon excess force detection, we want to move
        // to a safe position first), but these do:
        if (initiators_disconnected || event_pending(EVENT_BREAKING_STATION_STOP) || driver->driver_error
            || excess_force_block || encoder_deviation_detected
            || !state.cover_closed || !other_motors_idle) 
        {
            movement_prohibited = true;
        }
        
        if (!movement_prohibited && force_not_ready) 
        {
            if (event_pending_any_of(EVENT_BREAKING_STATION_GO_HOME | EVENT_MEASURE_DIAMETER | EVENT_MEASURE_FORCE)
                || (breaking_station_state > BREAKING_STATION_INITIAL // avoid initial, subscriptions not set up yet
                    && rpc_requested_any_of(RPC_BREAKING_STATION_GO_HOME | RPC_BREAKING_STATION_FIND_MAX_POSITION))
                || (flags & BREAKING_STATION_FLAG_AUTO_INIT)) 
            {
                // likely scenario: the thickness gauge was active earlier, and thus the current measurement channel is
                // not force, which we need here -> switch to force measurement channel
                // TODO: think about whether it might be nicer to have the status available as a state (or a system
                // condition flag) instead of having to check the last RPC call result (which currently depends on the
                // fact that the force channel is the initial default, and thus it's irrelevant whether the RPC has
                // already been called or not, since the integer value of MEASUREMENT_CHANNEL_FORCE is 0)
                if (rpc_result(RPC_ENABLE_MEASUREMENT_CHANNEL) != MEASUREMENT_CHANNEL_FORCE) 
                {
                    log("switching to force measurement channel\n");
                    // tell platform to switch to force measurement channel
                    rpc_call_arg_await(RPC_ENABLE_MEASUREMENT_CHANNEL, MEASUREMENT_CHANNEL_FORCE);
                    // after this it will still take some time until the ADC re-init is actually complete
                }
            }
            movement_prohibited = true;
        }
        
        static bool mov_proh_bef = false;
        
        if (movement_prohibited != mov_proh_bef) 
        {
            if (movement_prohibited) 
            	log("initiators_disconnected=%d, event_pending(EVENT_BREAKING_STATION_STOP)=%d, driver->driver_error=%d, "
                "excess_force_block=%d, encoder_deviation_detected=%d, !state.cover_closed=%d, force_not_ready=%d, "
                "!other_motors_idle=%d, movement_prohibited=%d\n",
                initiators_disconnected, event_pending(EVENT_BREAKING_STATION_STOP), driver->driver_error,
                excess_force_block, encoder_deviation_detected, !state.cover_closed, force_not_ready,
                !other_motors_idle, movement_prohibited);
            mov_proh_bef = movement_prohibited;
        }
        
        // taking everything into account, are we allowed to move?
        if (movement_prohibited) 
        {
            // No. Disable motor unless idle anyway.
            if (breaking_station_active_or_idle
                && (breaking_station_state != BREAKING_STATION_IDLE || event_pending(EVENT_BREAKING_STATION_STOP))) 
            {
                breaking_station_state = BREAKING_STATION_INITIALIZE;
            }
            
            // For some error reasons, we want some manual action before we continue (go home command or
            // opening and closing cover) (not needed if in idle anyway, there, we only need to do nothing):
            if (breaking_station_state != BREAKING_STATION_IDLE) 
            {
                if (initiators_disconnected || event_pending(EVENT_BREAKING_STATION_STOP) || driver->driver_error
                    || excess_force_block || encoder_deviation_detected) 
                {
                    if (flags & BREAKING_STATION_FLAG_AUTO_INIT) 
                    {
                        flags &= ~BREAKING_STATION_FLAG_AUTO_INIT;
                        dbg("auto init flag cleared\n");
                    }
                } 
                else 
                {
                    // Otherwise, we want to start things automatically once everything is fine again.
                    if (!(flags & BREAKING_STATION_FLAG_AUTO_INIT)) 
                    {
                        flags |= BREAKING_STATION_FLAG_AUTO_INIT;
                        dbg("auto init flag set\n");
                    }
                }
            }
            event_clear(EVENT_BREAKING_STATION_STOP);
            
            // TODO: refactor this to use state_changed/state_accept without influencing the other state machines
            bool cover_closed_after_open = false;
            static int cover_state_last = -1234; // dummy invalid value
            if (cover_state_last == -1234) 
            {
                // initial setting
                cover_state_last = state.cover_closed;
            }
            if (cover_state_last != state.cover_closed) 
            {
                if (state.cover_closed == NAMUR_ACTIVE) 
                {
                    cover_closed_after_open = true;
                }
                cover_state_last = state.cover_closed;
            }
            
            // while we are in this sorry state, keep ignoring requests..
            state_accept(STATE_BREAKING_STATION_POSITION_TARGET);
            if (event_pending(EVENT_BREAKING_STATION_GO_HOME) || cover_closed_after_open) 
            {
                // ..but allow them to clear a previous excess force condition. Go home is an exception, as that
                // indeed leads to initiating a new homing. The alternative is opening and closing the cover (see below).
                dbg("clearing error conditions\n");
                
                if (driver->driver_error) 
                {
                    tmc51x0_clear_errors(driver);
                    tmc51x0_get_global_status(driver);
                }
                
                flags &= ~BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE;
                excess_force_block = false;
                
                tmc51x0_encoder_clear_deviation_warning(driver);
                encoder_deviation_detected = false;
                
                if (breaking_station_state != BREAKING_STATION_IDLE && !(flags & BREAKING_STATION_FLAG_AUTO_INIT)) 
                {
                    flags |= BREAKING_STATION_FLAG_AUTO_INIT;
                    dbg("auto init flag set\n");
                }
            }
            
            if ((excess_force_block || encoder_deviation_detected) && state.breaking_station.position_max > 0) 
            {
                // the jaw might be distorted now, better recalibrate
                state_change(STATE_BREAKING_STATION_POSITION_MAX, 0);
            }
            
            // once the error conditions clear up, do not start immediately, wait a bit for them to be really stable
            if (breaking_station_state > BREAKING_STATION_INITIAL) 
            	cooldown_start_time = ticks_ms();
        }
        
        // set the system wide error conditions, e.g. for the blinking thread
        if (force_not_ready || driver->driver_error || excess_force_block || encoder_deviation_detected) 
        {
            if (!(state.error_conditions & ERROR_CONDITION_MOTOR_BREAKING_STATION)) 
            {
                chSysLock();
                state_change(STATE_ERROR_CONDITIONS,
                             state.error_conditions | ERROR_CONDITION_MOTOR_BREAKING_STATION);
                chSysUnlock();
            }
        } 
        else 
        {
            if (state.error_conditions & ERROR_CONDITION_MOTOR_BREAKING_STATION) 
            {
                chSysLock();
                state_change(STATE_ERROR_CONDITIONS,
                             state.error_conditions & ~ERROR_CONDITION_MOTOR_BREAKING_STATION);
                chSysUnlock();
            }
        }
        
        if (breaking_station_state != previous_state) 
        {
            if (breaking_station_state > BREAKING_STATION_START
                && breaking_station_state != BREAKING_STATION_STOP_MOVEMENT) 
            {
                dbg("%s: state change %s (%d) -> %s (%d)\n",
                    name, breaking_station_state_name(previous_state), previous_state,
                    breaking_station_state_name(breaking_station_state), breaking_station_state);
                /* This will update position & standstill flag in the driver. Normally, this gets updated in the FSM thread,
                 * but since this is a loop repeat (the state changed, and we want to let the new state run as soon as
                 * possible to keep latency low), we're doing it here to have the luxury of a position that's as current
                 * as possible. */
                tmc51x0_get_current_position(driver);
                previous_state = breaking_station_state;
            }
        }
        
        
        /******************************************************************************
         ******************************************************************************
         * The actual state handling - 实际状态处理
         ******************************************************************************
         *****************************************************************************/
        if (breaking_station_state == BREAKING_STATION_INITIAL) 													// 破碎站初始化
        {
            // this will be the case only when the device first initializes
            // 只有当设备首次初始化时才会出现这种情况
            if (!movement_prohibited) 											// 如果是非禁止移动的
            	flags |= BREAKING_STATION_FLAG_AUTO_INIT;
            
            // 事件订阅
            event_subscribe(EVENT_BREAKING_STATION_GO_HOME | EVENT_MEASURE_DIAMETER | EVENT_MEASURE_FORCE | EVENT_BREAKING_STATION_STOP);
            
            // 订阅RPC ID。调用RPC时将通知线程（如果正在睡眠则会被唤醒）
            rpc_subscribe(RPC_BREAKING_STATION_GO_HOME | RPC_BREAKING_STATION_FIND_MAX_POSITION);
            
            // 设置破碎站状态 = BREAKING_STATION_INITIALIZE
            breaking_station_state = BREAKING_STATION_INITIALIZE;
        } 
        else if (breaking_station_state == BREAKING_STATION_INITIALIZE) 									// ？
        {
            // also used to stop motor & bring into safe state (i.e. freewheeling) if above conditions detected
            // 也用于停止电机&如果检测到上述情况，则进入安全状态（即空转）
            if (previous_state == BREAKING_STATION_INITIAL) 					// 前一个状态是BREAKING_STATION_INITIAL？
            {
                dbg("init breaking\n");
                breaking_station_init();
                log("init breaking done\n");
            } 
            else 
            {
                int32_t pos = driver->position_actual;
                breaking_station_init();
                state_change(STATE_BREAKING_STATION_LATCH_FORCE, 0);
                palClearLine(BREAKING_STATION.config->latch_line); // does not exist on PCB rev 1.02
                state_change(STATE_BREAKING_STATION_LATCH_ACTIVE, false);
                log("%s: emergency stop at %d (excess force: %d, flags: %u, cover_closed: %d, breaking_home: %d, breaking.is_idle: "
                    "%d, magazine.is_idle: %d, thickness.is_idle: %d, EV_BS_STOP: %u, !force_ready: %u)\n",
                    name, pos, flags & BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE, flags,
                    state.cover_closed, state.breaking_home, state.breaking_station.is_idle, state.magazine.is_idle,
                    state.thickness_gauge.is_idle, event_pending(EVENT_BREAKING_STATION_STOP), force_not_ready);
            }
            
            breaking_station_state = BREAKING_STATION_DISABLED;
            
        } 
        else if (breaking_station_state == BREAKING_STATION_DISABLED) 										// 破碎站禁用
        {
            // cooldown time is to make sure the !movement_prohibited is stable before acting on it
            if (!movement_prohibited && (cooldown_start_time == 0 || cooldown_start_time + 500 < ticks_ms())) 
            {
                dbg("movement not prohibited, going to ready\n");
                log("%s: now ready @ pos %d (state: %d, cover_closed: %d, breaking_home: %d, breaking.is_idle: "
                    "%d, magazine.is_idle: %d, thickness.is_idle: %d, EV_BS_STOP: %u, !force_ready: %u)\n",
                    name, driver->position_actual, breaking_station_state, state.cover_closed,
                    state.breaking_home, state.breaking_station.is_idle, state.magazine.is_idle,
                    state.thickness_gauge.is_idle, event_pending(EVENT_BREAKING_STATION_STOP), force_not_ready);
                
                breaking_station_state = BREAKING_STATION_READY;
            } 
            else 
            {
                if (!state.breaking_station.is_idle) 
                {
                    state_change(STATE_BREAKING_STATION_IS_IDLE, true);
                    dbg("%s: going idle\n", name);
                }
            }
            
        } 
        else if (breaking_station_state == BREAKING_STATION_READY) 												// 破碎站就绪
        {
            if (movement_prohibited) 
            {
                dbg("movement prohibited, returning to disabled\n");
                
                breaking_station_state = BREAKING_STATION_DISABLED;
                
            } 
            else if (!movement_prohibited
                       && (
                           (flags & BREAKING_STATION_FLAG_AUTO_INIT)
                           || state_changed(STATE_BREAKING_STATION_POSITION_TARGET)
                           || event_pending(EVENT_BREAKING_STATION_GO_HOME)
                       )) 
            {
                dbg("auto_init: %u, state_changed: %u, event_pending: %u\n",
                    flags & BREAKING_STATION_FLAG_AUTO_INIT, state_changed(STATE_BREAKING_STATION_POSITION_TARGET),
                    event_pending(EVENT_BREAKING_STATION_GO_HOME));
                
                if (event_pending(EVENT_BREAKING_STATION_GO_HOME) || (flags & BREAKING_STATION_FLAG_AUTO_INIT)) 
                {
                    // start the homing process
                    event_clear(EVENT_BREAKING_STATION_GO_HOME);
                    if (flags & BREAKING_STATION_FLAG_AUTO_INIT) 
                    {
                        flags &= ~BREAKING_STATION_FLAG_AUTO_INIT;
                    }
                } 
                else if (state_changed(STATE_BREAKING_STATION_POSITION_TARGET)) 
                {
                    state_accept(STATE_BREAKING_STATION_POSITION_TARGET);
                    flags |= BREAKING_STATION_FLAG_TARGET_AFTER_HOME;
                    postponed_target = (state.breaking_station.position_target);
                }
                
                breaking_station_state = BREAKING_STATION_START;
            } 
            else 
            {
                if (!state.breaking_station.is_idle) 
                {
                    state_change(STATE_BREAKING_STATION_IS_IDLE, true);
                    dbg("%s: going idle\n", name);
                }
            }
            
        } 
        else if (breaking_station_state == BREAKING_STATION_START) 												// 破碎站启动
        {
            state_change(STATE_BREAKING_STATION_IS_IDLE, false);
            state_change(STATE_BREAKING_STATION_LATCH_ACTIVE, false);
            if (BREAKING_STATION.config->latch_line)
                palClearLine(BREAKING_STATION.config->latch_line); // does not exist on PCB rev 1.02
            state_change(STATE_BREAKING_STATION_LATCH_FORCE, 0);
            state_change(STATE_BREAKING_STATION_OFFSET_CALIBRATION_ACTIVE, false);
            
            
            dbg("start %s\n", name);
            // if we're here due to RPC find max pos, we might be started already
            if (BREAKING_STATION.state != TMC51X0_STATE_IDLE) breaking_station_start();
            tmc51x0_set_acceleration_max(&BREAKING_STATION, 0x3FFF); // soft acceleration for first start
            // avoid waiting here, especially for more than 100 ms, or the driver sometimes goes into a mysterious error
            log("start %s done\n", name);
            
            if (state.breaking_home == NAMUR_INACTIVE) 
            {
                log("set %s target -%d, home: %d\n", name, BREAKING_STATION_TOTAL_LENGTH, state.breaking_home);
                tmc51x0_set_velocity_max(&BREAKING_STATION,
                                         convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                    convertf_from(UNIT_MM_S, 3.0)));
                breaking_station_go_to_position(-1 * BREAKING_STATION_TOTAL_LENGTH);
                
                breaking_station_state = BREAKING_STATION_HOMING_FIND_HOME;
                
            } 
            else if (state.breaking_home == NAMUR_ACTIVE) 
            {
                // need to leave home so home position is accurate and driver motor has time to calibrate PWM settings
                // (which happens automatically, but only works well when moving slowly, which homing is)
                log("leaving home first, set %s target %d, home: %d\n", name, BREAKING_STATION_TOTAL_LENGTH, state.breaking_home);
                tmc51x0_set_velocity_max(&BREAKING_STATION,
                                         convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                    convertf_from(UNIT_MM_S, 3.0)));
                breaking_station_go_to_position(BREAKING_STATION_TOTAL_LENGTH);
                
                breaking_station_state = BREAKING_STATION_HOMING_LEAVE_HOME;
            }
            
        } 
        else if (breaking_station_state == BREAKING_STATION_HOMING_LEAVE_HOME) 						// 破碎站归航离开家
        {
            if (state.breaking_home == NAMUR_INACTIVE) 
            {
                DELAY_FSM_FOR_MS(5);
                log("left home, stopping\n", name);
                tmc51x0_stop(&BREAKING_STATION);
                
                breaking_station_state = BREAKING_STATION_HOMING_LEAVE_HOME_STOP;
                
            }
        } 
        else if (breaking_station_state == BREAKING_STATION_HOMING_LEAVE_HOME_STOP) 			// 破碎站归航离开终点
        {
            if (state.breaking_home == NAMUR_INACTIVE && BREAKING_STATION.standstill) 
            {
                DELAY_FSM_FOR_MS(130); // minimum time for auto PWM tuning to work reliably
                
                int32_t pos = tmc51x0_get_current_position(driver);
                tmc51x0_set_target_position(&BREAKING_STATION, pos);
                dbg("re-set target pos\n");
                log("left home, set %s target -%d, home: %d, standstill: %u\n",
                    name, BREAKING_STATION_TOTAL_LENGTH, state.breaking_home, BREAKING_STATION.standstill);
                tmc51x0_set_velocity_max(&BREAKING_STATION,
                                         convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                    convertf_from(UNIT_MM_S, 3.0)));
                breaking_station_go_to_position(-1 * BREAKING_STATION_TOTAL_LENGTH);
                
                breaking_station_state = BREAKING_STATION_HOMING_FIND_HOME;
                
            }
        } 
        else if (breaking_station_state == BREAKING_STATION_HOMING_FIND_HOME) 						// 破碎站归航到家
        {
            if (state.breaking_home == NAMUR_ACTIVE) 
            {
                /* This offset has been determined to be far enough back as to be as flush in the containing ring of the
                 * breaking bracket, but not yet influence the force result. */
                // TODO: verify if and how much this deviates between individual devices
                breaking_station_go_to_position(state.breaking_station.position_actual - BREAKING_STATION_HOME_OFFSET);
                log("%s: homing found at %d, moving to %d\n",
                    name, state.breaking_station.position_actual,
                    state.breaking_station.position_actual - BREAKING_STATION_HOME_OFFSET);
                
                breaking_station_state = BREAKING_STATION_HOMING_AWAIT_HOME_STOP;
            }
        } 
        else if (breaking_station_state == BREAKING_STATION_HOMING_AWAIT_HOME_STOP) 			// 破碎站归航等待进站
        {
            tmc51x0_drv_status_t driver_status = tmc51x0_get_driver_status(driver);
            if (driver_status.standstill && BREAKING_STATION.position_reached) 
            {
                DELAY_FSM_FOR_MS(130); // minimum time for auto PWM tuning to work reliably
                // yes, maybe that has already been done above, but maybe not, and there's no harm in doing it twice
                
                log("%s: homing end position of %d reached\n", name, state.breaking_station.position_actual);
                tmc51x0_adjust_nominal_position(driver, 0);
                state_change(STATE_BREAKING_STATION_POSITION_TARGET, 0);
                state_change(STATE_BREAKING_STATION_POSITION_ACTUAL, 0);
                state_accept(STATE_BREAKING_STATION_POSITION_TARGET);
                // restore acceleration
                tmc51x0_set_acceleration_max(&BREAKING_STATION, BREAKING_STATION.config->max_acceleration);
                
                // need to measure how far we can go?
                if (state.breaking_station.position_max == 0) 
                {
                    flags |= BREAKING_STATION_FLAG_SAVE_AS_MAX_POS;
                    event_emit_self(EVENT_MEASURE_DIAMETER);
                }
                
                breaking_station_state = BREAKING_STATION_IDLE;
            }
        } 
        else if (breaking_station_state == BREAKING_STATION_IDLE) 												// 破碎站空闲
        {
            if (!movement_prohibited) 
            {
                int32_t new_target = state.breaking_station.position_target;
                
                if (event_pending(EVENT_BREAKING_STATION_GO_HOME)) 
                {
                    event_clear(EVENT_BREAKING_STATION_GO_HOME);
                    new_target = 0;
                } 
                else if (event_pending_any_of(EVENT_MEASURE_DIAMETER | EVENT_MEASURE_FORCE)) 
                {
                    if (event_pending(EVENT_MEASURE_DIAMETER)) 
                    {
                        event_clear(EVENT_MEASURE_DIAMETER);
                        flags |= BREAKING_STATION_FLAG_MEASURE_DIAMETER;
                    } 
                    else if (event_pending(EVENT_MEASURE_FORCE)) 
                    {
                        event_clear(EVENT_MEASURE_FORCE);
                        flags |= BREAKING_STATION_FLAG_MEASURE_FORCE;
                    }
                    new_target = state.breaking_station.position_max;
                    if ((flags & BREAKING_STATION_FLAG_SAVE_AS_MAX_POS) && state.breaking_station.position_max == 0) 
                    {
                        new_target = BREAKING_STATION_TOTAL_LENGTH;
                    }
                    flags |= BREAKING_STATION_FLAG_HOME_AFTER_TARGET;
                } 
                else if (rpc_requested(RPC_BREAKING_STATION_FIND_MAX_POSITION)) 
                {
                    rpc_acknowledge(RPC_BREAKING_STATION_FIND_MAX_POSITION);
                    // make sure this is below the check for EVENT_MEASURE_DIAMETER, otherwise the finding max pos
                    // operation itself will loop back here again
                    log("RPC breaking station find max pos requested\n");
                    state_change(STATE_BREAKING_STATION_POSITION_MAX, 0);
                    flags |= BREAKING_STATION_FLAG_AUTO_INIT;
                    
                    breaking_station_state = BREAKING_STATION_READY;
                }
                
                if (new_target != state.breaking_station.position_target) 
                {
                    state_change(STATE_BREAKING_STATION_POSITION_TARGET, new_target);
                }
                
                if (state_changed(STATE_BREAKING_STATION_POSITION_TARGET)) 
                {
                    breaking_station_state = BREAKING_STATION_START_MOVEMENT;
                } 
                else 
                {
                    if (!state.breaking_station.is_idle) state_change(STATE_BREAKING_STATION_IS_IDLE, true);
                }
            } 
            else 
            {
                if (!state.breaking_station.is_idle) state_change(STATE_BREAKING_STATION_IS_IDLE, true);
            }
            
            if (state.breaking_station.is_idle && !breaking_station_power_off_timer
                && palReadLineLatch(LINE_STENA1) == PAL_LOW) 
            {
                breaking_station_power_off_timer = timer_schedule(breaking_station_power_off, 3000, 0);
            }
            
            
        } 
        else if (breaking_station_state == BREAKING_STATION_START_MOVEMENT) 							// ...开始移动
        {
            state_accept(STATE_BREAKING_STATION_POSITION_TARGET);
            bool moving_away_from_home = (state.breaking_station.position_target >
                                          state.breaking_station.position_actual) ?
                                         true : false;
            slow_down_pos = get_slow_down_pos();
            
            if (moving_away_from_home && (int32_t) state.force >= convert_from(UNIT_N, MAX_FORCE_NOISE)
                && !(flags & BREAKING_STATION_FLAG_MOVING_TO_SAFE_POSITION)) 
            {
                // TODO: signal error
                /* either there is something in the way, so we shouldn't move closer to it, or the force sensor is
                 * broken and it's offset is too high. */
                log("cannot move further, force is already too high\n");
                breaking_station_state = BREAKING_STATION_IDLE;
                continue;
            }
            
            state_change(STATE_BREAKING_STATION_IS_IDLE, false);
            state_change(STATE_BREAKING_STATION_LATCH_ACTIVE, false);
            if (BREAKING_STATION.config->latch_line)
                palClearLine(BREAKING_STATION.config->latch_line); // does not exist on PCB rev 1.02
            state_change(STATE_BREAKING_STATION_LATCH_FORCE, 0);
            state_change(STATE_BREAKING_STATION_OFFSET_CALIBRATION_ACTIVE, false);
            
            int32_t where_to_go;
            
            if (moving_away_from_home && state.breaking_station.position_actual >= slow_down_pos) 
            {
                uint32_t slow_down_speed = state.breaking_station.speed_hardness_measurement;
                if (flags & BREAKING_STATION_FLAG_SAVE_AS_MAX_POS) 
                {
                    slow_down_speed = convertf_from(UNIT_MM_S, 3.0);
                }
                tmc51x0_set_velocity_max(&BREAKING_STATION,
                                         convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                    slow_down_speed));
                
                where_to_go = state.breaking_station.position_target;
                
                flags |= BREAKING_STATION_FLAG_SLOW;
                
                dbg("slow down at %.2f of %.2f mm (%.2f mm from end)\n",
                    convertf_to(UNIT_MM,
                                convertf_from(UNIT_BREAKING_POSITION_STEPS, state.breaking_station.position_actual)),
                    convertf_to(UNIT_MM, convertf_from(UNIT_BREAKING_POSITION_STEPS, get_position_max())),
                    convertf_to(UNIT_MM, convertf_from(UNIT_BREAKING_POSITION_STEPS,
                                                       get_position_max() - state.breaking_station.position_actual)));
                
            } 
            else 
            {
                tmc51x0_set_velocity_max(&BREAKING_STATION,
                                         convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                    state.breaking_station.speed_max));
                
                if (moving_away_from_home) 
                {
                    where_to_go = slow_down_pos + 250; // half the minimum prebrake distance
                    tmc51x0_set_velocity_max(&BREAKING_STATION,
                                             convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                        state.breaking_station.speed_max));
                } 
                else 
                {
                    where_to_go = state.breaking_station.position_target;
                    tmc51x0_set_velocity_max(&BREAKING_STATION,
                                             convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                        state.breaking_station.speed_homing));
                }
                
                flags &= ~BREAKING_STATION_FLAG_SLOW;
            }
            
            breaking_station_go_to_position(where_to_go);
            
            log("%s: going from %d to position %d\n",
                name, state.breaking_station.position_actual, where_to_go);
            
            breaking_station_state = BREAKING_STATION_BUSY;
            
        } 
        else if (breaking_station_state == BREAKING_STATION_BUSY) 												// ...忙
        {
            static uint32_t contact_time = 0;
            static uint32_t block_speed_adjust_until = 0;
            static int modifier = 0;
            
            if (!(flags & BREAKING_STATION_FLAG_CONTACT)) 
            {
                breaking_force = 0;
                contact_time = 0;
                block_speed_adjust_until = 0;
                modifier = 0;
            }
            
            bool is_slow = flags & BREAKING_STATION_FLAG_SLOW;
            bool moving_away_from_home = (state.breaking_station.position_target >
                                          state.breaking_station.position_actual) ?
                                         true : false;
            bool already_there = (BREAKING_STATION.standstill && BREAKING_STATION.position_reached
                                  && state.breaking_station.position_actual == state.breaking_station.position_target)
                                 ? true : false;
            uint32_t force = state.force;
            
            bool force_reading_valid_for_lower_thres = false;
            if (state.force_offset > 0
                && ((moving_away_from_home && state.breaking_station.position_actual > BREAKING_STATION_HOME_OFFSET)
                    ||
                    (!moving_away_from_home && state.breaking_station.position_actual <= BREAKING_STATION_HOME_OFFSET)
                )) 
            {
                force_reading_valid_for_lower_thres = true;
                int32_t latch_force = 0;
                if (flags & BREAKING_STATION_FLAG_MEASURE_FORCE) 
                {
                    if ((flags & BREAKING_STATION_FLAG_CONTACT)
                        && force > max(state.breaking_station.force_threshold_lower,
                                       state.breaking_station.force_threshold_breaking)) 
                    {
                        latch_force = 0 - state.breaking_station.force_threshold_breaking; // negative for force drop
                    }
                } 
                else 
                {
                    latch_force = state.breaking_station.force_threshold_lower;
                }
                if (state.breaking_station.latch_force != latch_force) 
                {
                    state_change(STATE_BREAKING_STATION_LATCH_FORCE, latch_force);
                    dbg("setting latch force to %d\n", latch_force);
                }
            }
            
            // stop immediately if excess force detected, unless we're moving away from it during a move towards a safer
            // position
            bool ignore_force = false;
            if ((flags & BREAKING_STATION_FLAG_MOVING_TO_SAFE_POSITION)
                && (!moving_away_from_home
                    || (state.breaking_station.position_max == 0 && driver->position_actual < 0))) 
            {
                ignore_force = true;
                force_reading_valid_for_lower_thres = false;
            }
            
            if ((already_there
                 ||
                 (force_reading_valid_for_lower_thres && force >= state.breaking_station.force_threshold_lower))
                && !(flags & BREAKING_STATION_FLAG_CONTACT)) 
            {
                if (moving_away_from_home) 
                {
                    // platform has potentially already done this (to minimize latency)
                    state_change(STATE_BREAKING_STATION_LATCH_ACTIVE, true);
                    if (BREAKING_STATION.config->latch_line)
                        palSetLine(BREAKING_STATION.config->latch_line); // does not exist on PCB rev 1.02
                    contact_position = state.breaking_station.position_actual;
                    
                    // unless we are really doing a force measurement, upon any contact we should stop the motor
                    flags |= BREAKING_STATION_FLAG_CONTACT;
                    
                    contact_time = ticks_ms();
                    // the first speed adjustment can be made after just one sample time
                    block_speed_adjust_until = contact_time + state.breaking_station.force_sample_time;
                }
                if (!(flags & BREAKING_STATION_FLAG_MEASURE_FORCE)) 
                {
                    trace("stopping after contact\n");
                    breaking_station_state = BREAKING_STATION_STOP_MOVEMENT;
                } 
                else 
                {
                    // start collecting force & distance samples
                    rpc_call(RPC_COLLECT_FORCE_DISTANCE_SAMPLES);
                }
            } 
            else 
            {
                // time to slow down?
                if (moving_away_from_home && state.breaking_station.position_actual >= slow_down_pos
                    && (!is_slow || (flags & BREAKING_STATION_FLAG_SAVE_AS_MAX_POS))) 
                {
                    uint32_t slow_down_speed = state.breaking_station.speed_hardness_measurement;
                    if (flags & BREAKING_STATION_FLAG_SAVE_AS_MAX_POS) 
                    {
                        if (!is_slow) 
                        {
                            slow_down_speed = convertf_from(UNIT_MM_S, 3.0);
                        } 
                        else 
                        {
                            // second slowdown - good compromise between speed and accuracy, see below for explanation
                            slow_down_speed = convertf_from(UNIT_MM_S, 0.5);
                        }
                    }
                    if ((int32_t)BREAKING_STATION.velocity_max > convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                                   slow_down_speed)) 
                    {
                        tmc51x0_set_velocity_max(&BREAKING_STATION,
                                                 convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                            slow_down_speed));
                        breaking_station_go_to_position(state.breaking_station.position_target);
                        flags |= BREAKING_STATION_FLAG_SLOW;
                        is_slow = true;
                        
                        // when seeking the max pos initially, we slow down to homing speed, which is min. 3 mm/s, which
                        // is a tad inaccurate, but we don't want to use the hardness measurement speed, as that could take
                        // ages. So we use the homing speed until about 3 mm before the expected end (which should be very
                        // similar between machines) and slow down even more then.
                        if (flags & BREAKING_STATION_FLAG_SAVE_AS_MAX_POS) 
                        {
                            slow_down_pos = convert_to(UNIT_BREAKING_POSITION_STEPS,
                                                       convertf_from(UNIT_MM, 31.0));
                        }
                        
                        dbg("slow down at %.2f of %.2f mm (%.2f mm from end)\n",
                            convertf_to(UNIT_MM, convertf_from(UNIT_BREAKING_POSITION_STEPS,
                                                               state.breaking_station.position_actual)),
                            convertf_to(UNIT_MM, convertf_from(UNIT_BREAKING_POSITION_STEPS, get_position_max())),
                            convertf_to(UNIT_MM, convertf_from(UNIT_BREAKING_POSITION_STEPS, get_position_max() -
                                                                                             state.breaking_station.position_actual)));
                        log("%s: going from %d to position %d\n",
                            name, state.breaking_station.position_actual, state.breaking_station.position_target);
                    }
                    
                }
                
                // step positions in which we should never be able to touch anything and therefore have an accurate zero
                // force reading to use as offset
                int32_t safe_zone_start = BREAKING_STATION_HOME_OFFSET + 1;
                int32_t safe_zone_end = BREAKING_STATION_HOME_OFFSET + 100;
                if (slow_down_pos > safe_zone_end && !is_slow) 
                	safe_zone_end = slow_down_pos;
                if (state.breaking_station.position_actual >= safe_zone_start
                    && state.breaking_station.position_actual <= safe_zone_end) 
                {
                    if ((int32_t) state.force > convert_from(UNIT_N, MAX_FORCE_NOISE) && !ignore_force) 
                    {
                        flags |= BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE;
                        if (force > breaking_force) 
                        	breaking_force = force;
                        dbg("excess noise\n");
                        breaking_station_state = BREAKING_STATION_STOP_MOVEMENT;
                    }
                    if (!state.breaking_station.offset_calibration_active) 
                    {
                        state_change(STATE_BREAKING_STATION_OFFSET_CALIBRATION_ACTIVE, true);
                        dbg("entering offset calib zone (f: %u, r: %u, o: %u, p: %u)\n",
                            state.force, state.force_raw, state.force_offset, state.breaking_station.position_actual);
                    }
                } 
                else 
                {
                    if (state.breaking_station.offset_calibration_active) 
                    {
                        state_change(STATE_BREAKING_STATION_OFFSET_CALIBRATION_ACTIVE, false);
                        dbg("leaving offset calib zone (f: %u, r: %u, o: %u, p: %u)\n",
                            state.force, state.force_raw, state.force_offset, state.breaking_station.position_actual);
                    }
                }
                
                
                if (flags & BREAKING_STATION_FLAG_MEASURE_FORCE) 
                {
                    if ((flags & BREAKING_STATION_FLAG_CONTACT)) 
                    {
                        if (state_changed(STATE_FORCE)) 
                        {
                            state_accept(STATE_FORCE);
                            
                            if (force > breaking_force) 
                            {
                                breaking_force = force;
                            } 
                            else 
                            {
                                bool tablet_broken = false;
                                if (state.breaking_station.breaking_detection_mode_derivative) 
                                {
                                    if (state.force_change < 0) 
                                    {
                                        tablet_broken = true;
                                    }
                                } 
                                else 
                                {
                                    if (breaking_force > max(state.breaking_station.force_threshold_lower,
                                                                    state.breaking_station.force_threshold_breaking)
                                               && force <= breaking_force - state.breaking_station.force_threshold_breaking) 
                                    {
                                        tablet_broken = true;
                                    }
                                }
                                if (tablet_broken) 
                                {
                                    // force has dropped below breaking threshold
                                    flags |= BREAKING_STATION_FLAG_BREAK;
                                    trace("tablet broke\n");
                                    breaking_station_state = BREAKING_STATION_STOP_MOVEMENT;
                                    continue;
                                }
                            }
                        }
                        
                        if (state.breaking_station.force_increase_mode == true
                            && state_changed(STATE_FORCE_CHANGE)) 
                        {
                            state_accept(STATE_FORCE_CHANGE);
                            
                            // adjust speed to keep desired force increase constant
                            // the first speed adjustment can be made after one cycle period, all further speed
                            // must wait until the motor has reached the new velocity and at least one entire sample
                            // period has passed to ensure accurate results.
                            // TODO: is this reacting fast enough? + maybe use force_change_rate to make better
                            // decisions when to adjust speed
                            if ((
                                 (modifier == 0 && contact_time + state.breaking_station.force_sample_time <= ticks_ms())
                                 ||
                                 (contact_time + state.breaking_station.force_sample_time * 2.5f <= ticks_ms())
                                ) && block_speed_adjust_until <= ticks_ms()) 
                            {
                                
                                int diff = (int)state.force_change - (int)state.breaking_station.force_increase_amount;
                                if (abs(diff) >= 1500) 
                                {
                                    int speed_then = convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                                (state.breaking_station.speed_hardness_measurement
                                                                      * (100 + modifier))
                                                                / 100);
                                    
                                    if (diff > 0) 
                                    {
                                        modifier += 0 - (int)round1000(diff);
                                    } 
                                    else 
                                    {
                                        diff = 0 - diff;
                                        modifier += (int)round1000(diff);
                                        diff = 0 - diff;
                                    }
                                    if (modifier < -50) 
                                    	modifier = -50;
                                    if (modifier > 50) 
                                    	modifier = 50;
                                    
                                    int speed_now = convert_to(UNIT_BREAKING_POSITION_STEPS_S,
                                                               (state.breaking_station.speed_hardness_measurement * (100 + modifier))
                                                               / 100);
                                    
                                    if (modifier == 0) 
                                    {
                                        modifier = 0xFF; // misuse as flag we've done at least one speed adjustment already
                                    }
                                    
                                    // cache here so ISR after setting the max velocity already changes it (it might be
                                    // shorter!)
                                    uint32_t force_sample_time = state.breaking_station.force_sample_time;
                                    
                                    tmc51x0_set_velocity_max(&BREAKING_STATION, speed_now);
                                    
                                    log("changed speed from %d to %d (change=%u/diff=%d/mod=%d, sample time=%u contact"
                                        "time=%u diff time=%u)\n", speed_then, speed_now, state.force_change,
                                        diff, modifier, state.breaking_station.force_sample_time,
                                        contact_time, ticks_ms() - contact_time);
                                    
                                    contact_time = ticks_ms();
                                    block_speed_adjust_until = contact_time + 2.5f * force_sample_time;
                                }
                                
                            }
                        }
                    } 
                    else if (!(flags & BREAKING_STATION_FLAG_CONTACT)) 
                    {
                        /* check if position is already closer than half a tablet width (nominal_diameter) to the end
                         * (max_position), as that means something is seriously wrong (probably no tablet in place) */
                        if (state.breaking_station.position_actual
                            >= get_position_max() - convert_to(UNIT_BREAKING_POSITION_STEPS,
                                                               state.breaking_station.nominal_diameter / 2)) 
                        {
                            flags |= BREAKING_STATION_FLAG_ERROR_NO_TABLET;
                            trace("no tablet\n");
                            breaking_station_state = BREAKING_STATION_STOP_MOVEMENT;
                        }
                    }
                }
            }
            
        } 
        else if (breaking_station_state == BREAKING_STATION_STOP_MOVEMENT) 								// ...停止移动
        {
            tmc51x0_stop(&BREAKING_STATION);
            if (flags & BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE) 
            {
                if (flags & BREAKING_STATION_FLAG_MOVING_TO_SAFE_POSITION) 
                {
                    log("safe position reached, stopping\n");
                } 
                else 
                {
                    log("EMERGENCY STOP DUE TO EXCESS FORCE!\n");
                }
            }
            
            /* This will update position & standstill flag in driver. Normally, this gets updated in the FSM thread, and
             * if the state of this FSM changes, in between the loop runs. However, if we're about to stop, we
             * intentionally do NOT do it between loops, to keep latency between the state change to STOP_MOVEMENT and
             * the actual command to the motor driver as low as possible. */
            tmc51x0_get_current_position(driver);
            dbg("%s: state change %s (%d) -> %s (%d)\n",
                name, breaking_station_state_name(previous_state), previous_state,
                breaking_station_state_name(breaking_station_state), breaking_station_state);
            previous_state = breaking_station_state;
            
            if (state.breaking_station.offset_calibration_active) 
            {
                state_change(STATE_BREAKING_STATION_OFFSET_CALIBRATION_ACTIVE, false);
                dbg("leaving offset calib zone (f: %u, r: %u, o: %u, p: %u)\n",
                    state.force, state.force_raw, state.force_offset, driver->position_actual);
            }
            
            log("%s: stopping at pos %d (force: %u, force_raw: %u, noise: %u)\n",
                name, driver->position_actual, state.force, state.force_raw, state.force_offset);
            
            
            breaking_station_state = BREAKING_STATION_STOPPING;
            
        } 
        else if (breaking_station_state == BREAKING_STATION_STOPPING) 										// ...停止
        {
            if (BREAKING_STATION.standstill) 
            {
                log("%s: stop at pos %d (force: %u, force_raw: %u, noise: %u, contact_pos: %d, break_force: %u, excess "
                    "force: %u, move to safe pos: %u)\n",
                    name, driver->position_actual, state.force, state.force_raw, state.force_offset, contact_position,
                    breaking_force, flags & BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE,
                    flags & BREAKING_STATION_FLAG_MOVING_TO_SAFE_POSITION);
                
                if (tmc51x0_encoder_deviation_detected(driver)) 
                {
                    log("encoder position deviation detected!\n");
                    breaking_station_state = BREAKING_STATION_INITIALIZE;
                    continue;
                }
                
                // update contact position to more accurate latch value, if available
                if (BREAKING_STATION.config->latch_line) 
                {
#if STE_DEBUG_ENABLED
                    tmc51x0_ramp_status_t rstat = tmc51x0_get_ramp_status(&BREAKING_STATION);
                    dbg("%s: ev_pos=%u, ev_stopr=%u, ev_stopl=%u, latchr_ready=%u, latchl_ready=%u, stopr=%u, stopl=%u\n",
                        name, rstat.event_position_reached, rstat.event_stop_r, rstat.event_stop_l, rstat.status_latch_r,
                        rstat.status_latch_l, rstat.status_stop_r, rstat.status_stop_l);
#endif
                    int32_t latched_position = tmc51x0_get_latched_position(driver);
                    dbg("%s: XLATCH=%d, state.breaking_station.latch_active=%u\n",
                        name, latched_position, state.breaking_station.latch_active);
                    
                    if (latched_position != 0 && abs(latched_position - contact_position) < 1000) 
                    {
                        contact_position = latched_position;
                        dbg("using latched position instead\n");
                    }
                }
                
                state_change(STATE_BREAKING_STATION_LATCH_FORCE, 0);
                palClearLine(BREAKING_STATION.config->latch_line); // does not exist on PCB rev 1.02
                state_change(STATE_BREAKING_STATION_LATCH_ACTIVE, false);
                
                if (flags & BREAKING_STATION_FLAG_BREAK) 
                {
                    log("%s: breaking force was %u\n", name, breaking_force);
                }
                
                if (flags & BREAKING_STATION_FLAG_CONTACT) 
                {
                    log("%s: contact was at %d\n", name, contact_position);
                }
                
                if (flags & BREAKING_STATION_FLAG_SAVE_AS_MAX_POS) 
                {
                    if (!(flags & BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE)) 
                    {
                        log("%s: saving %d as max pos\n", name, contact_position);
                        state_change(STATE_BREAKING_STATION_POSITION_MAX, contact_position);
                        log("%s: %d is now max pos\n", name, state.breaking_station.position_max);
                        if (rpc_requested(RPC_BREAKING_STATION_FIND_MAX_POSITION)) 
                        {
                            rpc_reply_arg(RPC_BREAKING_STATION_FIND_MAX_POSITION, contact_position);
                        }
                    } 
                    else 
                    {
                        log("%s: not saving %d as max pos due to excess force\n", name, contact_position);
                    }
                }
                
                if (!BREAKING_STATION.position_reached) 
                {
                    // this won't cause further movement, but ensures that the motor won't start moving once we set the max
                    // velocity > 0 again in case the position target was further off and we stopped by setting max
                    // velocity to 0
                    tmc51x0_set_target_position(&BREAKING_STATION, driver->position_actual);
                    dbg("re-set target pos\n");
                }
                
                if (flags & BREAKING_STATION_FLAG_ERROR_NO_TABLET) 
                {
                    // TODO: signal error
                    dbg("stopped because no tablet found\n");
                } 
                else if (flags & BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE) 
                {
                    if (flags & BREAKING_STATION_FLAG_MOVING_TO_SAFE_POSITION) 
                    {
                        dbg("safe position reached and stopped\n");
                        flags &= ~(BREAKING_STATION_FLAG_MOVING_TO_SAFE_POSITION
                            | BREAKING_STATION_FLAG_HOME_AFTER_TARGET | BREAKING_STATION_FLAG_TARGET_AFTER_HOME);
                        // movement_prohibited will trigger now and keep everything blocked until user clears the error
                    } 
                    else 
                    {
                        // TODO: signal error
                        dbg("stopped after excess force (%d)\n", breaking_force);
                        
                        int32_t safe_distance = convert_to(UNIT_BREAKING_POSITION_STEPS,
                                                            convert_from(UNIT_MM, 2));
                        if (driver->position_actual < 0) 
                        {
                            // this is the case if excess force is detected during initial homing
                            if (0 - driver->position_actual >= safe_distance) 
                            {
                                postponed_target = driver->position_actual + safe_distance;
                            } 
                            else 
                            {
                                postponed_target = 0;
                            }
                        } 
                        else 
                        {
                            if (driver->position_actual >= safe_distance) 
                            {
                                postponed_target = driver->position_actual - safe_distance;
                            } 
                            else 
                            {
                                postponed_target = 0;
                            }
                        }
                        flags |= (BREAKING_STATION_FLAG_TARGET_AFTER_HOME |
                                  BREAKING_STATION_FLAG_MOVING_TO_SAFE_POSITION);
                    }
                } 
                else 
                {
                    if (flags & BREAKING_STATION_FLAG_MEASURE_DIAMETER) 
                    {
                        if (!(flags & BREAKING_STATION_FLAG_SAVE_AS_MAX_POS)) 
                        {
                            int32_t diameter = convert_from(UNIT_BREAKING_POSITION_STEPS,
                                                            state.breaking_station.position_max - contact_position);
                            event_emit_value(EVENT_MEASURE_DIAMETER, diameter);
                        }
                    }
                    if (flags & BREAKING_STATION_FLAG_MEASURE_FORCE) 
                    {
                        ensure_sane_force_distance_curve(breaking_force);
                        event_emit_value(EVENT_MEASURE_FORCE, breaking_force);
                    }
                }
                
                if (flags & (BREAKING_STATION_FLAG_HOME_AFTER_TARGET | BREAKING_STATION_FLAG_TARGET_AFTER_HOME)) 
                {
                    int32_t new_target = 0;
                    
                    if (flags & BREAKING_STATION_FLAG_TARGET_AFTER_HOME) 
                    {
                        new_target = postponed_target;
                        postponed_target = 0;
                    }
                    
                    flags &= ~(BREAKING_STATION_FLAG_HOME_AFTER_TARGET | BREAKING_STATION_FLAG_TARGET_AFTER_HOME);
                    
                    state_change(STATE_BREAKING_STATION_POSITION_TARGET, new_target);
                    breaking_station_state = BREAKING_STATION_START_MOVEMENT;
                    
                } 
                else 
                {
                    breaking_station_state = BREAKING_STATION_IDLE;
                }
                
                event_clear(EVENT_MEASURE_FORCE | EVENT_MEASURE_DIAMETER);
                flags &= ~(BREAKING_STATION_FLAG_MEASURE_DIAMETER | BREAKING_STATION_FLAG_MEASURE_FORCE
                           | BREAKING_STATION_FLAG_HOME_AFTER_TARGET | BREAKING_STATION_FLAG_TARGET_AFTER_HOME
                           | BREAKING_STATION_FLAG_BREAK | BREAKING_STATION_FLAG_CONTACT
                           | BREAKING_STATION_FLAG_ERROR_NO_TABLET);
                if (!(flags & BREAKING_STATION_FLAG_ERROR_EXCESS_FORCE)) 
                {
                    flags &= ~BREAKING_STATION_FLAG_SAVE_AS_MAX_POS;
                }
            }
        }
    } while (breaking_station_state != state_at_start);
}

void ensure_sane_force_distance_curve(uint32_t breaking_force) 
{
    if (!rpc_requested(RPC_COLLECT_FORCE_DISTANCE_SAMPLES)) 
    {
        // TODO: signal error
    } 
    else 
    {
        rpc_stop_await(RPC_COLLECT_FORCE_DISTANCE_SAMPLES);
        force_distance_curve_t* curve = rpc_resultp(RPC_COLLECT_FORCE_DISTANCE_SAMPLES);
        assert(curve != NULL, "invalid curve pointer");
        
        /* we want to ensure that the curve that we output contains at least the force sample we communicated
         * already as the result for the command (i.e. the 1ms sampling didn't catch the maximum force), but
         * it shouldn't contain any higher force sample either (i.e. the FSM sampling didn't catch it). While
         * the latter sounds worrisome, it's not very much, as we're talking about differences of less than
         * the measurement precision of the sensor. But for optical purposes, we shouldn't leave the end
         * users scratching their heads why there's a difference between the result and the curve.
         * For this to work, we need to dual approach. Capping to the actual max is easy and can be done in
         * the actual output loop, but to ensure the minimum, we need to iterate over all samples before the
         * actual output loop and note the maximum force sample. Then, we can check for that value in the
         * output loop and replace them with the higher value (simply noting the max sample found position
         * isn't enough as there could be more than 1 sample with the same value). */
        
        dbg("curve samples count: %u, distance_accumulator: %u, distance_total: %u, force_max_in_curve: %u\n",
            ringbuffer_count(curve->rb_samples), curve->initial_distance_offset, curve->total_distance, curve->force_max);
        
        uint32_t distance_accumulator = curve->total_distance - curve->initial_distance_offset;
        uint32_t force_max_of_response = round100(breaking_force);
        size_t num_samples = ringbuffer_count(curve->rb_samples);
        for (size_t i = 0; i < num_samples; i++) 
        {
            force_distance_sample_t* sample =
                (force_distance_sample_t*) ringbuffer_get_struct_ptr(curve->rb_samples, i);
            distance_accumulator += sample->distance_increment;
            if (force_max_of_response > 0 && sample->force > force_max_of_response) 
            {
                dbg("capping force sample at index %u (= %u) with %u\n", i, sample->force, force_max_of_response);
                sample->force = force_max_of_response;
            }
            if (curve->force_max > 0 && force_max_of_response > 0
                && curve->force_max < force_max_of_response
                && sample->force == curve->force_max) 
            {
                dbg("raising force sample at index %u (= %u)  to  %u\n", i, sample->force, force_max_of_response);
                sample->force = force_max_of_response;
            }
        }
    }
}
