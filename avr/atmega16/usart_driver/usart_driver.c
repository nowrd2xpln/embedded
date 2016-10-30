#include <avr/io.h>
 
/* F_CPU & BAUD RATE */
#define F_CPU       8000000UL   //--- CPU Clock Frequency
#define BAUD_RATE   9600        //--- Transmission Baud Rate
#define BAUD        9600
#define BAUDRATE    ((F_CPU)/(BAUD*16UL)-1)            // set baud rate value for UBRR
 
/* Prototype Declaration */
void usart_init(void);      //--- Usart Initialize
void usart_tx(char x);      //--- Usart Transmit
char usart_rx(void);        //--- Usart Receive
void usart_msg(char *c);    //--- Usart Transmit String
 
/* Global Delecarations */
char tx_data;
 
int main(void)
{
    usart_init();                       //--- Usart initializing
    usart_msg("INITIALIZING USART");    //--- Send a set of String
    usart_tx(0x0d);                     //--- Cmd to jump to next line
    while(1)
    {
         usart_msg("<<");
         tx_data = usart_rx();          //--- Receive the data from rxd
         usart_tx(0x0d);                //--- Next Line
         usart_msg(">>");
         usart_tx(tx_data);             //--- send the received data back
         usart_tx(0x0d);                //--- Next Line
    }
}
 
/* Proto Type Declaration */
void usart_init()
{
    UBRRH = 0x00;                       			//--- Initialize UBRR value for Baud Rate
    UBRRL = 0x33;                       			//--- Baud Rate Set as 9600 by Data Sheet
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
    while (*c != 0)                     //--- Check for null
        usart_tx(*c++);                 //--- Increment the pointer 
}
