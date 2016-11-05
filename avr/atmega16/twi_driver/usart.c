#include <avr/io.h>
#include "usart.h"

void usart_init()
{
    UBRRH = 0x00;                       			//--- Initialize UBRR value for Baud Rate
    UBRRL = BAUDRATE;                       		//--- Baud Rate Set as 9600 by Data Sheet
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
