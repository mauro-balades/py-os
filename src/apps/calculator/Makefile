CC = gcc
CFLAGS = -c -Wall
LFLAGS = -Wall
LIBS = -lm

calc: stack.o calculator.c
	$(CC) $(LFLAGS) -o calc calculator.c stack.c $(LIBS)

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) stack.c $(LIBS)

clean:
	rm -f *.o calc stack_test

.PHONY: calc
