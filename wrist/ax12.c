#include "ax12.h"

void InitServos(USART *device_USART)
{
	servoUSART = device_USART;
	PanTiltSetCompliance(1,1,1,8,8);
	_delay_ms(50);
}

void PanTiltSetPosition(char servo, unsigned short position) {
	char params[3] = { 0x1E, position & 0xFF, (position>>8) };
	SendServoCommand(servo, 0x03, 3, params);
}

void PanTiltSetCompliance(char servo, char CWMargin, char CCWMargin, char CWSlope, char CCWSlope)
{
	char params[5] = {0x1A, CWMargin, CCWMargin, CWSlope, CCWSlope};
	SendServoCommand(servo, 0x03, 5, params);
}

void SendServoCommand(char id, char instruction, char paramCount, char params[])
{      
	USART_WriteByte(servoUSART,0xFF);
	USART_WriteByte(servoUSART,0xFF);
	USART_WriteByte(servoUSART,id); // ID
	char length = paramCount + 2;
	USART_WriteByte(servoUSART,length); // Length

	USART_WriteByte(servoUSART,instruction); // Instruction
	char checksum = 0;
	char i;
	for (i = 0; i<paramCount; i++)
	{
		USART_WriteByte(servoUSART,params[i]); // Params...
		checksum += params[i];
	}

	checksum += id + length + instruction;
	checksum = ~checksum;

	USART_WriteByte(servoUSART,checksum); // Checksum
}

