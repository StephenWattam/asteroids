#include "entities.h"

// Shrink max to fit entity list in a scene.
void eoptimise_scene(scene_t* scene){
    int max=0;

    // Loop over and find highest id
    for(int i=0; i<=scene->max; i++)
        if(scene->entities[i].type != null)
            max = i;
    
    // Assign to max
    scene->max = max;
}


// Create a new entity and return a pointer to it.
void new_entity_raw(positional_entity_t* new, entity_type_t type, 
        float x, float y, float dx, float dy, 
        orientation_t orientation, orientation_t dorientation,
        float size,
        int lifespan, bool collides, bool friction){
    new->type   = type;
    new->x      = x;
    new->y      = y;
    new->dx     = dx;
    new->dy     = dy;
    new->orientation = orientation;
    new->dor    = dorientation;
    new->size   = size;
    new->lifespan = lifespan;
    new->collides = collides;
    new->friction = friction;
}

// Create a new entity in a given scene
positional_entity_t* new_entity(scene_t* scene, entity_type_t type, 
        float x, float y, float dx, float dy, 
        orientation_t orientation, orientation_t dorientation,
        float size,
        int lifespan, bool collides, bool friction){

    int spare = -1;
    for(int i=0; i<scene->max; i++)
        if(scene->entities[i].type == null){
            spare = i;
            break;
        }
    if(spare < 0) spare = scene->max + 1;

    // TODO: handle edge case where spare (positional_entity_t> MAX_ENTITIES
    if( spare > MAX_ENTITIES - 1 ) return (positional_entity_t*)0;

    // Keep track of max
    if(spare > scene->max)
        scene->max = spare;

    // create entity
    new_entity_raw( &scene->entities[spare], type, 
            x, y, dx, dy, 
            orientation, dorientation, 
            size, lifespan, collides, friction);

    return &scene->entities[spare];
}


// Create a new ship
positional_entity_t* enew_ship(scene_t* scene, float x, float y, float size, bool collides, bool friction){
    return new_entity(scene, ship, x, y, 0, 0, 0.0, 0.0, size, -1, collides, friction);
}


scene_t* enew_scene(){
    scene_t* new        = (scene_t*)malloc(sizeof(scene_t));

    // This one item is the player's ship.
    new->max            = 0;

    // Init null entities so we can be lazy later
    for(int i=0; i<MAX_ENTITIES; i++)
        new_entity_raw( &(new->entities[i]), null, 0, 0, 0, 0, 0.0, 0.0, 0, -1, false, false);
    
    // return pointer
    return new;
}



// Returns the x component of a given orientation as a unit vector
float eget_fwd_x(positional_entity_t* e, orientation_t o){
    return sinf(o);
}
float eget_fwd_y(positional_entity_t* e, orientation_t o){
    return cosf(o);
}


