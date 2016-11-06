#include <avr/io.h>
#include "usart.h"
#include "twi.h"

#ifndef NULL
#define NULL 0
#endif /*0 NULL */

/* F_CPU & BAUD RATE */
#define F_CPU       8000000UL	            //--- CPU Clock Frequency    

int main(void)
{
	DDRB = 0xff;
	PORTB = 0xFF;

	usart_init(57600);
	twi_init();

	usart_msg("\r\n\r\nAtmega16 Bootup\r\n");
    usart_msg("Intializing USART\r\n");


	twi_read_byte();
	
	return 0;
}
