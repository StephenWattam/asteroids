
#ifndef PHYSICS 
#define PHYSICS

#include <math.h>
#include "entities.h"

#define M_PI 3.14159265358979323846264338327

// FIXME: remove these...
#define CARTESIAN_FRICTION 0.001
#define ANGULAR_FRICTION 0.002

void paccel(positional_entity_t* e, float x, float y, orientation_t dor);
void pvel(positional_entity_t* e, float x, float y, orientation_t dor);


#include "game.h"
void piterate(game_state_t* current);

#endif
