#ifndef USART_H_
#define USART_H_

#ifndef NULL
#define NULL 0
#endif /* NULL */

void usart_init(uint32_t baud);
void usart_tx(char x);
char usart_rx(void);
void usart_msg(char *c);

#endif /* USART_H_ */
