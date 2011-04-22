#include <pic18.h>

#include "delay.h"
#include "i2c.h"

unsigned char delayus_variable;
/*
 *	I2C functions for HI-TECH PIC C - master mode only
 *
 * 	TIMING - see Philips document: THE I2C-BUS SPECIFICATION
 */

/*
 * 	Send stop condition
 * 	  - data low-high while clock high
 */
void
i2c_Stop(void)
{
	/* don't assume SCL is high on entry */

	/* ensure data is low first */
	SDA_LOW();
	SCL_HIGH();

	DelayUs(I2C_TM_DATA_SU);

	/* float clock high */
	SCL_DIR = I2C_INPUT;

	DelayUs(I2C_TM_STOP_SU);

	/* the low->high data transistion */
	SDA_HIGH();

	/* bus free time before next start */
	DelayUs(I2C_TM_BUS_FREE);

	/* float data high */
	SDA_DIR = I2C_INPUT;

	return;
}

/*
 * 	Send (re)start condition
 * 	  - ensure data is high then issue a start condition
 * 	  - see also i2c_Start() macro
 */
void
i2c_Restart(void)
{
	SDA = 0;
	SCL = 0;
	/* ensure clock is low */
	SCL_LOW();

	/* ensure data is high */
	SDA_HIGH();

	DelayUs(I2C_TM_DATA_SU);

	/* clock pulse high */
	SCL_DIR = I2C_INPUT;

	DelayUs(I2C_TM_SCL_HIGH);

	/* the high->low transition */
	SDA_LOW();

	DelayUs(I2C_TM_START_HD);

	return;
}

/*
 * 	Send a byte to the slave
 * 	  - returns true on error
 */
unsigned char
i2c_SendByte(unsigned char byte)
{
	signed char i;

	for (i = 7; i >= 0; i--) {

		/* drive clock low */
		SCL_LOW();

		/* data hold time = 0, send data now */
		SDA_DIR = ((byte>>i)&0x01);

		/* bit to send */
		if ((byte >> i) & 0x01) {
			SDA_HIGH();
		} else {
			SDA_LOW();
		}
		DelayUs(I2C_TM_DATA_SU);

		/* float clock high */
		SCL_DIR = I2C_INPUT;

		/* wait for clock release */
		if (i2c_WaitForSCL())
			/* bus error */
			return TRUE;

		/* clock high time */
		DelayUs(I2C_TM_SCL_HIGH);
	}

	return FALSE;
}

/*
 * 	send an address and data direction to the slave
 * 	  - 7-bit address (lsb ignored)
 * 	  - direction (FALSE = write )
 */
unsigned char
i2c_SendAddress(unsigned char address, unsigned char rw)
{
	return i2c_SendByte(address | (rw ? 1 : 0));
}

/*
 * 	Check for an acknowledge
 * 	  - returns ack or ~ack, or ERROR if a bus error
 */
signed char
i2c_ReadAcknowledge(void)
{
	unsigned char ack;

	/* make clock is low */
	SCL_LOW();

	/* disable data line - listen for ack */
	SDA_DIR = I2C_INPUT;

	/* SCL low to data out valid */
	DelayUs(I2C_TM_SCL_TO_DATA);

	/* float clock high */
	SCL_DIR = I2C_INPUT;

	DelayUs(I2C_TM_DATA_SU);

	/* read the acknowledge */
	ack = SDA;

	/* wait for slave to release clock line after processing byte */
	if (i2c_WaitForSCL())
		return I2C_ERROR;

	return ack;
}

/*
 * 	Read a byte from the slave
 * 	  - returns the byte, or I2C_ERROR if a bus error
 */
int
i2c_ReadByte(void)
{
	unsigned char i;
	unsigned char byte = 0;

	for (i = 0; i < 8; i++) {

		/* drive clock low */
		SCL_LOW();

		/* min clock low period */
		DelayUs(I2C_TM_SCL_LOW);

		/* release data line */
		SDA_DIR = I2C_INPUT;

		/* float clock high */
		SCL_DIR = I2C_INPUT;

		if (i2c_WaitForSCL())
			return I2C_ERROR;

		DelayUs(I2C_TM_SCL_HIGH);

		/* read the next bit */
		byte = byte << 1;

		byte |= SDA;
	}
	return (int) byte;
}

/*
 * 	Send an (~)acknowledge to the slave
 * 	  - status of I2C_LAST implies this is the last byte to be sent
 */
void
i2c_SendAcknowledge(unsigned char status)
{
	SCL_LOW();
	if (status & 0x01) {
		/* drive line low -> more to come */
		SDA_LOW();
	} else {
		SDA_HIGH();
	}
	DelayUs(I2C_TM_DATA_SU);

	/* float clock high */
	SCL_DIR = I2C_INPUT;

	DelayUs(I2C_TM_SCL_HIGH);
	return;
}

/*
 * 	Send a byte to the slave and acknowledges the transfer
 * 	  - returns I2C_ERROR, ack or ~ack
 */
signed char
i2c_PutByte(unsigned char data)
{
	if (i2c_SendByte(data))
		return I2C_ERROR;

	/* returns ack, ~ack */
	return i2c_ReadAcknowledge();
}

/*
 * 	Get a byte from the slave and acknowledges the transfer
 * 	  - returns true on I2C_ERROR or byte
 */
int
i2c_GetByte(unsigned char more)
{
	int byte;

	if ((byte = i2c_ReadByte()) == I2C_ERROR)
		return I2C_ERROR;

	i2c_SendAcknowledge(more);

	return byte;
}

/*
 * 	Send an array of bytes to the slave and acknowledges the transfer
 * 	  - returns number of bytes not successfully transmitted
 */
int
i2c_PutString(const unsigned char *str, unsigned char length) {
	signed char error;

	while (length) {
		if ((error = i2c_PutByte(*str)) == I2C_ERROR)
			/* bus error */
			return -(int) length;
		else
			if (error)
				/* non acknowledge */
				return (int) length;
		str++;
		length--;
	}

	/* everything OK */
	return FALSE;
}

/*
 * 	Reads number bytes from the slave, stores them at str and acknowledges
 * 	the transfer
 * 	  - returns number of bytes not successfully read in
 */
unsigned char
i2c_GetString(unsigned char *str, unsigned char number)
{
	int byte;

	while (number) {
		if ((byte = i2c_GetByte(number-1)) == I2C_ERROR)
			/* bus error */
			return number;
		else
			*str = (unsigned char) byte;
		str++;
		number--;
	}

	/* everything OK */
	return FALSE;
}

/*
 * 	Opens communication with a device at address. mode
 * 	indicates I2C_READ or I2C_WRITE.
 * 	  - returns TRUE if address is not acknowledged
 */
unsigned char
i2c_Open(unsigned char address, unsigned char mode)
{
	i2c_Start();
	i2c_SendAddress(address, mode);
	if (i2c_ReadAcknowledge()) 
		return TRUE;

	return FALSE;
}

/*
 * 	wait for the clock line to be released by slow slaves
 * 	  - returns TRUE if SCL was not released after the
 * 	    time out period.
 * 	  - returns FALSE if and when SCL released
 */
unsigned char
i2c_WaitForSCL(void)
{
	/* SCL_DIR should be input here */
	
	if (!SCL) {
		DelayUs(I2C_TM_SCL_TMO);
		/* if the clock is still low -> bus error */
		if (!SCL)
			return TRUE;
	}
	return FALSE;
}

void
i2c_Free(void)
{
	unsigned char ucI;

	SDA_DIR = I2C_INPUT;
	for (ucI = 0; ucI != 9; ucI++) {
		SCL_HIGH();
		DelayUs(5);
		SCL_LOW();
		DelayUs(5);
	}
}

unsigned char
i2c_read(unsigned char ucAdr)
{
	unsigned char ucDat;

	if (i2c_ReadFrom(ucAdr) == 0) {
		ucDat=i2c_GetByte(I2C_MORE);
		i2c_Stop();
	}

	return ucDat;
}
