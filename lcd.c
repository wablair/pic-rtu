#include <pic18.h>

#include "i2c.h"
#include "lcd.h"
#include "main.h"
#include "modbus_ascii.h"
#include "serial.h"
#include "xtend.h"

char line = 1;

int
lcd_init(void)
{
	unsigned char bitmap1[8] = LCD_ABE_LOGO_Q1;
	unsigned char bitmap2[8] = LCD_ABE_LOGO_Q2;
	unsigned char bitmap3[8] = LCD_ABE_LOGO_Q3;
	unsigned char bitmap4[8] = LCD_ABE_LOGO_Q4;

	/* 20 characters plus null terminator */
	unsigned char line1[21] = {7, 4, 1, 7, 7, 7, 7, 7, 7, 'A', 'x', 'i',
	    'o', 'm', '-', 'B', 'l', 'a', 'i', 'r', 0};

	/* 20 characters plus null terminator */
	unsigned char line2[21] = {7, 3, 2, 7, 7, 7, 7, 7, 7, 7, 7, 'P', 'I',
	     'C', '-', 'R', 'T', 'U', 7, 7, 0};

	int i;

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte(LCD_DEFINE_CUSTOM_CHAR);

	/* Character Number [0, 7] */
	i2c_PutByte(4);

	for (i = 0; i < 8; i++) {
		i2c_PutByte(bitmap1[i]);
	}

	i2c_Stop();

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte(LCD_DEFINE_CUSTOM_CHAR);

	/* Character Number [0, 7] */
	i2c_PutByte(1);

	for (i = 0; i < 8; i++) {
		i2c_PutByte(bitmap2[i]);
	}

	i2c_Stop();

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte(LCD_DEFINE_CUSTOM_CHAR);

	/* Character Number [0, 7] */
	i2c_PutByte(2);

	for (i = 0; i < 8; i++) {
		i2c_PutByte(bitmap3[i]);
	}

	i2c_Stop();

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte(LCD_DEFINE_CUSTOM_CHAR);

	/* Character Number [0, 7] */
	i2c_PutByte(3);

	for (i = 0; i < 8; i++) {
		i2c_PutByte(bitmap4[i]);
	}

	i2c_Stop();
	
	/* move to top line */
	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte(LCD_SET_CURSOR_POSITION);

	/* goto line */
	i2c_PutByte(1);

	/* goto column */
	i2c_PutByte(1);

	i2c_Stop();

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte(LCD_WRITE_STRING);
	i = 0;
	while (line1[i]) {
		i2c_PutByte(line1[i]);
		i++;
	}

	/* automatically wraps to the second line */
	i = 0;
	while (line2[i]) {
		i2c_PutByte(line2[i]);
		i++;
	}
	i2c_Stop();
	
	/*
	 * move to the second line because lcd_print will send a newline to
	 * move to the third line before it prints
	 */
	line = 2;

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte(LCD_SET_CURSOR_POSITION);

	/* goto line */
	i2c_PutByte(line);

	/* goto column */
	i2c_PutByte(1);

	i2c_Stop();
	
	return 0;
}

int
lcd_print(unsigned char *string)
{
	int i = 0;
	lcd_set(1);
	
	if (line == 4) {
		/* scroll */
		/* generates a Start condition */
		i2c_WriteTo(0x50);

		i2c_PutByte(LCD_SHIFT_DISPLAY_UP);
		i2c_Stop();
	
		/* move to bottom */
		/* generates a Start condition */
		i2c_WriteTo(0x50);
		i2c_PutByte(LCD_SET_CURSOR_POSITION);
		i2c_PutByte(4);
		i2c_PutByte(1);
		i2c_Stop();
	}

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte(LCD_WRITE_STRING);
	/*
	if (line < 4) {
		line++;
		i2c_PutByte('\n');
	}
	*/
	while (string[i]) {
		i2c_PutByte(string[i]);
		if (++i % 20 == 0) {
			/* wrap to the next line */
			/*
			i2c_PutByte('\n');
			*/
		}
	}

	/* because it seems to leave off the last character */
	i2c_PutByte(' ');

	i2c_Stop();
	
	return 0;
}

int
lcd_putch(unsigned char c)
{
	i2c_Start();
	i2c_WriteTo(0x50);
	i2c_PutByte(LCD_WRITE_STRING);
	i2c_PutByte(c);
	i2c_Stop();
	
	return 0;
}

int
lcd_putint(unsigned int value)
{
	unsigned char buffer[10];
	unsigned short int i = 0;
	
	do {
		/*
		 * accumulate digits from least to most significant (backwards)
		 */
		buffer[i] = BIN_TO_ASCIIHEX((value % 10));

		/* move to next most significant digit */
		value /= 10;

		/*
		 * must do increment separately instead of inline above
		 * because the compiler appears not to support inline
		 * increments
		 */
		i++; 
	} while (value && i < sizeof(buffer));

	while (i) {
		i--;

		/* loop backwards to print digits out forward */
		lcd_putch(buffer[i]);
	}
	lcd_putch(' ');
	return 0;
}

int
i2c_PutTerminatedString(unsigned char *string)
{
	unsigned char i = 0;
	
	while (string[i]) {
		i2c_PutByte(string[i++]);
	}

	return 0;
}

/*
 * Sets lcd cursor position to the first char at a user specified line.
 */
int
lcd_set(unsigned int line)
{
	i2c_WriteTo(0x50);
	i2c_PutByte(LCD_SET_CURSOR_POSITION);
	i2c_PutByte(line);
	i2c_PutByte(1);
	i2c_Stop();
	return 0;
}

void
lcd_set_startup(void)
{
	unsigned char bitmap1[8] = LCD_ABE_LOGO_Q1;
	unsigned char bitmap2[8] = LCD_ABE_LOGO_Q2;
	unsigned char bitmap3[8] = LCD_ABE_LOGO_Q3;
	unsigned char bitmap4[8] = LCD_ABE_LOGO_Q4;

	unsigned char line1[21] = LCD_LINE_ONE; 

	unsigned char line2[21] = LCD_LINE_TWO; 

	int i;

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte_Remember(LCD_REMEMBER_COMMAND);

	i2c_PutByte_Remember(LCD_DEFINE_CUSTOM_CHAR);

	/* Character Number [0, 7] */
	i2c_PutByte_Remember(4);

	for (i = 0; i < 8; i++) {
		i2c_PutByte_Remember(bitmap1[i]);
	}

	i2c_Stop();

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte_Remember(LCD_REMEMBER_COMMAND);

	i2c_PutByte_Remember(LCD_DEFINE_CUSTOM_CHAR);

	/* Character Number [0, 7] */
	i2c_PutByte_Remember(1);

	for (i = 0; i < 8; i++) {
		i2c_PutByte_Remember(bitmap2[i]);
	}

	i2c_Stop();

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte_Remember(LCD_REMEMBER_COMMAND);
	i2c_PutByte_Remember(LCD_DEFINE_CUSTOM_CHAR);

	/* Character Number [0, 7] */
	i2c_PutByte_Remember(2);

	for (i = 0; i < 8; i++) {
		i2c_PutByte_Remember(bitmap3[i]);
	}

	i2c_Stop();

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte_Remember(LCD_REMEMBER_COMMAND);

	i2c_PutByte_Remember(LCD_DEFINE_CUSTOM_CHAR);

	/* Character Number [0, 7] */
	i2c_PutByte_Remember(3);

	for (i = 0; i < 8; i++) {
		i2c_PutByte_Remember(bitmap4[i]);
	}

	i2c_Stop();
	
	/* move to top line */
	/* generates a Start condition */
	i2c_WriteTo(0x50);

	i2c_PutByte(LCD_SET_CURSOR_POSITION);

	/* goto line */
	i2c_PutByte(1);

	/* goto column */
	i2c_PutByte(1);

	i2c_Stop();

	/* generates a Start condition */
	i2c_WriteTo(0x50);

	/*
	i2c_PutByte(LCD_WRITE_STRING);
	*/
	i2c_PutByte(LCD_SET_STARTUP_SCREEN);
	i2c_PutByte(0x01);
	i = 0;
	while (line1[i]) {
		i2c_PutByte_Remember(line1[i]);
		i++;
	}

	/* automatically wraps to the second line */
	i2c_Stop();
	i2c_WriteTo(0x50);
	i2c_PutByte(LCD_SET_STARTUP_SCREEN);
	i2c_PutByte(0x02);
	i = 0;
	while (line2[i]) {
		i2c_PutByte_Remember(line2[i]);
		i++;
	}
	i2c_Stop();
}

void
i2c_PutByte_Remember(unsigned char input)
{
	__delay_ms(5);
	i2c_PutByte(input);
	__delay_ms(5);
}

void
lcd_clear(void) {
	/*
	char clear[] = "\f";
	lcd_print(clear);
	lcd_set(1);
	line = 1;
	*/
	lcd_putch('\f');

}

void
lcd_puthex(unsigned char input)
{
	lcd_putch(high_nibble_to_char(input));
	lcd_putch(low_nibble_to_char(input));
	lcd_putch(' ');
	/* No return needed from void function. */
}
