#include <avr/io.h>

#define F_CPU	8000000UL	//--- CPU Clock Frequency 

#ifndef NULL
#define NULL 0
#endif /* NULL */

/* SPI bits */
#ifndef SCK
#define SCK		7
#endif /* SCK */

#ifndef MISO
#define MISO	6
#endif /* MISO */

#ifndef MOSI
#define MOSI	5
#endif /* MOSI */

#ifndef nSS
#define nSS		4
#endif /* nSS */


/* Prototype Declaration */
void SPI_MasterInit(void);

int main(void)
{
	SPI_MasterInit();
}

/* Proto Type Declaration */
void SPI_MasterInit(void)
{
	DDRB |= (1<<SCK)|(1<<MOSI)|(1<<nSS);	//-- SCK, MOSI and nSS as outputs
	DDRB &= ~(1<<MISO);						//-- MISO as input
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);	//-- Enable SPI, Master, set clock rate fck/16
}
