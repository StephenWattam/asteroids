#include "physics.h"

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


/* ------------------------------------------------------------------------- */

// Move with toroidal wrapping
void pmove(positional_entity_t* e, bounds_t* b){
    // Update X
    e->x += e->dx;
    while(e->x > b->xmax) e->x -= b->width;    // wrapping >x
    while(e->x < b->xmin) e->x += b->width;    // wrapping <x

    // Update Y
    e->y += e->dy;
    while(e->y > b->ymax) e->y -= b->height;    // wrapping >y
    while(e->y < b->ymin) e->y += b->height;    // wrapping <y

    // Apply angular momentum
    e->orientation += e->dor;
    while(e->orientation < 0) e->orientation += 2*M_PI;
    while(e->orientation > 2*M_PI) e->orientation -= 2*M_PI;

    // TODO: make ->friction a float
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
