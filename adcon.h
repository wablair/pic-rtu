#ifndef _adcon_h_
#define _adcon_h_

/* Output length for debugging output to LCD used during print_conversion. */
#define	OUTPUT_LEN	64

#define NUM_AIN		5	
#define	NUM_AQUASPY	2
#define	MAX_AQUASPY	10

#define AQUASPY_SOIL_MOISTURE	0
#define	AQUASPY_TEMPERATURE	4

#define AQUASPY_RETRIES		2

/* Number of reads to average over. */
#define NUM_READS	16	

struct adres {
	unsigned short high;
	unsigned short low;
};

struct ad_data {
	unsigned int raw[NUM_AIN];
	unsigned int reads[NUM_AIN][NUM_READS];
	unsigned int index[NUM_AIN];
	unsigned int num_reads_first_time_through[NUM_AIN];
	unsigned int average[NUM_AIN];
	unsigned int scaled[NUM_AIN];
	unsigned int m[NUM_AIN];
	unsigned int b[NUM_AIN];
	float soil_moisture[NUM_AQUASPY];
	float temperature[NUM_AQUASPY];
};


void		 adcon_init(void);
void		 ad_data_init(struct ad_data *);
void		 average_data(struct ad_data *);
int		 print_conversion(unsigned int);
unsigned int	 get_conversion(unsigned short int);

#endif /* !_adcon_h_ */
