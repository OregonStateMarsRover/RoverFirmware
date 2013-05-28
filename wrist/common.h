/*BogieCommon.h
 *Authors: Marshal Horn
 */

#ifndef COMMON_HEADER
#define COMMON_HEADER

#include "avr_compiler.h"
#include <stdlib.h>
#include "USART.h" 
#include "SerialProtocol.h"
#define ADDRESS 9 // Address of the wrist controller


/* Pull LEDs high to turn them on */
#define GREEN 0x01	// green LED on port E
#define RED 0x02	// red LED on port E


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

/* If I define them as void*, then I don't have to
 * worry about what type they are right now.
 */
struct global{
	USART * probe;
	USART * ax12;
	USART * bus;
	USART * aux;
	SerialData * packet;
	uint8_t new_probe_data;	// set TRUE if there's new data to process
};

struct global wrist;

#endif
