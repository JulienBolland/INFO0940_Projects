CC=gcc
CFLAGS=-g --pedantic -Wall -Wextra -Wmissing-prototypes -std=c99
PROGRAM=oshell

# DO NOT MODIFY CC, CFLAGS and PROGRAM

all : $(PROGRAM)

$(PROGRAM): oshell.o main.o
						$(CC) -o $(PROGRAM) main.c oshell.c $(CFLAGS)

clean :
				rm -f *.o *.bin core
