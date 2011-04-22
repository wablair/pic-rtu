#include <pic18.h>
#include <stdio.h>
#include <string.h>

#include "adcon.h"
#include "lcd.h"
#include "main.h"


void
adcon_init(void)
{
	ADCON0 = 0x80;
	ADCON1 = 0x80;
	TRISA = 0x3F; 
	ADON = 1;
	ADRESH = 0;
	ADRESL = 0;

	/* No return needed from a void function. */
}

int
print_conversion(unsigned int which_input)
{
	unsigned char output[OUTPUT_LEN];
	unsigned int input;

	if (which_input >= NUM_AIN)
		return -1;

	memset(output, '\0', OUTPUT_LEN);

	input = get_conversion(which_input);

	sprintf(output, "Input: %u Raw: %04x\r\n%u mV", which_input, input,
	    (unsigned int) (input * (5000.0 / 1024.0)));

	lcd_clear();
	lcd_print(output);
	
	return 0;
}

unsigned int
get_conversion(unsigned short int which_input)
{
	unsigned int x, output;
	adcon_init();

	if (which_input >= NUM_AIN)
		return 0;

	for (x = 0; x < 0x0F; x++) {
		switch (which_input) {
		case 0:
			ADCON0 = 0x81;
			break;
		case 1:
			ADCON0 = 0x89;
			break;
		case 2:
			ADCON0 = 0x91;
			break;
		case 3:
			ADCON0 = 0x99;
			break;
		case 4:
			ADCON0 = 0xA1;
			break;
		default:
			return 0;
		}

		GODONE = 1;

		while (GODONE)
			__delay_ms(1);

		ADIF = 0;

		output = (ADRESH << 8) | ADRESL;
	}

	ADON = 0;

	return output;
}

void
ad_data_init(struct ad_data *data)
{
	int x = 0;

	if (data == NULL)
		return;

	memset(data->raw, '\0', NUM_AIN);
	memset(data->b, '\0', NUM_AIN);
	memset(data->num_reads_first_time_through, '\0', NUM_AIN);
	memset(data->m, 1, NUM_AIN);
	memset(data->index, '\0', NUM_AIN);

	for (x = 0; x < NUM_AIN; x++)
		memset(data->reads[x], '\0', NUM_READS);

	/* No return needed from void function. */
}

void
average_data(struct ad_data *data)
{
	/* No return needed from void function. */
}
