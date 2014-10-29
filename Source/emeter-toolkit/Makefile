#
# $Id: Makefile,v 1.7 2004/07/08 09:32:08 A0754793 Exp $
#
LIBNAME=libemeter-toolkit.a

CC=gcc
AR=ar
CFLAGS=-c -O2 -I. -mno-cygwin

objects=host_decu16.o \
        host_decu32.o \
        host_dec16.o \
        host_dec32.o \
        host_hex16.o \
        host_hex32.o \
        host_str.o \
        host_char.o \
        LCDdecu16.o \
        LCDdecu32.o \
        LCDdec16.o \
        LCDdec32.o \
        LCD-module.o \
        LCD-internal.o \
        hex2ASCII.o \
        bin2bcd16.o \
        bin2bcd32.o \
        flash_clr.o \
        flash_secure.o \
        flash_memcpy.o \
        flash_write_int8.o \
        flash_write_int16.o \
        flash_write_int32.o \
        flash_replace16.o \
        flash_replace32.o \
        iiceeprom.o \
        rtc.o \
        dds.o \
        rand16.o \
        dc_filter.o \
        imul16.o \
        isqrt32.o \
        isqrt32i.o \
        q1_15_mul.o \
        q1_15_mulr.o \
        q1_15_mulq.o \
		accum48.o \
		div48.o \
		div_sh48.o \
		mul48.o \
		shift48.o \
        debounce.o \
		setdco.o
	
all: $(LIBNAME)
	for I in msp430x* ; do cd $$I ; make || exit 1 ; cd .. ; done;

clean:
	rm -f *.obj *.asm *.map *.lst *.tmp *.exe *.o *.a *.hex *.qer *.asr *~
	for I in msp430x* ; do cd $$I ; make clean || exit 1 ; cd .. ; done;

dist:
	rm -f *.obj *.asm *.map *.lst *.tmp *.exe *.o *.hex *.qer *.asr *~
	for I in msp430x* ; do cd $$I ; make clean || exit 1 ; cd .. ; done;
	cd .. ; tar cvfz emeter-toolkit/emeter-toolkit.tgz emeter-toolkit

hostlib: $(LIBNAME)

$(LIBNAME): ${objects}
	$(AR) -r $(LIBNAME) ${objects}

$(objects): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

#automatic collection of dependencies in the source files.
#it's only updated the first time, after that it must be done maually
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
	$(CC) -MM ${CFLAGS} $(patsubst %.o,%.c,$(objects)) >$@
