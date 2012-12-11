/*
 * Header for the PID functions
 */

#ifndef BOGIE_PID
#define BOGIE_PID

#include "BogieCommon.h"

void setup_rtc( uint16_t period );

struct pid{
	uint8_t dt;
	int16_t ramp;
	int16_t p, i;
	int16_t setpoint, pv, output;
};

struct pid speed_pid;

void pid_setup( struct pid * settings, int16_t p, int16_t i, int16_t d, int16_t ramp, uint8_t dt);

void pid_speed_controller( struct pid * val );



#endif
