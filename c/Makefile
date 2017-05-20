EXEC = ev3motor_copy

DEBUG =

CC = arm-linux-gnueabi-gcc
CXX = arm-linux-gnueabi-g++

ifeq ($(DEBUG), true)
	CFLAGS = -W -Wall -ansi -pedantic -g
	CXXFLAGS =
else
	CFLAGS = -W -Wall -ansi -pedantic
	CXXFLAGS =
endif

LDFLAGS = -lev3dev-c

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)


all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<


.PHONY: clean distclean release

clean:
	rm -r *.o
	rm $(EXEC)

distclean: clean
	rm *~

release: 
	rm -r *.o
