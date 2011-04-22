/*
Interface PIC
Designed by Shane Tolmie Feb 2001.
Microprocessor: Microchip PIC16F87x
Compiled with: Hitech-C v7.87, developed using MPLAB v5.3
Note: all references are to PIC16C7X PDF version of Microchip manual, DS30390E
Overall goal: serial comms using USART to comm port of an ibm pc compatible
computer
*/

#include <pic18.h>
#include <conio.h>
#include <stdio.h>

#include "lcd.h"
#include "main.h"
#include "serial.h"
#include "xtend.h"

void
serial_init(void)
{
/*
relates crystal freq to baud rate - see PIC18Fxxx data sheet
BRGH=1		BRGH=1		BRGH=1	      BRGH=1
Fosc=3.6864MHz Fosc=4MHz	Fosc=8MHz     Fosc=16MHz
Baud	SPBRG  Baud	SPBRG   Baud	SPBRG Baud    SPBRG
1200	191    1200	207.3   1200	415.7 9600    103
2400	95     2400	103.2   2400	207.3 19200   51
4800	47     4800	51.1    4800	103.2 38400   25
9600	23     9600	25.0    9600	51.1  57600   16
19200	11     19200	12.0    19200	25.0  115200  8
38400	5      38400	5.5     38400	12.0
57600	3      57600	3.3     57600	7.7
115200	1      115200	1.2     115200	3.3
*/

	/* Values for 20 Mhz crystal. */
	/*
	#define SPBRG_VALUE	0x81
	#define BRGH_VALUE	1
	 */

	#define SPBRG_VALUE	((PIC_CLK/(16UL * BAUD) - 1))
	#define BRGH_VALUE	1

	SPBRG = SPBRG_VALUE;

	/* data rate for sending */
	BRGH = BRGH_VALUE;	

	/* asynchronous */
	SYNC = 0;

	/* enable serial port pins */
	SPEN = 1;

	/* enable reception */
	CREN = 1;

	/* no effect */
	SREN = 0;

	/* disable tx interrupts */
	TXIE = 0;

	/* disable rx interrupts */
	RCIE = 1;

	/* 8-bit transmission */
	TX9 = 0;

	/* 8-bit reception */
	RX9 = 0;

	/* reset transmitter */
	TXEN = 0;

	/* enable the transmitter */
	TXEN = 1;

	/* No return needed from void functions. */
}

void
putch(unsigned char c)
{
	/* set when register is empty */
	while (!TXIF) {
		clear_usart_errors();
		CLRWDT();
	}

	TXREG = c;

	while (!TXIF);
}

/* gets a character from the serial port without timeout */
unsigned char
getch(void)
{
	while (!RCIF) {
		CLRWDT();
		clear_usart_errors();
	}
	return RCREG;
}

unsigned char
getch_available(void)
{
	if (RCIF) {
		return 1;
	}
	return 0;
}

unsigned char
getch_timeout_bridge(int timeout_us, unsigned char *c)
{
	while (timeout_us--) {
		CLRWDT();
		if (RCIF) {
			*c = RCREG;

			/* indicate that a character was received */
			return 1;
		}
	}

	/* indicate timeout */
	return 0;
}

/* written by Scott Geppert */
int
getst_timeout(unsigned char *buffer, unsigned char length, int timeout_ms)
{
	unsigned char i;
	while (timeout_ms--) {
		if (getch_available()) {
			while (i + 1 < length && getch_timeout_bridge(100,
			    &buffer[i])) {
				i++;
			}

			/* null terminate it just in case */
			buffer[i] = 0;

			clear_usart_errors();

			/* indicate number of characters received */
			return i;
		}
		__delay_ms(1);
	}

	/* indicate timeout */
	return 0;
}

void
clear_usart_errors(void)
{
	unsigned char dummy;
	if (OERR) {
		TXEN = 0;
		TXEN = 1;
		CREN = 0;
		CREN = 1;
	}
	if (FERR) {
		dummy = RCREG;
		TXEN = 0;	
		TXEN = 1;
	}
}

void
putint(unsigned long int value)
{
	unsigned char buffer[10], i = 0;
	
	do {
		/*
		 * accumulate digits from least to most significant (backwards)
		 */
		buffer[i] = BIN_TO_ASCIIHEX((value % 10));

		/* move to next most significant digit */
		value /= 10;

		/*
		 * must do increment separately instead of inline above because
		 * the compiler appears not to support inline increments
		 */
		i++;
	} while (value && i < sizeof(buffer));
	while (i) {
		i--;
		/* loop backwards to print digits out forward */
		putch(buffer[i]);
	}
}

void
putst(register const char *str)
{
	while ((*str) != 0) {
		putch(*str);
		str++;
	}
}
