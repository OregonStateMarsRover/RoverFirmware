/*BogieCommon.h
 *Authors: Mike Fortner, Chris Newman
 */

#ifndef BOGIE_COMMON_HEADER
#define BOGIE_COMMON_HEADER

#include "avr_compiler.h"

#include "USART.h"
#include "Sabertooth.h"
#include "SerialProtocol.h"

struct BogieControllerData {
	struct USART motor;
	struct USART bb;
	
	SerialData packet;
};

struct BogieControllerData bogie;

// Used for setting the system clock to 16Mhz
void set_clock( void );

#endif
