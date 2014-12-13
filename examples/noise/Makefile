CC := clang
# CC := gcc # for wingnuts
CFLAGS := -fvisibility=hidden
CPPFLAGS := -std=gnu11 -Iinc

#CPPFLAGS := $(CPPFLAGS) -Wall -Wextra
#CPPFLAGS := $(CPPFLAGS) -Weverything
CPPFLAGS := $(CPPFLAGS) -w

#CFLAGS := $(CFLAGS) -ggdb
CFLAGS := $(CFLAGS) -O2

LDFLAGS := -pthread
LIBRARIES := -lm -lao -lpoly -lncurses
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
EXECUTABLE := noisetest

.PHONY: all clean

all: $(EXECUTABLE)

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJECTS) -o $@ $(LIBRARIES)

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
