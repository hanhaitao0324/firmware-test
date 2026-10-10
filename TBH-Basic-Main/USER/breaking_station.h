#ifndef BREAKING_STATION_H
#define BREAKING_STATION_H

#include "ste.h"
#include "tmc51x0.h"

#if BREAKING_USE_MICROSTEPS
#define BREAKING_STATION_HOME_OFFSET (400 * 256)
#define BREAKING_STATION_TOTAL_LENGTH (7000 * 256)
#else
#define BREAKING_STATION_HOME_OFFSET (400)
#define BREAKING_STATION_TOTAL_LENGTH (6700)
#endif

extern tmc51x0_driver_t BREAKING_STATION;

extern void breaking_station_init(void);
extern void breaking_station_start(void);
extern void breaking_station_power_on(void);
extern void breaking_station_power_off(void);
extern void breaking_station_go_to_position(int32_t position);
extern void breaking_station_fsm(void);

#endif //BREAKING_STATION_H
