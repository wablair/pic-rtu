#include <conio.h>
#include <ctype.h>
#include <pic18.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "adcon.h" 
#include "lcd.h"
#include "main.h"
#include "modbus.h"
#include "xtend.h"
#include "i2c.h"
#include "sdi-12.h"
#include "serial.h"

#if PIC_CLK < 8000000
	__CONFIG(1, XT);
#else
	__CONFIG(1, HS);
#endif

__CONFIG(2, BORDIS & PWRTEN & WDTDIS);
__CONFIG(4, LVPDIS);


/* Not enough room on the stack to declare this in main. */
struct ad_data sensor_data;

void
main(void)
{
	unsigned int analog[NUM_AIN];
	unsigned int timer_count = 0;
	unsigned short int x;
	unsigned short int radio_asleep = 0;
	unsigned short int read = 0;

	serial_init();
	radio_init();
	adcon_init();

	WRITETIMER0(0);
	INTCON = 0x00;
	T0CON = 0x87;

	get_aquaspy_sensor_data();

	for (;;) {
		if (radio_asleep && read != timer_count) {
			get_aquaspy_sensor_data();
			read = timer_count;
		}


		if (TMR0IF) {
			timer_count++;
			TMR0IF = 0;
		}

		if (timer_count >= 4 && !radio_asleep) {
			RADIO_CONTROL_PORT |= RADIO_SLEEP_PIN;
			timer_count = 0;
			radio_asleep = 1;
		}

		if (timer_count >= 32 && radio_asleep) {
			RADIO_CONTROL_PORT &= ~RADIO_SLEEP_PIN;
			timer_count = 0;
			radio_asleep = 0;
		}


		for (x = 0; x < NUM_AIN; x++)
			sensor_data.raw[x] = get_conversion(x);

		average_data(&sensor_data);
		
		rcif_poll(0xFFFF, &sensor_data);
	}
}

int
rcif_poll(unsigned int delay, struct ad_data *data)
{
	unsigned int x;

	radio_rts_assert();

	__delay_ms(1);

	for (x = 0; x < delay; x++)
		if (RCIF)
			modbus_field_request(data);

	radio_rts_de_assert();

	return 0;
}	

int
get_aquaspy_sensor_data()
{
	int addr = 0;
	int tries = 0;

	if (NUM_AQUASPY < 1 || NUM_AQUASPY > MAX_AQUASPY)
		return 1;

	for (addr = 0; addr < NUM_AQUASPY; addr++) {
		sdi12_start_measurement(addr);

		for (tries = 0; tries < AQUASPY_RETRIES; tries++) {
			sensor_data.soil_moisture[addr] =
			    sdi12_send_data(addr, AQUASPY_SOIL_MOISTURE);

			if (sensor_data.soil_moisture[addr] != 0.0)
				break;

		}

		for (tries = 0; tries < AQUASPY_RETRIES; tries++) {
			sensor_data.temperature[addr] =
			    sdi12_send_data(addr, AQUASPY_TEMPERATURE);

			if (sensor_data.temperature[addr] != 0.0)
				break;
		}
	}

	return 0;
}
