/**********************************************************
 *
 * Implements a USART on the XMEGA.
 *
 * Written by Taj Morton for OSURC Mars Rover.
 * Copyright (C) 2010 OSURC.
 *
 **********************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "USART.h"

#define USART_Format_Set(_usart, _charSize, _parityMode, _twoStopBits)         \
	(_usart)->CTRLC = (uint8_t) _charSize | _parityMode |                      \
	(_twoStopBits ? USART_SBMODE_bm : 0)

#define USART_Rx_Enable(_usart) ((_usart)->CTRLB |= USART_RXEN_bm)
#define USART_Rx_Disable(_usart) ((_usart)->CTRLB &= ~USART_RXEN_bm)
#define USART_Tx_Enable(_usart)	((_usart)->CTRLB |= USART_TXEN_bm)
#define USART_Tx_Disable(_usart) ((_usart)->CTRLB &= ~USART_TXEN_bm)

#define USART_RxdInterruptLevel_Set(_usart, _rxdIntLevel)                      \
	((_usart)->CTRLA = ((_usart)->CTRLA & ~USART_RXCINTLVL_gm) | _rxdIntLevel)

#define USART_TxdInterruptLevel_Set(_usart, _txdIntLevel)                      \
	(_usart)->CTRLA = ((_usart)->CTRLA & ~USART_TXCINTLVL_gm) | _txdIntLevel

#define USART_IsTXDataRegisterEmpty(_usart) (((_usart)->STATUS & USART_DREIF_bm) != 0)
#define USART_IsTXComplete(_usart) (((_usart)->STATUS & USART_TXCIF_bm) != 0)

#define USART_TXEn_Disable(_usart) if ((_usart)->use_rs485) \
	(_usart)->port.gpio_port->OUTCLR = (_usart)->port.txen_pin_bm;


//#define USART_IsTXDataRegisterEmpty(_usart) (((_usart)->STATUS & USART_TXCIF_bm) != 0)

void USART_InitPortStructs( void ) {
	// These values calculated with a scale factor of -7 for accuracy
	// This BSCALE is set for the USART in USART_Open
	USART_BAUD_TABLE[USART_BAUD_2400] = 3329;
	USART_BSCALE_TABLE[USART_BAUD_2400] = -2;

	USART_BAUD_TABLE[USART_BAUD_4800] = 3326;
	USART_BSCALE_TABLE[USART_BAUD_4800] = -3;

	USART_BAUD_TABLE[USART_BAUD_9600] = 3318;
	USART_BSCALE_TABLE[USART_BAUD_9600] = -4;

	USART_BAUD_TABLE[USART_BAUD_19200] = 3302;
	USART_BSCALE_TABLE[USART_BAUD_19200] = -5;

	USART_BAUD_TABLE[USART_BAUD_38400] = 3270;
	USART_BSCALE_TABLE[USART_BAUD_38400] = -6;

	USART_BAUD_TABLE[USART_BAUD_115200] = 2098;
	USART_BSCALE_TABLE[USART_BAUD_115200] = -7;

	USART_BAUD_TABLE[USART_BAUD_76800] = 3205;
	USART_BSCALE_TABLE[USART_BAUD_76800] = -7;

	// 250000 baud, 0% error
	USART_BAUD_TABLE[USART_BAUD_250000] = 3;
	USART_BSCALE_TABLE[USART_BAUD_250000] = 1;	

	// 1M baud, 0% error
	USART_BAUD_TABLE[USART_BAUD_1M] = 1;
	USART_BSCALE_TABLE[USART_BAUD_1M] = 0;	
	

	// USART0
	usart_ports[0].usart_port = &USARTC0;
	usart_ports[0].gpio_port = &PORTC;
	usart_ports[0].tx_pin_bm = PIN3_bm;
	usart_ports[0].rx_pin_bm = PIN2_bm;
	usart_ports[0].txen_pin_bm = PIN1_bm;

	// USART1
	usart_ports[1].usart_port = &USARTC1;
	usart_ports[1].gpio_port = &PORTC;
	usart_ports[1].tx_pin_bm = PIN7_bm;
	usart_ports[1].rx_pin_bm = PIN6_bm;
	usart_ports[1].txen_pin_bm = PIN5_bm;

	// USART2
	usart_ports[2].usart_port = &USARTD0;
	usart_ports[2].gpio_port = &PORTD;
	usart_ports[2].tx_pin_bm = PIN3_bm;
	usart_ports[2].rx_pin_bm = PIN2_bm;
	usart_ports[2].txen_pin_bm = PIN1_bm;

	// USART3
	usart_ports[3].usart_port = &USARTD1;
	usart_ports[3].gpio_port = &PORTD;
	usart_ports[3].tx_pin_bm = PIN7_bm;
	usart_ports[3].rx_pin_bm = PIN6_bm;
	//usart_ports[3].txen_pin_bm = PIN5_bm;
	usart_ports[3].txen_pin_bm = 0;

	// USART4
	// 	usart_ports[4].usart_port = &USARTE0;
	// 	usart_ports[4].gpio_port = &PORTE;
	// 	usart_ports[4].tx_pin_bm = PIN3_bm;
	// 	usart_ports[4].rx_pin_bm = PIN2_bm;
	// 	usart_ports[4].txen_pin_bm = PIN1_bm;

	// USART5
	// 	usart_ports[5].usart_port = &USARTE1;
	// 	usart_ports[5].gpio_port = &PORTE;
	// 	usart_ports[5].tx_pin_bm = PIN7_bm;
	// 	usart_ports[5].rx_pin_bm = PIN6_bm;
	// 	usart_ports[5].txen_pin_bm = PIN4_bm;

	// USART6
	// 	usart_ports[6].usart_port = &USARTF0;
	// 	usart_ports[6].gpio_port = &PORTF;
	// 	usart_ports[6].tx_pin_bm = PIN3_bm;
	// 	usart_ports[6].rx_pin_bm = PIN2_bm;
	// 	usart_ports[6].txen_pin_bm = PIN4_bm;
}

void USART_Open(struct USART * serial, unsigned char port, unsigned char baud_rate, unsigned short tx_buf, unsigned short rx_buf, bool use_rs485 ) {

	serial->port_num = port;
	serial->port = usart_ports[port];

	serial->baud_rate = baud_rate;
	serial->use_rs485 = use_rs485;

	RingBufferInit(&serial->tx_buffer, tx_buf);
	RingBufferInit(&serial->rx_buffer, rx_buf);

	USART_Table[port] = serial;

	// Set the TxD pin high
	serial->port.gpio_port->OUTSET = serial->port.tx_pin_bm;

	// set the TxD pin as an output, RxD as an input
	serial->port.gpio_port->DIRSET = serial->port.tx_pin_bm;
	serial->port.gpio_port->DIRCLR = serial->port.rx_pin_bm;

	if (use_rs485) {
		serial->port.gpio_port->DIRSET = serial->port.txen_pin_bm;
		serial->port.gpio_port->OUTCLR = serial->port.txen_pin_bm;
	}

	cli();	// disable interrupts

	// Enable the receive interrupt, to copy the data to a buffer
	serial->port.usart_port->CTRLA = ((serial->port.usart_port)->CTRLA & ~USART_RXCINTLVL_gm) | USART_RXCINTLVL_LO_gc;

	if (use_rs485 ) { // only enable the TXC interrupt if we're in RS485 mode, as it's only used to clear the TXEN line
		// set transmit completion as low-level interrupt
		serial->port.usart_port->CTRLA = ((serial->port.usart_port)->CTRLA & ~USART_TXCINTLVL_gm) | USART_TXCINTLVL_LO_gc;
	}

	// Set the baud rate
	serial->port.usart_port->BAUDCTRLA = USART_BAUD_TABLE[baud_rate] & 0xFF;
	serial->port.usart_port->BAUDCTRLB = (USART_BAUD_TABLE[baud_rate] >> 8);

	serial->port.usart_port->BAUDCTRLB |= ((USART_BSCALE_TABLE[baud_rate]&0x0F) << USART_BSCALE0_bp);

	// Set the mode of operation
	serial->port.usart_port->CTRLC = 0x03; // 8 bits, no parity, one stop bit

	// Enable the transmitter and receiver
	serial->port.usart_port->CTRLB = 0x1C;

	PMIC.CTRL |= PMIC_LOLVLEX_bm;	// enable low-level interrupts

	sei();	// enable interrupts
}

unsigned short USART_Write(struct USART * serial, char * buf, unsigned short num_bytes) {
	unsigned short written;
	unsigned char tempCTRLA = serial->port.usart_port->CTRLA;

	// Disable the data register interrupt to prevent access to ring buffer while we're writing data
	tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
	serial->port.usart_port->CTRLA = tempCTRLA;

	written = RingBufferAdd(&serial->tx_buffer, buf, num_bytes);

	if (serial->use_rs485) {
		serial->port.gpio_port->OUTSET = serial->port.txen_pin_bm;
	}

	// re-enable data register empty low-level interrupt
	tempCTRLA = serial->port.usart_port->CTRLA;
	tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_LO_gc;
	serial->port.usart_port->CTRLA = tempCTRLA;
	return written;
}

void USART_TransmitMode(struct USART * serial, bool doTx) {

	if (serial->use_rs485) {
		if (doTx) {
			serial->port.gpio_port->OUTSET = serial->port.txen_pin_bm;
		}
		else {
			serial->port.gpio_port->OUTCLR = serial->port.txen_pin_bm;
			USART_Rx_Enable(serial->port.usart_port);	// redundant, but why not?
		}
	}
}

unsigned short USART_Read(struct USART * serial, char * buf, unsigned short max_bytes) {
	return RingBufferGetData(&serial->rx_buffer, buf, max_bytes);
}

unsigned char USART_ReadByte(struct USART * serial) {
	return (unsigned char) RingBufferGetByte(&serial->rx_buffer);
}

unsigned short USART_TXFree(struct USART * serial) {
	return RingBufferBytesFree(&serial->tx_buffer);
}

unsigned short USART_TXUsed(struct USART * serial) {
	return RingBufferBytesUsed(&serial->tx_buffer);
}

unsigned short USART_RXFree(struct USART * serial) {
	return RingBufferBytesFree(&serial->rx_buffer);
}

unsigned short USART_RXUsed(struct USART * serial) {
	return RingBufferBytesUsed(&serial->rx_buffer);
}

void USART_RXIntComplete(unsigned char port) {
	char dat = USART_Table[port]->port.usart_port->DATA;
	RingBufferAddByte(&USART_Table[port]->rx_buffer, dat);
}

void USART_DataRegEmpty(unsigned char port) {
	if (RingBufferBytesUsed(&USART_Table[port]->tx_buffer)==0) { // TX buffer empty, disable interrupts
		unsigned char tempCTRLA = USART_Table[port]->port.usart_port->CTRLA;
		tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
		USART_Table[port]->port.usart_port->CTRLA = tempCTRLA;
	}
	else {
		USART_Table[port]->port.usart_port->DATA = RingBufferGetByte(&USART_Table[port]->tx_buffer);
	}
}

void USART_TXIntComplete(unsigned char port) {
	USART_TXEn_Disable(USART_Table[port]);
}


void USART_WriteByte(struct USART * serial, unsigned char dat) {
	USART_Write(serial,(char*)&dat,1);
}

ISR(USARTC0_RXC_vect)
{
	USART_RXIntComplete(0);
}

ISR(USARTC0_TXC_vect)
{
	USART_TXIntComplete(0);
}

ISR(USARTC0_DRE_vect)
{
	USART_DataRegEmpty(0);
}

ISR(USARTC1_RXC_vect)
{
	USART_RXIntComplete(1);
}

ISR(USARTC1_TXC_vect)
{
	USART_TXIntComplete(1);
}

ISR(USARTC1_DRE_vect)
{
	USART_DataRegEmpty(1);
}

ISR(USARTD0_RXC_vect)
{
	USART_RXIntComplete(2);
}

ISR(USARTD0_TXC_vect)
{
	USART_TXIntComplete(2);
}

ISR(USARTD0_DRE_vect)
{
	USART_DataRegEmpty(2);
}

ISR(USARTD1_RXC_vect)
{
	USART_RXIntComplete(3);
}

ISR(USARTD1_TXC_vect)
{
	USART_TXIntComplete(3);
}

ISR(USARTD1_DRE_vect)
{
	USART_DataRegEmpty(3);
}

// ISR(USARTE0_RXC_vect)
// {
// 	USART_RXIntComplete(4);
// }
// 
// ISR(USARTE0_TXC_vect)
// {
// 	USART_TXIntComplete(4);
// }
// 
// ISR(USARTE0_DRE_vect)
// {
// 	USART_DataRegEmpty(4);
// }
// 
// ISR(USARTE1_RXC_vect)
// {
// 	USART_RXIntComplete(5);
// }
// 
// ISR(USARTE1_TXC_vect)
// {
// 	USART_TXIntComplete(5);
// }
// 
// ISR(USARTE1_DRE_vect)
// {
// //	USART_DataRegEmpty(5);
// }
// 
// ISR(USARTF0_RXC_vect)
// {
// 	USART_RXIntComplete(6);
// }
// 
// ISR(USARTF0_TXC_vect)
// {
// 	USART_TXIntComplete(6);
// }
// 
// ISR(USARTF0_DRE_vect)
// {
// 	USART_DataRegEmpty(6);
// }

ISR( BADISR_vect ) {
	// do nothing
}
