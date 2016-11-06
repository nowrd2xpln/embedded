#include <avr/io.h>
#include <stdint.h>
#include "usart.h"

void usart_init(uint32_t baud)
{
	uint16_t baudrate = 0;
	uint16_t ubrr = 0;
	char msg[32] = {0};
	
	baudrate = F_CPU/baud/16UL - 1;
	UBRRH = 0;
	UBRRL = baudrate+1;                    			//--- Calculation is somehow off by 1
    UCSRB = (1<<TXEN) | (1<<RXEN);      			//--- Transmit and Receive Enable
    UCSRC|= (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1);	//--- Set the number of bits in a frame for RX/TX
}
 
void usart_tx(char x)
{
    UDR = x;                            //--- Move data to UDR Data Reg
    while (!(UCSRA & (1<<UDRE)));	    //--- Check whether UDR is empty
}

char usart_rx()
{
    while (!(UCSRA & (1<<RXC)));	    //--- Check whether Receive is complete
    return(UDR);                        //--- Return Data from UDR
}
 
void usart_msg(char *c)
{
    while (*c != NULL)                  //--- Check for null
        usart_tx(*c++);                 //--- Increment the pointer 
}
