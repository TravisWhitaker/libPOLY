# libPOLY Copyright (C) Travis Whitaker 2014

# This Makefile assumes Clang is available:
CC=clang
CFLAGS= -Wall -Wextra -Werror -pedantic -O2 -pipe -march=native
DEBUG_CFLAGS= -Wall -Wextra -Werror -pedantic -O0 -g -pipe -DDEBUG_MSG_ENABLE
INCLUDE= -I ./include

# You may need to change this to '-fpic' if you're using a strange
# architecture like ancient SPARC or MIPS:
FPIC = -fPIC

# Archiver for building the static library:
AR=ar
ARFLAGS=rvs

# Default values for user-supplied compile time directives:
DEBUG_MSG=

# Enable debugging messages outside of the 'debug' target:
ifeq ($(DEBUG_MSG),y)
	CFLAGS += -DDEBUG_MSG_ENABLE
endif

.PHONY: all
all: libpoly.a

.PHONY: shared
shared: client.o generator.o waveform.o
	$(CC) -shared -o libpoly.so client.o generator.o waveform.o

libpoly.a: client.o generator.o waveform.o
	$(AR) $(ARFLAGS) libpoly.a client.o generator.o waveform.o

client.o: src/client.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $(FPIC) src/client.c

generator.o: src/generator.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $(FPIC) src/generator.c

waveform.o: src/waveform.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $(FPIC) src/waveform.c

.PHONY: debug
debug: libpolydebug.a

libpolydebug.a: client.o.debug generator.o.debug waveform.o.debug
	$(AR) $(ARFLAGS) libpolydebug.a client.o.debug generator.o.debug waveform.o.debug

client.o.debug: src/client.c
	$(CC) -c $(DEBUG_CFLAGS) $(INCLUDE) $(FPIC) src/client.c -o client.o.debug

generator.o.debug: src/generator.c
	$(CC) -c $(DEBUG_CFLAGS) $(INCLUDE) $(FPIC) src/generator.c -o generator.o.debug

waveform.o.debug: src/waveform.c
	$(CC) -c $(DEBUG_CFLAGS) $(INCLUDE) $(FPIC) src/waveform.c -o waveform.o.debug

.PHONY: install
install:
	cp libpoly.a /usr/local/lib/
	chown root /usr/local/lib/libpoly.a
	chmod 0755 /usr/local/lib/libpoly.a
	cp include/poly.h /usr/local/include/
	chown root /usr/local/include/poly.h
	chmod 0755 /usr/local/include/poly.h

.PHONY: clean
clean:
	rm -f libpoly.a
	rm -f libpolydebug.a
	rm -f *.o
	rm -f *.o.debug
