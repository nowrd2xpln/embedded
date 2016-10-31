#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU	8000000UL	//--- CPU Clock Frequency 

#ifndef NULL
#define NULL 0
#endif /* NULL */

/* SPI bits */
#define SCK		DDB7
#define MISO	DDB6
#define MOSI	DDB5
#define nSS		DDB4

/* Prototype Declaration */
void SPI_MasterInit(void);
unsigned char SPI_Tranceiver(unsigned char data);
ISR(SPI_STC_vect);

/* Global variables */
volatile unsigned char data;

int main(void)
{
	SPI_MasterInit();

	while(1)
	{

	}
}

/* Proto Type Declaration */
void SPI_MasterInit(void)
{
	DDRB |= (1<<SCK)|(1<<MOSI)|(1<<nSS);			//-- SCK, MOSI and nSS as outputs
	DDRB &= ~(1<<MISO);								//-- MISO as input
	SPCR  = (1<<SPE)|(1<<SPIE)|(1<<MSTR)|(1<<SPR0);	//-- Enable SPI, SPI Interrupt, Master, set clock rate fck/16

	sei();	//-- Enable Global Interrupts
}

ISR (SPI_STC_vect)
{
	data = SPDR;
	// Process the received data
}

unsigned char SPI_Tranceiver(unsigned char data)
{
    SPDR = data;				//-- Load data into the buffer

    while(!(SPSR & (1<<SPIF) ));//-- Wait until transmission complete

    return(SPDR);				//-- Return received data
}
