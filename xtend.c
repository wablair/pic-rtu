#include <pic18.h>
#include <math.h>

#include "lcd.h"
#include "main.h"
#include "serial.h"
#include "xtend.h"

int
radio_init(void)
{
	/* set SHDN and CONFIG pin as an output */
	RADIO_DIRECTION_REGISTER = RADIO_DIRECTION_REGISTER & (0xFF ^
	    (RADIO_SHDN_PIN | RADIO_SLEEP_PIN | RADIO_RTS_PIN));

	/*
	 * turn radio on and set config pin high to avoid going into config
	 * mode
	 */
	RADIO_CONTROL_PORT = RADIO_CONTROL_PORT | RADIO_SHDN_PIN |
	    RADIO_RTS_PIN;

	RADIO_CONTROL_PORT &= ~RADIO_SLEEP_PIN;

	return 0;
}

int
radio_on(void)
{
	/*
	lcd_print((unsigned char*) "Radio On");
	 */

	/* set SHDN and CONFIG pin as an output */
	RADIO_DIRECTION_REGISTER = RADIO_DIRECTION_REGISTER & (0xFF ^
	    (RADIO_SHDN_PIN | RADIO_CONFIG_PIN));

	/* wake radio up */
	RADIO_CONTROL_PORT = RADIO_CONTROL_PORT | RADIO_SHDN_PIN;

	/* radio takes < 100ms to wake up (Manual P.14) */
	__delay_ms(RADIO_WAKEUP_TIME_MS);

	return 0;
}

int
radio_off(void)
{
	/*
	lcd_print((unsigned char*) "Radio Off");
	 */

	/* set SHDN and CONFIG pin as an output */
	RADIO_DIRECTION_REGISTER = RADIO_DIRECTION_REGISTER & (0xFF ^
	    (RADIO_SHDN_PIN | RADIO_CONFIG_PIN));

	/* initiate radio shutdown mode */
	RADIO_CONTROL_PORT = RADIO_CONTROL_PORT & (0xFF ^ RADIO_SHDN_PIN);

	return 0;
}

int
radio_sleep(unsigned int seconds)
{
	if (seconds == 0)
		return 0;

	/* wait a bit to let the radio finish transmitting */
	__delay_ms(1000 * RADIO_ALLOW_TX_WAIT_S);

	/* we already waited this many seconds */
	seconds -= RADIO_ALLOW_TX_WAIT_S;

	radio_off();

	/* turn LED off */
	IOR44_LED_PORT = IOR44_LED_PORT & (0xFF ^ IOR44_LED_PIN);
	
	while (seconds--)
		/* wait the prescribed number of seconds */
		__delay_ms(1000);

	radio_on();

	return 0;
}

unsigned int
AT_command(unsigned char *buffer)
{
	unsigned char response[AT_COMMAND_RESPONSE_LENGTH];
	unsigned int value = 0;
	char i;

	/* this method allows us to program the radio out-of-band */
#if (RADIO_COMMAND_MODE == RADIO_COMMAND_MODE_USE_PIN)

	/*
	 * assert (low) the config pin and deassert the SHDN pin to initiate
	 * shutdown
	 */
	RADIO_CONTROL_PORT = RADIO_CONTROL_PORT & (0xFF ^ (RADIO_SHDN_PIN |
	    RADIO_CONFIG_PIN));

	/* wait for 5ms just to make sure the radio sees the low signal */
	__delay_ms(5);

	/*
	 * wake radio back up and go into config mode, waiting for the radio
	 * to initialize
	 */
	radio_on();

	/*
	 * take the config pin high again so we don't enter command mode next
	 * time
	 */
	RADIO_CONTROL_PORT = RADIO_CONTROL_PORT | RADIO_CONFIG_PIN;

	/*
	 * want for a second and assume that we succeed in entering config mode
	 */
	__delay_ms(1000);

#else
	/* request to go into config mode */
	putst("+++");

	/*
	 * want for a second and assume that we succeed in entering config mode
	 */
	__delay_ms(1500);
#endif

	/* null-terminated */
	putst(buffer);

	/* adds null terminator */
	if (getst_timeout(response, AT_COMMAND_RESPONSE_LENGTH,
	    AT_COMMAND_TIMEOUT_MS)) {
		/* calculate binary value from ASCII HEX value */
		for (i = 0; i < 4; i++) {
			value *= 10;
			value += ASCIIHEX_TO_BIN(response[i]);
		}
	}

	/* exit command mode */
	putst("ATCN\n");

	return value;
}

unsigned int
radio_voltage(void)
{
	return AT_command((unsigned char*) "AT%V\n");
}

unsigned int
radio_txpower(int level)
{
	char command[10] = "ATPL ,WR\n";

	/* level >= 0 signals that we are setting the power level ([0,4]) */
	if (level >= 0)
		/*
		 * convert int to ASCII digit and insert parameter into command
		 */
		command[4] = level - '0';

	return AT_command(command);
}

/*
 * get Receive Signal Strength level
 */
unsigned int
radio_rssi(void)
{
	return AT_command((unsigned char*) "ATDB\n");
}

unsigned int
radio_cyclical_sleep(char seconds)
{
	char command[10] = "ATSM0,WR\n";

	if (seconds >= 16)
		command[4] = '8';
	else if (seconds >= 8)
		command[4] = '7';
	else if (seconds >= 4)
		command[4] = '6';
	else if (seconds >= 2)
		command[4] = '5';
	else if (seconds == 1)
		command[4] = '4';

	/* else leave command in tact to disable cyclical sleep */
	return AT_command(command);
}

/* Set RTS pin low causing serial data to be sent out on DO. */
void
radio_rts_assert(void)
{
	RADIO_CONTROL_PORT &= (0xFF ^ RADIO_RTS_PIN);
	/* No return needed from void function. */
}

/* Set RTS pin high causing serial data to be stored in buffer. */
void
radio_rts_de_assert(void)
{
	RADIO_CONTROL_PORT |= RADIO_RTS_PIN;
	/* No return needed from void function. */
}
