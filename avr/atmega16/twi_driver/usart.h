#ifndef USART_H_
#define USART_H_

#ifndef NULL
#define NULL 0
#endif /* NULL */

/* F_CPU & BAUD RATE */
#define F_CPU       8000000UL	            //--- CPU Clock Frequency    
#define BAUD        9600                    //--- Transmission Baud Rate
#define BAUDRATE    ((F_CPU)/(BAUD*16UL)-1)	//--- Set baud rate value for UBRR

void usart_init(void);
void usart_tx(char x);
char usart_rx(void);
void usart_msg(char *c);

#endif /* USART_H_ */
