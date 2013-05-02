/* Sabertooth.h
*  Authors: Mike Fortner, Chris Newman
*/

#ifndef sabertooth_h
#define sabertooth_h

#include "common.h"

//send initialization commands to motor driver.  timeout(watchdog timer) is set.  Could set deadband or ramping as well.
void sabertooth_init(struct USART *p);

//calculates checksum and sends the command over USART to the sabertooth
void send_command(uint8_t opcode, uint8_t data);

/* send drive motor forward/backwards command, 'speed' represents a value between -127 and 127.
 * However, it accepts a 16-bit integer so you don't have to deal with too-large numbers every time.
 */
void shoulder_set(int16_t speed);
void elbow_set(int16_t speed);

#endif
