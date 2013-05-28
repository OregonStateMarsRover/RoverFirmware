#ifndef HAVE_PANTILT_H
#define HAVE_PANTILT_H

#include "USART.h"

#define PROBE_SERVO 0x10
#define WRIST_SERVO 0x11
#define SCOOP_SERVO 0x12

#define WRIST_MIN 0x260
#define WRIST_MAX 0x3D0

void InitServos(void );
void SendServoCommand(char id, char instruction, char paramCount, char params[]);
//void PanTiltHandlePacket(CommPacket * pkt);
void PanTiltSetPosition(char servo, unsigned short position);
void PanTiltSetCompliance(char servo, char CWMargin, char CCWMargin, char CWSlope, char CCWSlope);

USART *servoUSART;

#endif
