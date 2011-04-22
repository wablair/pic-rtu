#ifndef _delay_h_
#define _delay_h_

#include <htc.h>

#include "main.h"

extern unsigned char delayus_variable;

#if PIC_CLK == 4000000 || PIC_CLK == 3686400
	#define DelayDivisor 4
	#define WaitFor1Us asm("nop")
	#define Jumpback asm("goto $ - 4")
#elif PIC_CLK == 8000000 || PIC_CLK == 10000000
	#define DelayDivisor 2
	#define WaitFor1Us asm("nop")
	#define Jumpback asm("goto $ - 4")
#elif PIC_CLK == 16000000
	#define DelayDivisor 1
	#define WaitFor1Us asm("nop")
	#define Jumpback asm("goto $ - 4")
#elif PIC_CLK == 20000000 || PIC_CLK == 18432000
	#define DelayDivisor 1
	#define WaitFor1Us asm("nop"); asm("nop")
	#define Jumpback asm("goto $ - 6")
#elif PIC_CLK == 32000000
	#define DelayDivisor 1
	#define WaitFor1Us asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop")
	#define Jumpback asm("goto $ - 12")
#else
	#error delay.h - please define PIC_CLK correctly
#endif

#define DelayUs(x) {							\
		delayus_variable = (unsigned char) (x / DelayDivisor);	\
		asm("movlb (_delayus_variable) >> 8");			\
		WaitFor1Us;						\
	}								\
	asm("decfsz (_delayus_variable)&0ffh,f");			\
	Jumpback;

#define __delay_us(x) __delay_400_cycles(((x)*(_XTAL_FREQ/1000000))/(1600))

#endif /* !_delay_h_ */
