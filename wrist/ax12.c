#include "common.h"
#include "ax12.h"

void InitServos( void )
{
	//PanTiltSetCompliance(1,1,1,8,8);
	//_delay_ms(50);
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
	char start[5];
	start[0] = 0xff;
	start[1] = 0xff;
	start[2] = id;
	start[3] = paramCount + 2;	//length
	start[4] = instruction;

	USART_Write( ((USART* )wrist.ax12), start, 5 );
	USART_Write( ((USART *)wrist.ax12), params, paramCount );

	char checksum = id + paramCount + 2 + instruction;
	char i;
	for (i = 0; i<paramCount; i++)
	{
		checksum += params[(int)i];
	}
	checksum = ~checksum;

	USART_WriteByte((USART*)wrist.ax12,checksum); // Checksum
} 

