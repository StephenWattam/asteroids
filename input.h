


#ifndef INPUT
#define INPUT

#include <stdbool.h>

typedef enum input_instruction{
    in_null,
    in_up,
    in_down,
    in_left,
    in_right,
    in_fire,
    in_pause,
    in_quit,
    in_zoom_in,
    in_zoom_out
} input_instruction_t;


bool iinit();
input_instruction_t iget();
void iset_timeout(int t);
void iset_blocking();
void iteardown();

#endif 
