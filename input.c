#include "input.h"
#include <curses.h>

/* Initialise the input system */
bool iinit(){
    cbreak(); // don't wait for newline
    noecho();
    keypad(stdscr, TRUE);

    // Successful init
    return true;
}

// Get an input instruction
input_instruction_t iget(){
    int c = getch();

    switch(c){	
        case KEY_LEFT:
            return in_left;
        case KEY_RIGHT:
            return in_right;
        case KEY_UP:
            return in_up;
        case KEY_DOWN:
            return in_down;
        case ' ':
            return in_fire;
        case '-':
            return in_zoom_out;
        case '+':
            return in_zoom_in;
        case 'p':
            return in_pause;
        case KEY_F(1):
            return in_quit;
    }

    return in_null;
}

// set the timeout in ms
void iset_timeout(int t){
    timeout(t);
}

// Set infinitely blocking
void iset_blocking(){
    timeout(-1);
}

// Uninitialise input
void iteardown(){
    // do nothing
}
