/*
 * adc.c
 *
 */

#include "adc.h"
#include "../System/lpc214x.h"
#include "../System/sysConfig.h"
#include "../System/sysdefs.h"
#include "../System/rprintf.h"
#include "../FreeRTOS/FreeRTOS.h"
#include "../FreeRTOS/task.h"
#include "../FreeRTOS/semphr.h"

/*
 * AD1.1 --> P0.8
 */
void adc1_init(void)
{
	/* Configure P0.25 as ADC */
	PINSEL1 &= ~(3<<18);
	PINSEL1 |= (1<<18);
	AD0_CR |= (1<<4);
	AD0_CR |= (12<<8);		// PCLK/8 = 4MHz
	AD0_CR |= (1<<21);		// ADC is operational

#if 0
	PINSEL0 |= (3<<16);
	AD1_CR |= (1<<1);		// Channel 1 on ADC 1
	AD1_CR |= (12<<8);		// PCLK/8 = 4MHz
	AD1_CR |= (1<<21);		// ADC is operational
#endif
}

int getADCData(void)
{
	AD0_CR |= (1<<24);		// Start conversion now
	while(!(AD0_GDR&(1<<31)))
		;
	return (AD0_GDR>>6) & 0x3ff;
}