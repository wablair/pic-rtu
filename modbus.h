#ifndef _modbus_h_
#define _modbus_h_

#include "adcon.h"

#ifndef MODBUS_ADDR
#define MODBUS_ADDR				1
#endif

#define	MAX_MESSAGE_LENGTH			20

/* Minimum time between messages. */
#define MSG_INTERVAL				2920

/* Minimum time between characters. */
#define CHAR_INTERVAL				1250

/* Difference between the two. */
#define MSG_CHAR_DIFF				(MSG_INTERVAL - CHAR_INTERVAL)

/* 0k */
#define FUNC_READ_COILS				0x01

/* 10k */
#define FUNC_READ_DISCRETE_INPUTS		0x02

/* 40k */
#define FUNC_READ_HOLDING_REGISTERS		0x03

/* 30k */
#define FUNC_READ_INPUT_REGISTERS		0x04

#define FUNC_WRITE_SINGLE_COIL			0x05
#define FUNC_WRITE_SINGLE_REGISTER		0x06
#define FUNC_WRITE_MULTIPLE_COILS		0x0F
#define FUNC_WRITE_MULTIPLE_REGISTERS		0x10
#define FUNC_READ_FILE_RECORD			0x14
#define FUNC_WRITE_FILE_RECORD			0x15
#define FUNC_MASK_WRITE_REGISTER		0x16
#define FUNC_READWRITE_MULTIPLE_REGISTERS	0x17
#define FUNC_READ_DEVICE_IDENTIFICATION		0x2B

#define MODBUS_ADDR_OFFSET			0
#define MODBUS_FUNC_OFFSET			1
#define MODBUS_SIZE_OFFSET			2
#define MODBUS_DATA_OFFSET			4

#define	MODBUS_REQ_ADDR_OFFSET			0
#define	MODBUS_REQ_FUNC_OFFSET			1
#define	MODBUS_REQ_START_OFFSET			2
#define	MODBUS_REQ_QTY_OFFSET			4

/* Read-Only Registers (30k) */

/* 30 0001 */
#define REGISTER_AI0			0

/* 30 0002 */
#define REGISTER_AI1			1

/* 30 0003 */
#define REGISTER_AI2			2

/* 30 0004 */
#define REGISTER_AI3			3

/* 30 0005 */
#define REGISTER_BATTERY_VOLT		4

/* 30 0100 */
#define REGISTER_RADIO_VOLTAGE		99

/* I/O Registers (40k) */

/* 40 0001 */
#define REGISTER_COUNTER1		0

/* 40 0002 */
#define REGISTER_COUNTER2		1

/* 40 0010 */
#define REGISTER_RADIO_POWER		9

/* 40 0011 - number of seconds to automatically sleep after polling event */
#define REGISTER_RADIO_AUTOSLEEP_S	10 

#define HTONS(A)	((((unsigned short) (A) & 0xFF00) >> 8) | \
			    (((unsigned short) (A) & 0x00FF) << 8))

/* MODBUS Exception Codes */
#define	ILLEGAL_FUNCTION		0x01
#define	ILLEGAL_DATA_ADDRESS		0x02
#define	ILLEGAL_DATA_VALUE		0x03
#define	SLAVE_DEVICE_FAILURE		0x04
#define	ACKNOWLEDGE			0x05
#define	SLAVE_DEVICE_BUSY		0x06
#define	MEMORY_PARITY_ERROR		0x08
#define GATEWAY_PATH_UNAVAILABLE	0x0A
#define	GATEWAY_TARGET_FAILED_RESPONSE	0x0B

void	 modbus_exception_response(unsigned char, unsigned char);
void	 modbus_field_request(struct ad_data *);
int	 modbus_decode(unsigned char *, const unsigned char, struct ad_data *);
int	 modbus_read_coils(unsigned char *, const unsigned char,
	     unsigned int []);
int	 modbus_read_discrete_inputs(unsigned char *, const unsigned char,
	     unsigned int []);
int	 modbus_read_holding_registers(unsigned char *, const unsigned char,
	     unsigned int [], float [], float []);
int	 modbus_read_input_registers(unsigned char *, const unsigned char,
	     unsigned int [], float [], float []);
int	 modbus_valid_register_read(unsigned char, unsigned int, unsigned int);

#endif /* !_modbus_h_ */
