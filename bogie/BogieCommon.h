/*BogieCommon.h
 *Authors: Mike Fortner, Chris Newman
 */

#ifndef BOGIE_COMMON_H
#define BOGIE_COMMON_H

#include "avr_compiler.h"

#include "USART.h"
#include "PacketQueuer.h"
#include "CommInterface.h"
#include "Encoders.h"
#include "Sabertooth.h"

struct BogieControllerData {
	CommInterface bb_com;
	struct USART motor;
	struct USART bb;
	
	PacketQueue pktQueue;
	// Where do these numbers come from???
	CommPacket queuedPackets[6];
	unsigned char queuedData[6 * 20];
};

struct BogieControllerData bogie;

void parse_command( CommPacket *pkt );

struct PIDobject 
{
	uint16_t p;			//proportional constant
	uint16_t i;			//integral constant
	uint16_t d;			//derivative constant
	uint16_t prev_error;//most recent error
	uint16_t sum_error; //sum of previous errors
	uint16_t dt;		//delta time between calculations
};

typedef struct PIDobject PIDobject;


uint8_t pid(PIDobject *pid, uint16_t desired, uint16_t actual);

void pid_init( void );

#endif
