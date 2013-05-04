/*arm.c
 *Authors: Marshal Horn
 */

#include "arm.h"
#include <stdio.h>

void init(void) {
	set_clock( ); // set clock to 16Mhz

	USART_InitPortStructs();
	
	/***set I/O port directions***/
	PORTA.DIR = 0x00;
	PORTB.DIR = 0x00;
	PORTC.DIR = 0b10011010;	//outputs are UART TX and SPI control
	PORTD.DIR = 0b00111110;	//all outputs; rest are unused

	/***Motor Driver USART init***/
		
	
	USART_Open(&arm.motor, 2, USART_BAUD_9600, 10, 10, false);
	/***Mainboard USART init***/
	USART_Open(&arm.bb, 0, USART_BAUD_115200, 100, 100, true);
	
	SerialDataInitialize( &arm.packet );
	// Set behavior when packet is received
	arm.packet.ReceivePacketComplete = handle_packet;
	// Return error over RS485
	//arm.packet.ReceiveDataError = packet_error;

	/*** Initialize Sabertooth Motor Driver ***/
	
	sabertooth_init(&arm.motor);
	
	
	sei();
}


int main(void)
{

	init();

	char text[10];


	setup_rtc(1);
	init_spi( 6 );	// don't set this lower than 4 - the RS483 chips are only made for 1Mhz

	RingBuffer * buffer = &(arm.bb.rx_buffer);
	uint8_t new_byte;
	uint16_t time = get_time();
	int8_t test_speed = 50;


	struct pid * shoulder = &arm.shoulder;
	struct pid * elbow = &arm.elbow;

	shoulder->pv_scale = 0.17578;
	shoulder->pv_offset = 2121;
	shoulder->p = 20;
	shoulder->output = 0;
	shoulder->setpoint = 130;

	elbow->pv_scale = 0.17578;
	elbow->pv_offset = 2915;
	elbow->p = -20;
	elbow->output = 0;
	elbow->setpoint = 20;

	while(1) {
		if( get_time() - time > 10 ) 
		{
			PORTD.OUTTGL = RED;

			time = get_time();
			
			update_pid( shoulder, 0 );
			update_pid( elbow, 1 );

			/*
			USART_Write( &arm.bb, (unsigned char *)text, mitoa( shoulder->pv, text, 10) );
			USART_Write( &arm.bb, (unsigned char *)"\t", 1 );
			USART_Write( &arm.bb, (unsigned char *)text, mitoa( elbow->pv, text, 10) );
			USART_Write( &arm.bb, (unsigned char *)"\r\n", 2 );
			*/
			
			shoulder_set( shoulder->output );
			elbow_set( elbow->output );

		}

		else if( RingBufferBytesUsed( buffer ) ) 
		{
			new_byte = RingBufferGetByte( buffer );
			ProcessDataChar( &(arm.packet), new_byte );
		} 
	}


	return 0;
}

