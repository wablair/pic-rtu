#include <pic18.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "lcd.h"
#include "main.h"
#include "serial.h"
#include "xtend.h"
#include "i2c.h"

config_t config;

int
config_init(void)
{
	unsigned char switches;
	
	switches = read_dip_switches().byte;

	/* if we have loaded valid configuration */
	if (config_read_struct()) 
		lcd_print((unsigned char*) "Config Loaded");
	else { 
		/* otherwise go into software config mode */
		config.address = switches;

		/* do not configure the radio upon startup */
		config.radio_config_on_startup = 0; 

		/* sleep after poll event */
		config.radio_autosleep_time_s = 26;

		/* wake up every second to check for radio activity */
		config.radio_cyclic_sleep_s = 1;
		config.radio_txpower = RADIO_TXPOWER_100mW;
		config_write_struct();

		/*
		command_interpreter();
		 */
	}

	/*
	putst("Modbus address: ");
	putint(config.address);
	putst("\r\n");
	*/
	
	/* Output test */
	/*
	i2c_WriteTo(0x50);
	i2c_PutByte(LCD_SET_CURSOR_POSITION);
	i2c_PutByte(3);
	i2c_PutByte(1);
	i2c_Stop();

	lcd_print((unsigned char *) "Modbus Address");

	lcd_putint(config.address);

	lcd_putch(' ');
	*/
	
	return 0;
}

chartype_t
read_dip_switches(void)
{
	chartype_t switches;
	
	switches.byte = 1;

	/* all Port B pins are inputs */
	/*
	TRISB = 0xFF; 
	switches.byte = PORTB;
	 */
	
	return switches;
}

int
config_write_struct(void)
{
	unsigned int i;

	/* make sure that the struct isn't too big */
	if (sizeof(config_t) > (unsigned int) (EEPROM_SIZE - 2))
		return 0;
	/* indicate that config data has been written */
	eeprom_write(EEPROM_CONFIG_ADDRESS, 0xAA);

	/* for each byte in the config struct */
	for (i = 0; i < sizeof(config_t); i++) {
		/* write out each byte of the config struct */
		eeprom_write(EEPROM_CONFIG_ADDRESS + 1 + i, ((unsigned char*)
		    &config)[i]);
	}

	/* indicate end of config data */
	eeprom_write(EEPROM_CONFIG_ADDRESS + 1 + i, 0xAA);

	return 1;
}

int
config_read_struct(void)
{
	config_t temp;
	unsigned int i;
	unsigned char *c;

	/* check that config data has been written */
	if (eeprom_read(EEPROM_CONFIG_ADDRESS) != 0xAA) {
		return 0;
	}

	/* for each byte in the config struct */
	for (i = 0; sizeof(config_t) < 255 && i < sizeof(config_t); i++) {
		/* get address of next byte in config struct */
		c = ((unsigned char*) &temp) + i;

		/* read next byte from eeprom into config struct */
		*c = eeprom_read(EEPROM_CONFIG_ADDRESS + 1 + i);
	}

	/* check for end of config data */
	if (eeprom_read(EEPROM_CONFIG_ADDRESS + 1 + i) != 0xAA) {
		return 0;
	}

	/* the data passed muster, we can use it now */
	config = temp;

	return 1;
}

int
command_interpreter(void)
{
	unsigned char command[20];
	unsigned char i = 0;
	
	putst("for a list of commands type: ?<enter>\r\n\r\n>");
	lcd_print((unsigned char *)"Enter Command Mode");
	
	for (;;) {
		/* wait for a character to be typed */
		command[i] = getch();

		/* echo the character just typed */
		putch(command[i]);

		if (command[i] == '\n' || command[i] == '\r') {
			/* null terminate the command */
			command[i] = 0;

			if (i && execute_command(command, i) == -1)
				break;

			putst(">");
			i = 0;
		} else {
			i++;
			if (i >= sizeof(command)) {
				putst("\noverflow\r\n>");
				i = 0;
			}
		}
	}
	lcd_print((unsigned char *) "Exit Command Mode");
	return 0;
}	

int
execute_command(unsigned char *command, unsigned char length)
{
	unsigned char i = 0, j = 0;
	unsigned int retval;

	/* search for a space separating the command and any parameter */
	while (i < length) { 
		if (command[i] == ' ') {
			/* null terminate command substring */
			command[i] = 0;

			j = i + 1;

			/* search for the next token */
			while (command[j] == ' ')
				j++;

			break;
		}
		i++;
	}
	
	if (stricmp(command, "address") == 0) {
		/* see if we got a parameter */
		if (j) { 
			/* read the parameter string */
			config.address = atoi(&command[j]);
		}
		putst("Address: ");
		putint(config.address);
		putst("\r\n");
	}
	return 0;
}
