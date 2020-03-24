CC=gcc
CFLAGS=-g --pedantic -Wall -Wextra -Wmissing-prototypes -std=c99
PROGRAM=oshell

# DO NOT MODIFY CC, CFLAGS and PROGRAM

all : $(PROGRAM)

$(PROGRAM): main.o oshell.o sys.o
						$(CC) -o $(PROGRAM) main.o oshell.o sys.o $(CFLAGS)
						
oshell.o: oshell.c oshell.h
					$(CC) -c oshell.c $(CFLAGS)

sys.o: sys.c sys.h
			 $(CC) -c sys.c $(CFLAGS)

main.o: main.c
			 $(CC) -c main.c $(CFLAGS)

clean :
				rm -f *.o *.bin core
