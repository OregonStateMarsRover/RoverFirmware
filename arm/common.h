/*BogieCommon.h
 *Authors: Mike Fortner, Chris Newman
 */

#ifndef BOGIE_COMMON_HEADER
#define BOGIE_COMMON_HEADER

#include "avr_compiler.h"

#include "USART.h"
#include "Sabertooth.h"
#include "SerialProtocol.h"
#include "pots.h"
#include "itoa.h"

#define ADDRESS 8 // Address of the arm controller

// Limit switches are on PORTB
// If they are hi, that means they aare pressed or disconnected
#define LIM0 0x01
#define LIM1 0x02
#define LIM2 0x04
#define LIM3 0x08
#define LIMS 0x0F


/* Pull LEDs low to turn them on */
#define GREEN 0x10	// green LED on port D
#define RED 0x20	// red LED on port D


// Used for setting the system clock to 16Mhz
void set_clock( void );

/* This function just gets the time from the RTC.
 * If you want something with even less overhead,
 * then just use RTC.CNTL
 */
#define get_time() (RTC.CNT)

/* Set up the real-time clock for timing tasks.
 * A prescaler of 1 will yield 1 tick every 30.5us
 * A prescaler of 5 will yield 1 tick every 2ms
 * A prescaler of 0 will stop the clock
 */
void setup_rtc( uint8_t prescaler );


struct pid{
	int16_t setpoint;
	_Accum pv_scale;	// s15.16 fixed point
	int16_t pv_offset;
	_Accum pv;			// calculated, not raw
	short _Accum p;		// s7.8 fixed point
	int16_t output;
};

void update_pid( struct pid *v, uint8_t enc );


void handle_packet( SerialData *s );
void packet_error( SerialData *s, uint8_t errCode );


struct status_data{
	struct USART motor;
	struct USART bb;
	
	SerialData packet;

	struct pid elbow, shoulder;
};

struct status_data arm;

#endif
