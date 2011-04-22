#ifndef	_I2C_H_
#define _I2C_H_

/*
 *	SDA (data) and SCL (clock) bits
 *	
 *	Special note!!!
 *	
 *	If the clock and data lines are in the same port, you will need
 *	to beware of the Read/Modify/Write issue in the PIC - since
 *	a bit set or clear on any one bit in a port will read and write
 *	back all other bits, any bits configured as input which 
 */


/* Uncomment the next line to use the PIC's SSP Module*/
#define I2C_MODULE 1


#ifdef I2C_MODULE
/* I2C module uses PORT C */

/* clock on port C bit 2 */
#define SCL             RC3             

#define SCL_DIR		TRISC3

/* data on port C bit 1 */
#define SDA		RC4

#define SDA_DIR         TRISC4
#define I2CTRIS		TRISC

/* I2C firmware controlled Master Mode (slave idle) */
#define MASTER_MODE     0B1011

#define SSPMode(val)   SSPCON1 &=0xF0; SSPCON1 |=(val & 0xf)

#else

/* Change port as required - defaults to port b */

/* clock on port B bit 2 */
#define	SCL		RB2

#define	SCL_DIR		TRISB2

/* data on port B bit 1 */
#define SDA             RB1

#define	SDA_DIR		TRISB1
#define I2CTRIS		TRISB

#endif /* I2C_MODULE */



#define M_SDA_INP	0x02
#define M_SDA_OUT	0xFD
#define M_SCL_INP	0x04
#define M_SCL_OUT	0xFB

/* data direction input */
#define I2C_INPUT	1

/* data direction output */
#define I2C_OUTPUT	0

/* read bit used with address */
#define I2C_READ	0x01

/* write bit used with address */
#define I2C_WRITE	0x00

#define FALSE		0
#define TRUE		!FALSE

#define I2C_ERROR	(-1)

/* SendAck: no more bytes to send */
#define I2C_LAST	FALSE

/* SendAck: more bytes to send */
#define I2C_MORE	TRUE

#define i2c_Start()		i2c_Restart()
#define i2c_WriteTo(address)	i2c_Open((address), I2C_WRITE)
#define i2c_ReadFrom(address)	i2c_Open((address), I2C_READ)

#ifdef I2C_MODULE

#define SCL_HIGH() SCL_DIR = I2C_INPUT
#define SCL_LOW()  SCL_DIR = I2C_OUTPUT
#define SDA_HIGH() SDA_DIR = I2C_INPUT
#define SDA_LOW()  SDA_DIR = I2C_OUTPUT

#else

#define SCL_HIGH() SCL = 1; SCL_DIR = I2C_OUTPUT
#define SCL_LOW()  SCL = 0; SCL_DIR = I2C_OUTPUT
#define SDA_HIGH() SDA = 1; SDA_DIR = I2C_OUTPUT
#define SDA_LOW()  SDA = 0; SDA_DIR = I2C_OUTPUT

#endif

/*
 * Timings for the i2c bus. Times are rounded up to the nearest
 * micro second.
 */

#define I2C_TM_BUS_FREE		5
#define	I2C_TM_START_SU		5
#define I2C_TM_START_HD		4
#define I2C_TM_SCL_LOW		5
#define	I2C_TM_SCL_HIGH		4
#define I2C_TM_DATA_SU		1
#define I2C_TM_DATA_HD          0
#define I2C_TM_SCL_TO_DATA	4	/* SCL low to data valid */
#define	I2C_TM_STOP_SU		4
#define I2C_TM_SCL_TMO		10	/* clock time out */

extern signed char	 i2c_ReadAcknowledge(void);
extern unsigned char	 i2c_SendAddress(unsigned char, unsigned char);
extern unsigned char	 i2c_SendByte(unsigned char);
extern int		 i2c_ReadByte(void);
extern void		 i2c_Restart(void);
extern void		 i2c_Stop(void);
extern void		 i2c_SendAcknowledge(unsigned char);
extern signed char	 i2c_PutByte(unsigned char);
extern int		 i2c_GetByte(unsigned char);
extern unsigned char	 i2c_Open(unsigned char, unsigned char);
extern unsigned char	 i2c_GetString(unsigned char *, unsigned char);
extern int		 i2c_PutString(const unsigned char *, unsigned char);
extern unsigned char	 i2c_WaitForSCL(void);
extern void		 i2c_Free(void);

#endif /* _I2C_H_ */
