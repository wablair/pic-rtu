#ifndef _config_h_
#define _config_h_

#include "utility.h"

typedef struct config_s {
	unsigned char address;
	char radio_config_on_startup;

	/*
	 * number of seconds the device should sleep after a Modbus poll event
	 */
	unsigned int radio_autosleep_time_s;

	/* 0 for off, positive number determines sleep time */
	char radio_cyclic_sleep_s;

	char radio_txpower;
} config_t;

extern config_t config;

chartype_t	 read_dip_switches(void);
int		 config_init(void);

/* write the config struct out to EEPROM */
int		 config_write_struct(void);

/* read from EEPROM into the config struct */
int		 config_read_struct(void);

int		 command_interpreter(void);
int		 execute_command(unsigned char *, unsigned char);

#endif /* !_config_h_ */
