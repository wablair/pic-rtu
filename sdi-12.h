#ifndef _sdi-12_h_
#define _sdi-12_h_

#define	OUTPUT	0
#define	INPUT	1
#define	BUF_LEN	100


#define SDI12_PORT		RB0
#define SDI12_PORT_DIRECTION	TRISB0

/* Width of bit (1 / 1200 baud) in useconds. */
#define	SDI12_DELAY		833
#define SDI12_DELAY_HALF	416

/* Marking binary state - 1, Voltage Range - -0.5V to 1.0V */
#define MARKING		0

/* Spacing binary state - 0, Voltage Range - 3.5V to 5.5V */
#define SPACING		1

#define NAME_SIZE	32

#define SDI12_MARKING() {\
	SDI12_PORT_DIRECTION = OUTPUT; \
	SDI12_PORT = MARKING; }

#define SDI12_SPACING() {\
	SDI12_PORT_DIRECTION = OUTPUT; \
	SDI12_PORT = SPACING; }
	

struct identification_data {
	int address;
	int compatability_number;
	char company_name[NAME_SIZE];
	int model_number;
	int version;
	int serial_number;
};

void	 sdi12_send_break(void);

/* Commands Supported by the AquaSpy: */

/* aAb! Changes probe base address to b. */
int	 sdi12_change_address(int, int);

/* ?!   Returns probe base address. */
int	 sdi12_address_query(void);

/*
 * aM0! Same as aM!
 * aM1! Measures new air count. Use for calibration only.
 * aM2! Measures new water count. Use for calibration only.
 */
void	 sdi12_additional_measurements(void);

/* aI!  Returns sensor ID. All sensors in a probe have the same ID. */
struct identification_data	 sdi12_send_identification(int);

/*
 * aM!  Measures new soil moisture for addressed sensor.
 *	This overwrites the internally measured average soil moisture ­
 *	if averaging is required, no measurement should be done.
 */
int	 sdi12_start_measurement(int);

/*
 * aD!  Sends scaled soil moisture reading. This is either the internally
 *	measured averaged reading, or if an M command has been performed
 *	it is the instantaneous moisture reading at that time.  Range is
 *	+00.000 to +99.999
 * aD0! Same as aD!
 * aD1! Sends air calibration count. For test purposes.
 *	Range is +00000 to +65535.
 * aD2! Sends water calibration count. For test purposes.
 *	Range is +00000 to +65535.
 * aD3! Sends raw soil count. Range is +00000 to +65535.
 * aD4! Sends temperature, for the section of probe that the sensor is
 *	fitted to. Output is signed Celsius, example +27.3. There is one
 *	temperature sensor per probe section, mounted at the top of that
 *	section.
 */
float	 sdi12_send_data(int, int);

/* Not implemented by AquaSpy. */

/* a! */
void	 sdi12_acknowledge_active(void);

/* aC! */
void	 sdi12_start_concurrent_measurement(void);

/* aV! */
void	 sdi12_start_verification(void);

/* aCd! */
void	 sdi12_additional_concurrent_measurements(void);

/* aRd! */
void	 sdi12_continuous_measurements(void);

/* CRC is not currently implemented in the AquaSpy. */
void	 sdi12_crc_start_measurement_request(void);
void	 sdi12_crc_start_concurrent_measurement_request(void);
void	 sdi12_crc_additional_measurements_request(void);
void	 sdi12_crc_additional_concurrent_measurements_request(void);
void	 sdi12_crc_continuous_measurements_request(void);
void	 sdi12_crc_send_data(void);

void	 sdi12_putchar(char);
char	 sdi12_getchar(void);

unsigned short int	 sdi12_gets(char *, unsigned short int);

#endif /* !_sdi-12_h_ */
