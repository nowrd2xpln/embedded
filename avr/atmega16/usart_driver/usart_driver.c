#include <avr/io.h>

#ifndef NULL
#define NULL 0
#endif /* NULL */

/* F_CPU & BAUD RATE */
#define F_CPU       8000000UL	            //--- CPU Clock Frequency    
#define BAUD        9600                    //--- Transmission Baud Rate
#define BAUDRATE    ((F_CPU)/(BAUD*16UL)-1)	//--- Set baud rate value for UBRR
 
/* Prototype Declaration */
void usart_init(void);      //--- Usart Initialize
void usart_tx(char x);      //--- Usart Transmit
char usart_rx(void);        //--- Usart Receive
void usart_msg(char *c);    //--- Usart Transmit String
 
/* Global Delecarations */
char tx_data;
 
int main(void)
{
    usart_init();

	usart_msg("Atmega16 Bootup\r\n");
    usart_msg("INITIALIZING USART\r\n");

    while(1)
    {
         usart_msg("\r\n$> ");
         tx_data = usart_rx();          //--- Receive the data from rxd
         usart_tx(tx_data);             //--- Echo the received data
    }
}
 
/* Proto Type Declaration */
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
