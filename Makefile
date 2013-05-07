CC=gcc
CFLAGS=-g -std=c99
NAME=as

all:
		$(CC) $(CFLAGS) -o $(NAME) ass.c -lcurses -lm

clean:
		rm *.o


