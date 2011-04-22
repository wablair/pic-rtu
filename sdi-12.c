#include <htc.h>
#include <pic18.h>
#include <stdlib.h>
#include <string.h>

#include "delay.h"
#include "main.h"
#include "sdi-12.h"
#include "serial.h"

unsigned char buf[1024];

void
sdi12_send_break(void)
{

	SDI12_PORT_DIRECTION = OUTPUT;

	SDI12_SPACING();

	__delay_ms(12);

	SDI12_MARKING();
	
	__delay_ms(9);

	/* No return needed for void function. */

}

int
sdi12_change_address(int old_address, int new_address)
{
	if (old_address < 0 || old_address > 9 || new_address < 0 ||
	    new_address > 9)
		return 1;

	sdi12_send_break();
	sdi12_putchar((char) (old_address + '0'));
	sdi12_putchar('A');
	sdi12_putchar((char) (new_address + '0'));
	sdi12_putchar('!');

	return 0;
}

int
sdi12_address_query(void)
{
	char ret[3];
	sdi12_send_break();
	sdi12_putchar('?');
	sdi12_putchar('!');

	sdi12_gets(ret, 3);

	return ret[0];
}

float
sdi12_send_data(int address, int reading)
{
	unsigned short int x = 0;
	char ret[9];

	if (address < 0 || address > 9 || reading < 0 || reading > 4)
		return NULL;

	memset(ret, '\0', 9);

	sdi12_send_break();

	sdi12_putchar((char) (address + '0'));
	sdi12_putchar('D');
	sdi12_putchar((char) (reading + '0'));
	sdi12_putchar('!');

	sdi12_gets(ret, 8);

	return atof(ret + 1);
}

int
sdi12_start_measurement(int address)
{
	if (address < 0 || address > 9)
		return -1;

	sdi12_send_break();

	sdi12_putchar(address + '0');
	sdi12_putchar('M');
	sdi12_putchar('!');

	return 0;
}



void
sdi12_putchar(char c)
{
	unsigned short int parity = 0;
	unsigned short x = 0;

	SDI12_PORT_DIRECTION = OUTPUT;

	/* Start bit. */
	SDI12_SPACING();

	__delay_us(SDI12_DELAY);

	/* Output character with least significant bit first. */

	for (x = 0; x < 7; x++) {
		if (c & 0x01) {
			SDI12_MARKING();
		} else {
			SDI12_SPACING();
		}

		__delay_us(SDI12_DELAY);

		parity ^= c & 0x01;

		c >>= 1;
	}

	/* Even parity. */

	if (parity) {
		SDI12_MARKING();
	} else {
		SDI12_SPACING();
	}

	__delay_us(SDI12_DELAY);

	/* Stop bit. */
	SDI12_MARKING();

	__delay_us(SDI12_DELAY);

	/* No return needed for void function. */
}

char
sdi12_getchar(void)
{
	unsigned int x = 0;
	char c = '\0';

	SDI12_MARKING();
	SDI12_PORT_DIRECTION = INPUT;

	/*
	x = 8;

	while (!SDI12_PORT) {
		while (!SDI12_PORT);
		__delay_us(SDI12_DELAY_HALF - 9);
	}

	while (x--) {
		__delay_us(SDI12_DELAY - 14);
		CARRY = SDI12_PORT ? 1 : 0;
		c = c >> 1 | c << 7;
	}
	 */

	x = 0;

	while (x < 1500) {
		if (SDI12_PORT == 1)
			break;
		__delay_us(10);
		x++;
	}

	x = 0;

	while (x < 11) {
		buf[x] = SDI12_PORT;
		__delay_us(SDI12_DELAY - 14);
		x++;
	}

	c = (~buf[1] & 0x01) | ((~buf[2] & 0x01) << 1) | ((~buf[3] & 0x01) << 2)
	    | ((~buf[4] & 0x01) << 3) | ((~buf[5] & 0x01) << 4) | ((~buf[6]
	    & 0x01) << 5) | ((~buf[7] & 0x01) << 6);

	return c;
}

unsigned short int
sdi12_gets(unsigned char *s, unsigned short int length)
{
	unsigned int x = 0;
	char c = '\0';
	unsigned int bits = length * 11;
	unsigned short int count = 0;

	if (bits > 1024)
		return NULL;

	SDI12_MARKING();
	SDI12_PORT_DIRECTION = INPUT;

	x = 0;

	while (x < 1500) {
		if (SDI12_PORT == 1)
			break;
		__delay_us(10);
		x++;
	}

	x = 0;

	while (x < bits) {
		buf[x] = SDI12_PORT;
		__delay_us(SDI12_DELAY - 14);
		x++;
	}

	for (x = 0; x < bits; x += 11) {
		c = (~buf[x + 1] & 0x01) | ((~buf[x + 2] & 0x01) << 1) |
		    ((~buf[x + 3] & 0x01) << 2) | ((~buf[x + 4] & 0x01) << 3) |
		    ((~buf[x + 5] & 0x01) << 4) | ((~buf[x + 6] & 0x01) << 5) |
		    ((~buf[x + 7] & 0x01) << 6);


		if (count < length)
			s[count++] = c;
	}

	return count;
}
