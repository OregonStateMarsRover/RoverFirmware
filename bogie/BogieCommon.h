/*BogieCommon.h
 *Authors: Mike Fortner, Chris Newman
 */

#ifndef BOGIE_COMMON_HEADER
#define BOGIE_COMMON_HEADER

#include "avr_compiler.h"

#include "USART.h"
#include "Sabertooth.h"
#include "SerialProtocol.h"

struct pid{
	int16_t setpoint;
	_Accum pv_scale;	// s15.16 fixed point
	_Accum pv;			// calculated, not raw
	short _Accum p;		// s7.8 fixed point
	int16_t output;
};

struct BogieControllerData {
	struct USART motor;
	struct USART bb;
	
	SerialData packet;

	struct pid steer;
};

struct BogieControllerData bogie;

// Used for setting the system clock to 16Mhz
void set_clock( void );

void setup_rtc( uint8_t prescaler );

void pid_steer( void );

#endif
