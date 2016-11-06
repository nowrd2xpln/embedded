#include <avr/io.h>
#include "usart.h"

#ifndef NULL
#define NULL 0
#endif /*0 NULL */

/* F_CPU & BAUD RATE */
#define F_CPU       8000000UL	            //--- CPU Clock Frequency    

int main(void)
{
	usart_init(57600);
	
	usart_msg("\r\n\r\nAtmega16 Bootup\r\n");
    usart_msg("Intializing USART\r\n");

	while(1)
	{
		
	}
	return 0;
}
