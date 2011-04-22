#ifndef _xtend_h_
#define _xtend_h_

#define RADIO_COMMAND_MODE_USE_SEQUENCE		0
#define RADIO_COMMAND_MODE_USE_PIN		1
#define RADIO_COMMAND_MODE			RADIO_COMMAND_MODE_USE_PIN

#define RADIO_CONTROL_PORT			PORTC
#define RADIO_DIRECTION_REGISTER		TRISC
#define RADIO_SHDN_PIN				0b00000001
#define RADIO_CONFIG_PIN			0b00000010
#define RADIO_SLEEP_PIN				0b00000010
#define RADIO_RTS_PIN				0b00000100

#define AT_COMMAND_TIMEOUT_MS			1000
#define AT_COMMAND_RESPONSE_LENGTH		20


/* radio takes < 100ms to wake up (Manual P.14) */
#define RADIO_WAKEUP_TIME_MS			100 

#define RADIO_ALLOW_TX_WAIT_S			1

#define RADIO_TXPOWER_1mW			0
#define RADIO_TXPOWER_10mW			1
#define RADIO_TXPOWER_100mW			2
#define RADIO_TXPOWER_500mW			3
#define RADIO_TXPOWER_1W			4

int	 radio_init(void);

/* bring the radio out of shutdown mode and return immediately */
int	 radio_on(void);

/* put the radio into shutdown mode and return immediately */
int	 radio_off(void);

/* puts the radio into hardware sleep mode (function blocks for the duration) */
int	 radio_sleep(unsigned int);

/* helper function that executes a given ASCII AT command */
unsigned int	 AT_command(unsigned char *);

/* get the voltage */
unsigned int	 radio_voltage(void);

/*
 * set the radio transmission power using the symbolic constants above,
 * such as RADIO_TXPOWER_1W
 */
unsigned int	 radio_txpower(int);

/* get Receive Signal Strength level */
unsigned int	 radio_rssi(void);

/*
 * set how often the radio wakes up for 100ms to listen for activity
 * (0 to disable cyclical sleep)
 */
unsigned int	 radio_cyclical_sleep(char);

void	 radio_rts_assert(void);
void	 radio_rts_de_assert(void);

#endif /* !_xtend_h_ */
