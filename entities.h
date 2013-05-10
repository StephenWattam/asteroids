#ifndef ENTITIES
#define ENTITIES

#define MAX_ENTITIES 100


#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


typedef enum entity_type{
    ship,       // 0
    asteroid,   // 1
    bullet,     // 2
    null        // 3
} entity_type_t;

// Orientation from north, in radians.
typedef float orientation_t;

typedef struct positional_entity{
    entity_type_t type;
    float x;
    float y;
    // function pointer to render, perhaps?
    
    // Velocity, vector encoded
    float dx;
    float dy;

    // orientation.
    orientation_t orientation;
    orientation_t dor;  // angular momentum

    // Size and bounding box
    float size; // a radius for now...
    
    // Lifespan in physics ticks
    int lifespan;

    // True if this should collide with stuff, else false
    // NB: colliding entities can hit non-colliding ones,
    //     but not vice versa (NC+NC = nil, NC+C = C)
    bool collides;
    bool friction;

} positional_entity_t;


// Keeps a list of entities.
typedef struct scene{
    unsigned int max;               // Don't bother rendering over this number
    positional_entity_t entities[MAX_ENTITIES];  // pointer to head of array
} scene_t;




void eoptimise_scene(scene_t* scene);
void new_entity_raw(positional_entity_t* new, entity_type_t type, 
        float x, float y, float dx, float dy, 
        orientation_t orientation, orientation_t dorientation,
        float size,
        int lifespan, bool collides, bool friction);
positional_entity_t* new_entity(scene_t* scene, entity_type_t type, 
        float x, float y, float dx, float dy, 
        orientation_t orientation, orientation_t dorientation,
        float size,
        int lifespan, bool collides, bool friction);


positional_entity_t* enew_ship(scene_t* scene, float x, float y, float size, bool collides, bool friction);

float eget_fwd_x(positional_entity_t* e, orientation_t o);
float eget_fwd_y(positional_entity_t* e, orientation_t o);

scene_t* enew_scene();
#endif
