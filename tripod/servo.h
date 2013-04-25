/*
 * AX-12 Servos
 *
 * The datasheet for these servos can be found in the hw12 repository at
 * documents/motors/AX-12(English).pdf
 * 
 * Software overview:
 *
 * Half-duplex UART; transmitter should be disabled when not transmitting
 * (Taken care of in USART.c?)
 * AX-12 USART is on PORTD; The TX enable is PD4
 *
 * 
 * INSTRUCTION PACKET: (sent to the servo)
 * 0xFF 0xFF
 * ID
 * Length
 * Instruction
 * Parameters...
 * Checksum = ~sum( ID through parameters)
 *
 *
 * STATUS PACKET: (sent by the servo, only as a reply to an instruction)
 * OxFF 0xFF
 * ID
 * Length
 * Error
 * Parameters...
 * Checksum = ~sum( ID through parameters)
 *
 *
 */

/* Errors (bitmasked) */
#define INSTRUCTION_ERR (1<<6) /* undefined function
 * Could also mean an action instruction is sent without a Reg_Write instruction 
 */
#define OVERLOAD_ERR (1<<5) // specified torque can't control the load
#define CHECKSUM_ERR (1<<4)
#define RANGE_ERR (1<<3) // instruction is out of range
#define OVER_TEMP (1<<2)
#define ANGLE_ERR (1<<1) // Angle sent is outside the CW and CCW angle limits
#define VOLT_ERR (1<<0) // Supply voltage is too low or too high



/* Initial baud rate is 1M.
 * 9600 and 19200 have acceptable error rates.  115200 is TERRIBLE.
 * Probably the easiest thing would be to simply leave it at the initial baud rate.
 * I'm assuming the XMEGA can handle an interrupt every 160 cycles.
 *
 */
#define BROADCAST_ID 0xFE	// used to send a message to ALL the servos at once


struct servo{
	uint8_t id;
	uint8_t status;
	uint16_t setpoint;
};


struct message{
	uint8_t index;
	uint8_t * data;
	uint8_t len;
};



/* Action instructions 
 * These will be blocking for now, meaning they'll wait until they receive a reply.
 * If I made this threaded, I'd have to make a thread manager, and make my functions
 * thread-safe.
 * All these functions return an error.
 * If the error is nonzero, it can mean a bad checksum, a timeout, or unknown.
 */
uint8_t servo_ping( struct servo s );

uint8_t servo_read_data( struct servo * s, struct message * m );

uint8_t servo_write_data( struct servo * s, struct message * m );

uint8_t servo_reg_write( struct servo * s, struct message * m );

uint8_t servo_write_flush( struct servo * s ); // completion of reg_write
// (The datasheet refers to this command as Action)

uint8_t servo_reset( struct servo * s );

uint8_t servo_sync_write( struct servo * s, unit8_t quant, struct message * m);




uint8_t servos_init( void );

uint8_t servo_set_angle( struct servo * s, uint16_t angle );

