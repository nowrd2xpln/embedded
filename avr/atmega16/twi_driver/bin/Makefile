###############################################################################
# Makefile for the project twi_driver
###############################################################################

## General Flags
PROJECT = twi_driver
MCU = atmega16
TARGET = twi_driver.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99                                     -DF_CPU=8000000UL -O2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=twi_driver.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"C:\Users\allen\Documents\GitHub\embedded\avr\atmega16\twi_driver\." 

## Library Directories
LIBDIRS = -L"C:\Users\allen\Documents\GitHub\embedded\avr\atmega16\twi_driver\." 

## Objects that must be built in order to link
OBJECTS = twi_driver.o usart.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) twi_driver.hex twi_driver.eep twi_driver.lss## Compile
twi_driver.o: ../twi_driver.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usart.o: ../usart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) twi_driver.elf dep/* twi_driver.hex twi_driver.eep twi_driver.lss twi_driver.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

