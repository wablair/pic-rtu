#include <stdio.h>

#include "modbus_ascii.h"

unsigned char
low_nibble_to_char(unsigned char input)
{
	input &= 0x0F;
	if (input < 10) {
		input += '0';
		return input;
	}
	input -= 10;
	input += 'A';
	return input;
}

unsigned char
high_nibble_to_char(unsigned char input)
{
	return low_nibble_to_char(input >> 4); 
}
