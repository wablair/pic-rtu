#include "crc.h"

unsigned short
CRC16(unsigned char *msg, unsigned short length)
{
	unsigned short crc = 0xFFFF;
	unsigned short i, j;
	unsigned short lsb;

	for (i = 0; i < length; i++) {

		/* XOR the CRC with the current byte */
		crc ^= msg[i]; 

		/* make eight shifts on the CRC */
		for (j = 0; j < 8; j++) {
			/* extract the least-significant bit */
			lsb = crc & 0x1;

			/* perform the shift */
			crc = crc >> 1;

			/* if the lsb was '1' */
			if (lsb)
				/* XOR with the polynomial constant */
				crc ^= 0xA001;
		}
	}

	return crc;
}


/*
 * For calculating CRC16 byte by byte.  First time called crc needs
 * to be 0xFFFF. 
 */
unsigned short
crc16_single(unsigned char msg, unsigned short crc)
{
	unsigned short i, j;
	unsigned short lsb;

	/* XOR the CRC with the current byte */
	crc ^= msg; 

	/* make eight shifts on the CRC */
	for (j = 0; j < 8; j++) {
		/* extract the least-significant bit */
		lsb = crc & 0x1;

		/* perform the shift */
		crc = crc >> 1;

		/* if the lsb was '1' */
		if (lsb)
			/* XOR with the polynomial constant */
			crc ^= 0xA001;
	}

	return crc;
}
