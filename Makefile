CC=gcc
CFLAGS=-I. -g -std=c99 -lcurses -lm -lSDL -lSDL_gfx
DEPS=ass.c game.c entities.c physics.c sdl_input.c sdl_renderer.c bitmap.c util.c
OBJ=ass.c util.o game.o entities.o physics.o sdl_input.o sdl_renderer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

as: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o







