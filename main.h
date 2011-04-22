#ifndef _main_h_
#define _main_h_

#include "adcon.h" 

#define ASCIIHEX_TO_BIN(X)	((X < 65) ? (X - 48) : (X < 97) ? (X - 55) : \
				    (X - 87))

/*
 * must parenthesize (X) in order to preserve value if the result is
 * bit shifted
 */
#define BIN_TO_ASCIIHEX(X)	(((X & 0x0F) < 10) ? ((X & 0x0F) + '0') : \
				    ((X & 0x0F) - 10 +  'A'))

#define IOR44_LED_PORT			PORTB
#define IOR44_LED_DIRECTION_REGISTER	TRISB
#define IOR44_LED_PIN			0b01000000

/* set LED pin as an output */
#define led_init()		IOR44_LED_DIRECTION_REGISTER = \
				    IOR44_LED_DIRECTION_REGISTER & (0xFF ^\
				    IOR44_LED_PIN);
/* turn LED on */
#define led_on()		IOR44_LED_PORT = IOR44_LED_PORT | IOR44_LED_PIN

/* turn LED off */
#define led_off()		IOR44_LED_PORT = IOR44_LED_PORT & (0xFF ^\
				    IOR44_LED_PIN)

/* blink LED */
#define led_toggle()		IOR44_LED_PORT = IOR44_LED_PORT ^ IOR44_LED_PIN

#define led_blink		led_toggle

#define EEPROM_CONFIG_ADDRESS		0
#define EEPROM_COUNTER1_ADDRESS		(2 + sizeof(config_t))
#define EEPROM_COUNTER2_ADDRESS		(2 + EEPROM_COUNTER1_ADDRESS)

#ifndef PIC_CLK
/* #define PIC_CLK 3686400 */
#define PIC_CLK 4000000 
/* #define PIC_CLK 8000000 */
/* #define PIC_CLK 16000000 */
/* #define PIC_CLK 18432000 */
/* #define PIC_CLK 20000000 */
#endif /* !PIC_CLK */

/* Some Hi-Tech PICC18 functions (i.e. __delay_ms) need this defined. */
#define _XTAL_FREQ PIC_CLK

int	 rcif_poll(unsigned int, struct ad_data *); 
int	 get_aquaspy_sensor_data(void);

#endif /* _main_h_ */
