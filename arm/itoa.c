#include "itoa.h"

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 *
 *
 * Modification by Marshal Horn:
 * Return the length of the string
 *
 * When reading this code, beware of the extensive use of post-increment and 
 * post-decrement operators.
 * They return the value prior to changing it.
 */
uint8_t mitoa(int value, char* result, uint8_t base) {
	// check that the base is valid
	if (base < 2 || base > 36) { 
		*result = '\0'; 
		return 0; 
	}

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;
	uint8_t len = 0;

	do {
		tmp_value = value;
		value /= base;
		ptr[len++]= "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" 
			[35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0){
		ptr[len++] = '-';
	}
	ptr[len] = '\0';

	/* Now reverse the array 
	 * You see that it goes to pointer arithmatic here.  It may be harder to 
	 * read, but it's so elegant that I decided to leave it.
	 */
	ptr = &ptr[len-1];
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return len;
}
