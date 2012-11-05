/* Sabertooth.c
*  Authors: Mike Fortner, Chris Newman
*/

#include "Sabertooth.h"

#define MAX 127

#define SABERTOOTH_ADDRESS 128
#define DRIVE_FORWARD_CMD 0
#define DRIVE_REVERSE_CMD 1
#define MIN_VOLT_CMD 2
#define MAX_VOLT_CMD 3
#define ACTUATOR_FORWARD_CMD 4
#define ACTUATOR_REVERSE_CMD 5
#define MIXED_FWD 8 	 // used for stopping as quickly as possible
#define TIMEOUT_CMD 14	 // not persisent
#define BAUD_SET_CMD 15	 // persistant.  Use this with care
#define RAMP_RATE_CMD 16 // persistant
#define DEADBAND_CMD 17	 // persistant

uint8_t desired_speed = 0;
uint8_t desired_angle = 0;

struct USART *port;
PIDobject act_pid;

void sabertooth_init(struct USART *p)
{
	port = p;
	uint8_t timeout = 1; //1 * 100ms = 100ms

	send_command(TIMEOUT_CMD, timeout);
	
	/* We could also set the min and max voltage.
	 * However, I think we want to always keep it in regenerative mode,
	 * instead of doing a hard brake (and absorbing the energy
	 * as heat)
	 * However, we do want to get rid of the deadband, to make
	 * PID operate more smoothly.
	 * I don't know yet if we want ramping.  Probably not.
	 */
	
	send_command( DEADBAND_CMD, 1 );	// set deadband to minimum



}

void send_command(uint8_t opcode, uint8_t data)
{
	uint8_t address = SABERTOOTH_ADDRESS;
	USART_WriteByte(port, address);
	USART_WriteByte(port, opcode);
	USART_WriteByte(port, data);
	USART_WriteByte(port, (address + opcode + data) & 0x7F);
}

void motor_set(int8_t speed, uint8_t forward_cmd, uint8_t reverse_cmd)
{
	uint8_t opcode;

	if (speed >= 0)
	{
		if (speed > MAX) speed = MAX;
		opcode = forward_cmd;
	}
	else
	{	// flip speed and use reverse command
		speed = (speed <= -MAX)? MAX : -speed;
		opcode = reverse_cmd;
	}

	send_command(opcode, speed);
}

void actuator_set(int8_t speed)
{
	motor_set(speed, ACTUATOR_FORWARD_CMD, ACTUATOR_REVERSE_CMD);
}

void drive_set(int8_t speed)
{
	motor_set(speed, DRIVE_FORWARD_CMD, DRIVE_REVERSE_CMD);
}


// stop all motors
void motor_stop( void ) {
	send_command( MIXED_FWD, 0 );
}
