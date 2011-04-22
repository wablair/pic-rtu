#ifndef _serial_h_
#define _serial_h_

#define BAUD 9600

/* put line feed */
#define putlf putst("\n")

/*
Time taken:
optimisations on:       9cyc/number loop, 4.5us @ 8Mhz
with extra check ie:    && (RB7==1), +3cyc/number loop, +1.5us @ 8Mhz

Formula:
rough timeout value = (<us desired>/<cycles per loop>) * (PIC_CLK/4.0)
*/

/* how many cycles per loop, optimisations on */
#define LOOP_CYCLES_CHAR	9     

#ifndef timeout_char_us
#define timeout_char_us(x)	(unsigned char) ((x / LOOP_CYCLES_CHAR) * \
				    (PIC_CLK / 4.0))
#endif

#ifndef LOOP_CYCLES_INT
/* how many cycles per loop, optimisations on */
#define LOOP_CYCLES_INT       16    
#endif

#ifndef timeout_int_us
#define timeout_int_us(x)	(unsigned int) ((x / LOOP_CYCLES_INT) * \
				    (PIC_CLK / 4.0))
#endif


void		 serial_init(void);
void		 putch(unsigned char);
unsigned char	 getch(void);
unsigned char	 getch_timeout_bridge(int, unsigned char *);
int		 getst_timeout(unsigned char *, unsigned char, int);
void		 putst(register const char *);
unsigned char	 usart_timeout(void);
void		 putchdec(unsigned char);
void		 putchhex(unsigned char);
void		 putinthex(unsigned int);
void		 putint(unsigned long int);

unsigned char	 getch_available(void);
void		 clear_usart_errors(void);

#endif /* !_serial_h_ */
