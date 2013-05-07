
/* Initialise the input system */
void iinit(){
    cbreak(); // don't wait for newline
    noecho();
    keypad(stdscr, TRUE);
}

void iteardown(){
    // do nothing
}
