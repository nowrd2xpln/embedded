#include <avr/io.h>
#include "usart.h"
#include "twi.h"
#include <util/delay.h>

#define ERROR		1
#define TP81_ADDR	0xD0
#define SUCCESS		0
	
// Reference http://www.embedds.com/programming-avr-i2c-interface/

union  USI_TWI_state
{
  unsigned char errorState;         // Can reuse the TWI_state for error states due to that it will not be need if there exists an error.
  struct
  {
    unsigned char addressMode         : 1;
    unsigned char masterWriteDataMode : 1;
    unsigned char unused              : 6;
  }; 
}   USI_TWI_state;

void twi_init(void)
{
	TWBR=0x0C;  				// Bit rate
	TWSR=(0<<TWPS1)|(0<<TWPS0); // Setting prescalar bits	
								// SCL freq= F_CPU/(16+2(TWBR).4^TWPS)
								// 400kHz
	TWCR = (1<<TWEN);
}

void twi_start(void)
{
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
}

void twi_stop(void)
{
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void twi_write(uint8_t u8data)
{
    TWDR = u8data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
}

uint8_t twi_read_ack(void)
{
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    while ((TWCR & (1<<TWINT)) == 0);
    return TWDR;
}

uint8_t twi_read_nack(void)
{
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
    return TWDR;
}

uint8_t twi_get_status(void)
{
    uint8_t status;
    //mask status
    status = TWSR & 0xF8;
    return status;
}

uint8_t twi_read_byte(void)
{
    //uint8_t databyte;
    twi_start();

    if (twi_get_status() != 0x08)
        return ERROR;

    //select devise and send A2 A1 A0 address bits
    twi_write(0xD0);
    if (twi_get_status() != 0x18)
        return ERROR;

    twi_write(0x01);
    if (twi_get_status() != 0x18)
        return ERROR;
	twi_read_ack();

    twi_write(0xD1);
    if (twi_get_status() != 0x18)
        return ERROR;

	twi_start();

    twi_read_nack();
    if (twi_get_status() != 0x58)
        return ERROR;
    twi_stop();

    return SUCCESS;
}
