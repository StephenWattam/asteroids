CC=gcc
CFLAGS=-I. -g -std=c99 -lcurses -lm
DEPS=ass.c game.c entities.c physics.c input.c renderer.c bitmap.c util.c
OBJ=ass.c util.o game.o entities.o physics.o input.o renderer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

as: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o







