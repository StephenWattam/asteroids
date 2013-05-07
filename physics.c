#include <math.h>

// Handy for trig
#define M_PI 3.14159265358979323846264338327

// 0-1. 0 is low, 1 is high
#define CARTESIAN_FRICTION 0.001
#define ANGULAR_FRICTION 0.002

/* ------------------------------------------------------------------------- */
// Make an entity accelerate in a given direction
void paccel(positional_entity_t* e, float x, float y, orientation_t dor){
    e->dx   += x;
    e->dy   += y;
    e->dor  += dor;
}

// Set velocity
void pvel(positional_entity_t* e, float x, float y, orientation_t dor){
    e->dx   = x;
    e->dy   = y;
    e->dor  = dor;
}


// Returns the x component of a given orientation as a unit vector
float pget_fwd_x(positional_entity_t* e, orientation_t o){
    return sinf(o);
}
float pget_fwd_y(positional_entity_t* e, orientation_t o){
    return cosf(o);
}


/* ------------------------------------------------------------------------- */

// Move with toroidal wrapping
// TODO: move this to be a division-based algorithm? it's fast but it isn't totally 
// foolproof for very fast things.
void pmove(positional_entity_t* e, bounds_t* b){
    // Update X
    e->x += e->dx;
    if(e->dx > 0 && e->x > b->xmax) e->x -= (b->xmax - b->xmin);    // wrapping >x
    if(e->dx < 0 && e->x < b->xmin) e->x += (b->xmax - b->xmin);    // wrapping <x

    // Update Y
    e->y += e->dy;
    if(e->dy > 0 && e->y > b->ymax) e->y -= (b->ymax - b->ymin);    // wrapping >y
    if(e->dy < 0 && e->y < b->ymin) e->y += (b->ymax - b->ymin);    // wrapping <y

    // Apply angular momentum
    e->orientation += e->dor;
    while(e->orientation < 0) e->orientation += 2*M_PI;
    while(e->orientation > 2*M_PI) e->orientation -= 2*M_PI;

    if(e->friction){
        // Apply friction to deltas
        // this is complicated by the use of integer positioning.
        e->dx *= (1-CARTESIAN_FRICTION);
        e->dy *= (1-CARTESIAN_FRICTION);
        e->dor *= (1-ANGULAR_FRICTION);
    }
}

// Iterate all objects in the entity list
void piterate(game_state_t* current){
    for(int i=0; i<=current->scene->max; i++){
        if(current->scene->entities[i].type != null){
            pmove( &current->scene->entities[i], &current->bounds );

            // Update lifespan
            if(current->scene->entities[i].lifespan > 0){
                current->scene->entities[i].lifespan --;

                // If below zero set object to null
                if(current->scene->entities[i].lifespan == 0){
                    current->scene->entities[i].type = null;   
                    current->scene->entities[i].lifespan = -1;
                }
            }
        }
    }
}
