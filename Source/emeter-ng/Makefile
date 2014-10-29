#--------------------------------------------------------------------------
#
#  Software for MSP430 based e-meters.
#
#  Steve Underwood <steve-underwood@ti.com>
#  Texas Instruments Hong Kong Ltd.
#
#  $Id: Makefile,v 1.19 2007/08/07 05:55:37 a0754793 Exp $
#
#--------------------------------------------------------------------------
#
#TARGET_MCU=msp430x1232
#TARGET_MCU=msp430x149
#TARGET_MCU=msp430x435
#TARGET_MCU=msp430x436
TARGET_MCU=msp430x437
#TARGET_MCU=msp430x449
#TARGET_MCU=msp430xE427
#TARGET_MCU=msp430xE425
#
#TARGET_STARBURST=-DUSE_STARBURST
TARGET_STARBURST=

MSP430INC=

objects=emeter-setup.o \
	emeter-background.o \
	emeter-communication.o \
	emeter-foreground.o \
	emeter-host-environment.o \
	emeter-basic-display.o \
	emeter-phase-corrections.o \
	emeter-fir-table.o \
	emeter-iiceeprom.o \
	emeter-multirate.o

CC=msp430-gcc
AR=msp430-ar
CFLAGS=-c -O2 -g -Wall -mmcu=$(TARGET_MCU) $(TARGET_STARBURST) $(MSP430INC) -I../emeter-toolkit

all: emeter.hex

clean:
	rm -f *.obj *.asm *.map *.lst *.tmp *.exe *.o *.hex *.qer *.asr *~

dist:
	rm -f *.obj *.asm *.map *.lst *.tmp *.exe *.o *.hex *.qer *.asr *~
	cd .. ; tar cvfz emeter-ng/emeter-ng.tgz emeter-ng

docs:
	doxygen doxygen

emeter.hex: emeter
	msp430-objcopy -O ihex emeter emeter.hex

emeter: ${objects}
	$(CC) -mmcu=$(TARGET_MCU) -oemeter ${objects} \
	    -L../emeter-toolkit/$(TARGET_MCU) -lemeter-toolkit -Wl,-Map=emeter.map -Wl,-T emeter-$(TARGET_MCU).x

#automatic collection of dependencies in the source files.
#it's only updated the first time, after that it must be done manually
#with "make depend"
#the dependecies are included from a separate file:
-include dependencies.in
#target to update the file, it's removed first
depend: rmdepend dependencies.in
#remove the file
rmdepend:
	rm -f dependencies.in
#build the file that contains the dependencies. no deps in this rule.
#if there were deps it would be rebuilt every chnage, which is unneded:
dependencies.in:
	$(CC) -MM ${CFLAGS} $(objects:.o=.c) >$@
