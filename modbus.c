#include <pic18.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "adcon.h"
#include "crc.h"
#include "lcd.h"
#include "main.h"
#include "modbus.h"
#include "serial.h"

/*
 * Delay times that are acceptable specified by MODBUS spec based on baud rate.
 * In ms.
 */

/* minimum time between messages */
unsigned int msginterval = 2920;

/* maximum time between characters */
unsigned int charinterval = 1250;

/* difference between the two previous times */
unsigned int msgchardiff = 1670;

void
modbus_field_request(struct ad_data *data)
{
	int modbus_decode_return;
	unsigned char empty;
	unsigned char message[MAX_MESSAGE_LENGTH];
	unsigned char x;

	x = 0;

	while (x < MAX_MESSAGE_LENGTH && getch_timeout_bridge(CHAR_INTERVAL,
	    &message[x]))
		x++;
	
	clear_usart_errors();

	modbus_decode_return = modbus_decode(message, x, data);
	
	/*
	 * If there is an error, let's clear out whatever remaining input
	 * we may get.
	 */
	if (modbus_decode_return)
		while (getch_timeout_bridge(MSG_INTERVAL, &empty));
	
	clear_usart_errors();
		
	/* No return needed from void function. */
}

int
modbus_decode(unsigned char *message, const unsigned char length,
    struct ad_data *data)
{
	unsigned char function = '\0';

	unsigned short response_crc = 0xFFFF;
	unsigned short request_crc_calculated = 0;

	if (length < 2)
		return 1;

	function = message[MODBUS_REQ_FUNC_OFFSET];

	if (message[MODBUS_REQ_ADDR_OFFSET] != MODBUS_ADDR)
		return 1;

	request_crc_calculated = CRC16(message, length - 2);

	if (request_crc_calculated !=
	    ((message[length - 1] << 8) | message[length - 2]))
		return 1;

	switch (function) {
	case FUNC_READ_COILS:
		modbus_exception_response(function, ILLEGAL_FUNCTION);
		break;
	case FUNC_READ_DISCRETE_INPUTS:
		modbus_exception_response(function, ILLEGAL_FUNCTION);
		break;
	case FUNC_READ_HOLDING_REGISTERS:
		modbus_read_holding_registers(message, length, data->scaled,
		    data->soil_moisture, data->temperature);
		break;
	case FUNC_READ_INPUT_REGISTERS:
		modbus_read_input_registers(message, length, data->raw,
		    data->soil_moisture, data->temperature);
		break;
	default:
		modbus_exception_response(function, ILLEGAL_FUNCTION);
		break;
	}

	return 0;
}

void
modbus_exception_response(unsigned char function, unsigned char code)
{
	unsigned int x;
	unsigned char response[] = {MODBUS_ADDR, 0x80, 0x00, 0x00,
	    0x00};
	unsigned short response_crc;

	response[1] += function;
	response[2] = code;

	response_crc = CRC16(response, 2);
	response[2] = (unsigned char) response_crc;
	response[3] = (unsigned char) (response_crc >> 8);

	for (x = 0; x < 4; x++)
		putch(response[x]);

	/* No return needed from void function. */
}

int
modbus_valid_register_read(unsigned char function,
    unsigned int starting_address, unsigned int quantity)
{	

	if (quantity == 0 || quantity > 0x007D) {
		modbus_exception_response(function, ILLEGAL_DATA_VALUE);
		return 0;
	}

	if ((long) starting_address + (long) quantity > 0x0000FFFF) {
		modbus_exception_response(function, ILLEGAL_DATA_ADDRESS);
		return 0;
	}

	return 1;
}

int
modbus_read_coils(unsigned char *message,
    const unsigned char length, unsigned int data[])
{
	return 0;
}

int
modbus_read_discrete_inputs(unsigned char *message,
    const unsigned char length, unsigned int data[])
{
	return 0;
}

int
modbus_read_holding_registers(unsigned char *message,
    const unsigned char length, unsigned int data[], float moisture[],
    float temperature[])
{
	return modbus_read_input_registers(message, length, data, moisture,
	    temperature);
}

int
modbus_read_input_registers(unsigned char *message, const unsigned char length,
    unsigned int analog[], float moisture[], float temperature[])
{
	static unsigned char count = '\0';
	unsigned char function = '\0';
	unsigned int quantity = 0;
	unsigned int starting_address = 0;
	unsigned int x = 0;

	unsigned short int response_crc = 0xFFFF;

	function = message[MODBUS_REQ_FUNC_OFFSET];

	quantity = (message[MODBUS_REQ_QTY_OFFSET] << 8) |
	    message[MODBUS_REQ_QTY_OFFSET + 1];

	starting_address = (message[MODBUS_REQ_START_OFFSET] << 8) |
	    message[MODBUS_REQ_START_OFFSET + 1];

	if (!modbus_valid_register_read(function, starting_address, quantity))
		return 1;

	putch(MODBUS_ADDR);
	response_crc = crc16_single(MODBUS_ADDR, response_crc);

	putch(function);
	response_crc = crc16_single(function, response_crc);

	putch(quantity * 2);
	response_crc = crc16_single(quantity * 2, response_crc);

	for (x = 0; x < quantity; x++) {
		if (starting_address < NUM_AIN) {
			putch((unsigned char) (analog[starting_address] >> 8));
			response_crc = crc16_single((unsigned char)
			    (analog[starting_address] >> 8), response_crc);

			putch((unsigned char) (analog[starting_address]));
			response_crc = crc16_single((unsigned char)
			    (analog[starting_address]), response_crc);
		} else if (starting_address >= NUM_AIN &&
		    starting_address < NUM_AIN + NUM_AQUASPY * 2) {
			unsigned int value = 0;

			if (((starting_address - NUM_AIN) % 2) == 0)
				value = (unsigned int)
				    (moisture[(starting_address - NUM_AIN) / 2]
				    * 10);
			else
				value = (unsigned int)
				    (temperature[(starting_address - NUM_AIN) /
				    2] * 10);

			putch((unsigned char) (value >> 8));
			response_crc = crc16_single((unsigned char)
			    (value >> 8), response_crc);

			putch((unsigned char) (value));
			response_crc = crc16_single((unsigned char)
			    (value), response_crc);
		} else {
			putch('\0');
			response_crc = crc16_single('\0', response_crc);

			putch(++count);
			response_crc = crc16_single(count, response_crc);
		}
		starting_address++;
	}

	putch((unsigned char) response_crc);
	putch((unsigned char) (response_crc >> 8));

	return 0;
}
