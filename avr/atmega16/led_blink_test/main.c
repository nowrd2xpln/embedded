//************************************************************//
 //System Clock          :1MHz
 //Software                 :AVR Studio 4
 //************************************************************//
#include<avr/io.h>
#include<util/delay.h>

#define           F_CPU           1000000
 
int main(void)
{
	DDRB=0xff;
	PORTB=0xff;
	
	while(1)
	{
		/*All the 8 LEDs of LED array are On*/
		PORTB=0xff;
		
		/*LEDs are On for 500ms*/
		_delay_ms(500);
		
		/*All the 8 LEDs of LED array are Off*/
		PORTB=0x00;
		
		/*LEDs are Off for 500ms*/
		_delay_ms(500);
	}
}
/*End of Program*/
