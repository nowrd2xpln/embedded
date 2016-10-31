#include "./osHandles.h"                // Includes all OS Files
#include "./System/cpu.h"               // Initialize CPU Hardware
#include "./System/crash.h"             // Detect exception (Reset)
#include "./System/watchdog.h"
#include "./drivers/uart/uart0.h"       // Initialize UART
#include "./System/rprintf.h"			// Reduced printf.  STDIO uses lot of FLASH space & Stack space
#include "./general/userInterface.h"	// User interface functions to interact through UART
#include "./fat/ff.h"					// FAT File System Library
#include "./fat/diskio.h"				// Disk IO Initialize SPI Mutex
#include "./adc/adc.h"
#include "./drivers/pcm1774.h"
#include "./drivers/sta013.h"

/* INTERRUPT VECTORS:
 * 0:    OS Timer Tick
 * 1:    Not Used
 * 2:    UART Interrupt
 * 3:    Not Used
 * 4:    I2C
 * 5-16: Not Used
 */

/* Resources Used
 * 1.  TIMER0 FOR OS Interrupt
 * 2.  TIMER1 FOR Run-time Stats (can be disabled at FreeRTOSConfig.h)
 * 3.  Watchdog for timed delay functions (No CPU Reset or timer)
 */

#define BUTT7 0x80  //bit 7
#define BUTT6 0x40  //bit 6
#define BUTT5 0x20  //bit 5
#define BUTT4 0x10  //bit 4
#define BUTT3 0x08  //bit 3
#define BUTT2 0x04  //bit 2
#define BUTT1 0x02  //bit 1
#define BUTT0 0x01  //bit 0
#define PE_ADDR 0x40 //PortExpander Address
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct
{
	char song[16];
}PLAYLIST;

/* function prototypes */
void controls(void *pvParameters);
void adc();
void mp3(void *pvParameters);
void initPE();
void diskTimer();
void delay_ms(unsigned long count);

/* global variables */
char adcdata[5];
char theMaxValue[5];
char blank = '\n';
char changeyourlifestyle[15] = "lifestyle.mp3";
int playlistIndex=0;
int nowPlayingIndex=0;
int maxValue = 0;
int ledStuff = 0;
unsigned int br = sizeof(adcdata);
unsigned int br2 = sizeof(theMaxValue);
unsigned int bw = 0;

FIL fdst;
FRESULT res;
PLAYLIST songs[20];

int main (void)
{
	OSHANDLES System;            // Should contain all OS Handles

	cpuSetupHardware();          // Setup PLL, enable MAM etc.
	watchdogDelayUs(2000*1000);  // Some startup delay
	uart0Init(38400, 256);       // 256 is size of Rx/Tx FIFO

	// Use polling version of uart0 to do printf/rprintf before starting FreeRTOS
	rprintf_devopen(uart0PutCharPolling);

	if(didSystemCrash())
	{
		printCrashInfo();
		clearCrashInfo();
	}
	cpuPrintMemoryInfo();

	// Open interrupt-driven version of UART0 Rx/Tx
	rprintf_devopen(uart0PutChar);

	System.lock.SPI = xSemaphoreCreateMutex();
	System.queue.songname = xQueueCreate(1,15);

	// Use the WATERMARK command to determine optimal Stack size of each task (set to high, then slowly decrease)
	// Priorities should be set according to response required

	if(
		// User Interaction set to lowest priority.
		pdPASS != xTaskCreate( uartUI, (signed char*)"Uart UI", STACK_BYTES(1024*6), &System, PRIORITY_LOW,  &System.task.userInterface ) ||
		// diskTimer should always run, and it is a short function, so assign CRITIAL priority.
		pdPASS != xTaskCreate( diskTimer, (signed char*)"Dtimer", STACK_BYTES(256), &System, PRIORITY_CRITICAL,  &System.task.diskTimer ) ||
		pdPASS != xTaskCreate( mp3, (signed char*)"mp3", STACK_BYTES(4*1024), &System, PRIORITY_HIGH, &System.task.mp3 ) ||
		pdPASS != xTaskCreate( controls, (signed char*)"controls", STACK_BYTES(1024*4), &System, PRIORITY_HIGH, &System.task.controls ) ||
		pdPASS != xTaskCreate( adc, (signed char*)"adc", STACK_BYTES(256), &System, PRIORITY_CRITICAL,  &System.task.adc )
	)
	{
		rprintf("ERROR:  OUT OF MEMORY: Check OS Stack Size or task stack size.\n");
	}

	rprintf("\n-- Starting FreeRTOS --\n");
	vTaskStartScheduler();	// This function will not return.

	rprintf_devopen(uart0PutCharPolling);
	rprintf("ERROR: Unexpected OS Exit!\n");
	return 0;
}

void delay_ms(unsigned long count)
{
	volatile unsigned long c = count * 4000;

	while (--c)
		;
}

void diskTimer()
{
	for(;;)
	{
		vTaskDelay(10);
		disk_timerproc();
	}
}

void initPE()
{
	char Data = 0x00;
	i2cWriteDeviceRegister(PE_ADDR, 6, Data);		// Register 6 Port0 enabled as OUTPUT
	Data = 0xFF;
	i2cWriteDeviceRegister(PE_ADDR, 7, Data);		// Register 7 Port1 enabled as INPUT
	Data = 0x00;
	i2cWriteDeviceRegister(PE_ADDR, 2, Data);		// turning off LEDs
}

void mp3(void *pvParameters)
{
	OSHANDLES *osHandles = (OSHANDLES*)pvParameters;

	char songname[15];
	while(1){
		if(xQueueReceive(osHandles->queue.songname, &songname[0], portMAX_DELAY)){
			rprintf("Playing song: %s\n", songname);
			FIL fileHandle;
			char buffer[2048];

			if(FR_OK == f_open(&fileHandle, songname, FA_OPEN_EXISTING | FA_READ)){
				rprintf("File opened, about to play: \n");
				int numOfReadBytes = sizeof(buffer);
				while(numOfReadBytes == sizeof(buffer)){
					if(FR_OK == f_read(&fileHandle, buffer, sizeof(buffer), &numOfReadBytes)){
						if(xSemaphoreTake(osHandles->lock.SPI, 1000)){
							SELECT_MP3_CS();

							int i = 0;
							while(i < numOfReadBytes){
								if(STA013_NEEDS_DATA()){
									rxTxByteSSPSPI(buffer[i++]);
								}else{
									vTaskDelay(1);
								}
							}

							DESELECT_MP3_CS();
							xSemaphoreGive(osHandles->lock.SPI);
						}else{
							rprintf("Error: SPI semaphore is taken\n");
						}
					}
				}
				f_close(&fileHandle);
			}else{
				rprintf("Error opening file.\n");
			}
		}
	}
}

void adc()
{
	adc1_init();
	dataFromADC = 0;

	while(1)
	{
		vTaskDelay(10);
		dataFromADC = getADCData();
	}
}

void controls(void *pvParameters)
{
	char dirPath[] = "0:";
	char Lfname[512];
	char songname[15];
	int vol = 95;
	int pausePlay = 0;
	OSHANDLES *osHandles = (OSHANDLES*)pvParameters;
	DIR Dir;
	FILINFO Finfo;
	FRESULT returnCode = FR_OK;

	vTaskDelay(3000);
	initPE();
	pcm1774_OutputVolume(vol,vol);

	while(1)
	{
		// Check buttons
		// based on buttons, change volume, bass, treble etc...
		char i2cData = i2cReadDeviceRegister(PE_ADDR, 1);
		delay_ms(100);

		switch(i2cData)
		{
			case BUTT5:	//BUTT5 = STOP button
				ledStuff=0x1F;
				i2cWriteDeviceRegister(PE_ADDR, 2, ledStuff);
				delay_ms(1000);
				ledStuff=0x0F;
				i2cWriteDeviceRegister(PE_ADDR, 2, ledStuff);
				delay_ms(1000);
				ledStuff=0x07;
				i2cWriteDeviceRegister(PE_ADDR, 2, ledStuff);
				delay_ms(1000);
				ledStuff=0x03;
				i2cWriteDeviceRegister(PE_ADDR, 2, ledStuff);
				delay_ms(1000);
				ledStuff=0x01;
				i2cWriteDeviceRegister(PE_ADDR, 2, ledStuff);
				delay_ms(1000);
				ledStuff=0x00;
				i2cWriteDeviceRegister(PE_ADDR, 2, ledStuff);

				res = f_open(&fdst, "0:writeINTOme.csv", FA_CREATE_ALWAYS | FA_WRITE);
				for(int i=0; i<100; i++)
				{
					if(i==0) i2cWriteDeviceRegister(PE_ADDR, 2, 0xFF);
					else if(i==15) i2cWriteDeviceRegister(PE_ADDR, 2, 0x7F);
					else if(i==30) i2cWriteDeviceRegister(PE_ADDR, 2, 0x3F);
					else if(i==45) i2cWriteDeviceRegister(PE_ADDR, 2, 0x1F);
					else if(i==60) i2cWriteDeviceRegister(PE_ADDR, 2, 0x0F);
					else if(i==75) i2cWriteDeviceRegister(PE_ADDR, 2, 0x07);
					else if(i==90) i2cWriteDeviceRegister(PE_ADDR, 2, 0x03);
					else if(i==99) i2cWriteDeviceRegister(PE_ADDR, 2, 0x01);

					rprintf("\nData from adc: %i", dataFromADC);

					sprintf(adcdata, "%d", dataFromADC);
					adcdata[4]=',';

					res = f_write(&fdst, &adcdata, br, &bw);
					rprintf("\ndata stuff: %i", (char) dataFromADC);
					delay_ms(100);

					maxValue = MAX(maxValue, dataFromADC);
				}

				i2cWriteDeviceRegister(PE_ADDR, 2, 0x00);
				res = f_write(&fdst, &blank, sizeof(blank), &bw);

				rprintf("\n\nthe max is: %i", maxValue);
				sprintf(theMaxValue, "%d", maxValue);
				res = f_write(&fdst, &theMaxValue, br2, &bw);

				f_close(&fdst);

				if(!xQueueSend(osHandles->queue.songname, &changeyourlifestyle[0],0))
				{
					rprintf("No dequeue.\n");
				}
				break;
			}
			default:
				break;
		}
	}
}