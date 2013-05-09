#define MAX_ENTITIES 100

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
positional_entity_t* new_ship(scene_t* scene, float x, float y, float size, bool collides, bool friction){
    return new_entity(scene, ship, x, y, 0, 0, 0.0, 0.0, size, -1, collides, friction);
}


scene_t* new_scene(){
    scene_t* new        = (scene_t*)malloc(sizeof(scene_t));

    // This one item is the player's ship.
    new->max            = 0;

    // Init null entities so we can be lazy later
    for(int i=0; i<MAX_ENTITIES; i++)
        new_entity_raw( &(new->entities[i]), null, 0, 0, 0, 0, 0.0, 0.0, 0, -1, false, false);
    
    // return pointer
    return new;
}
