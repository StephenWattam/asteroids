


#ifndef RENDERER
#define RENDERER

#include "game.h"
#include "entities.h"



bool rinit();
void rteardown();
void rrender(game_state_t* current);
void rpause_dialog( game_state_t* current );

void rclear_pause_dialog();
void rsummarise_game( game_state_t* current );


float rget_aspect();

#include <math.h>
float rget_diag_size();

#endif
