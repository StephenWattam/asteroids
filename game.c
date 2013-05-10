#include "game.h"

// Current game state
game_state_t* current_game;

// Frees game memory
void gdestroy(){
    free(current_game);
}

// Make a new game with a given set of entities
void gnew(int width, int height){
    // Destroy current game if one exists
    if(current_game) gdestroy(); 

    // create new one in its stead
    game_state_t* new_game = (game_state_t*)malloc(sizeof(game_state_t));
    if(!new_game) fail("Cannot allocate new game memory\n");
    
    new_game->score      = 0;
    new_game->wave       = 0;
    new_game->lives      = START_LIVES;
    new_game->warps      = START_WARPS;
    new_game->scene      = enew_scene();
    new_game->damage     = 100.0;
    new_game->temporary_invulnerability = INVULNERABILITY_COUNTER;
    /* new_game->player     = &new_game->scene[0]; */

    // plane bounds.
    new_game->bounds.width  = width;
    new_game->bounds.height = height;

    new_game->bounds.xmin   = -width/2;
    new_game->bounds.ymin   = -height/2;
    new_game->bounds.xmax   = width/2;
    new_game->bounds.ymax   = height/2;

    // Create a new ship
    new_game->player = enew_ship(new_game->scene, 0, 0, SHIP_SIZE, SHIP_CAN_COLLIDE, SHIP_EXPERIENCES_FRICTION);

    /* new_game. */
    current_game = new_game;
}

void gredefine_bounds(int width, int height){
    if(height < MIN_GAME_HEIGHT || width < MIN_GAME_WIDTH) return;

    // plane bounds.
    current_game->bounds.width  = width;
    current_game->bounds.height = height;
    current_game->bounds.xmin   = -width/2;
    current_game->bounds.ymin   = -height/2;
    current_game->bounds.xmax   = width/2;
    current_game->bounds.ymax   = height/2;
}

void gplayer_shoot(){
    positional_entity_t* ship = current_game->player;

    // Create the bullet
    if(new_entity(current_game->scene, bullet, ship->x, ship->y, 
                eget_fwd_x(ship, ship->orientation) * BULLET_SPEED, 
                -1 * eget_fwd_y(ship, ship->orientation) * BULLET_SPEED, 
                ship->orientation, 0.0, // bullets don't rotate
                BULLET_SIZE,
                BULLET_LIFESPAN, true, false
                )){

        // Recoil if fired
        paccel( ship, 
                -1 * eget_fwd_x(ship, ship->orientation) * RECOIL_SPEED, 
                eget_fwd_y(ship, ship->orientation) * RECOIL_SPEED, 
                0 );
    }
}

void gplayer_rotate(float direction){
    paccel( current_game->player, 0, 0, direction);
}

void gplayer_accel(float speed){
    positional_entity_t* ship = current_game->player;
    paccel( ship, 
            eget_fwd_x(ship, ship->orientation) * speed, 
            -1 * eget_fwd_y(ship, ship->orientation) * speed, 
            0 );
}

// Warp the player somewhere random
void gwarp_player(){
    if(current_game->warps <= 0) return;

    // Randomly position player
    current_game->player->x = (((float)rand()/RAND_MAX) - 0.5) * current_game->bounds.width + current_game->bounds.xmin;
    current_game->player->y = (((float)rand()/RAND_MAX) - 0.5) * current_game->bounds.height + current_game->bounds.ymin;

    // Decrement allowed warps
    current_game->warps--;
}

// Damage the player's ship and, possibly, destroy it.
void hurt_player(){

    // If not invulnerable
    if(current_game->temporary_invulnerability < 0){
        // Apply damage
        current_game->damage -= DAMAGE_PER_COLLISION;

        // If over capacity, kill player and reset
        if(current_game->damage < 0){
            current_game->player->type = null;
            current_game->lives--;
            current_game->player = enew_ship(current_game->scene, 0, 0, SHIP_SIZE, SHIP_CAN_COLLIDE, SHIP_EXPERIENCES_FRICTION);

            // Accounting
            current_game->temporary_invulnerability = INVULNERABILITY_COUNTER;
            current_game->damage = 100;
        }
    }
}

// Floating rand from 0-1, handy for below
#define frand() ((float)rand()/RAND_MAX)
void gnew_wave(){

    float x, y, dx, dy, or, dor, size;

    //Increase wave count
    current_game->wave ++;
    
    // Add some asteroids
    for(int i=0; i<ASTEROIDS_PER_WAVE + (current_game->wave * ASTEROID_COUNT_INCREASE_PER_WAVE); i++){
        x       = frand() * (current_game->bounds.xmax - current_game->bounds.xmin) + current_game->bounds.xmin;
        y       = frand() * (current_game->bounds.ymax - current_game->bounds.ymin) + current_game->bounds.ymin;
        dx      = (frand() - 0.5) * (ASTEROID_MOTION_VARIABILITY + (ASTEROID_SPEED_INCREASE_PER_WAVE * current_game->wave));
        dy      = (frand() - 0.5) * (ASTEROID_MOTION_VARIABILITY + (ASTEROID_SPEED_INCREASE_PER_WAVE * current_game->wave));
        or      = frand() * (M_PI * 2);
        dor     = frand() * (M_PI * 2);
        size    = frand() * ASTEROID_SIZE_VARIABILITY + MIN_START_ASTEROID_SIZE + (ASTEROID_SIZE_INCREASE_PER_WAVE * current_game->wave);

        new_entity(current_game->scene, asteroid, 
                x, y, 
                dx, dy, 
                or, dor, 
                size, 
                -1, ASTEROIDS_COLLIDE, ASTEROIDS_EXPERIENCE_FRICTION);

        current_game->asteroid_count ++;
    }

    // Add on new lives and warps per wave
    current_game->lives += NEW_LIVES_PER_WAVE;
    current_game->warps += WARPS_PER_WAVE;
}


// Split asteroids into smaller ones
void split_asteroid(positional_entity_t* e){

    // Destroy old one
    current_game->asteroid_count--;
    e->type = null;

    int new_size;
    int num = ((int) (frand() * (ASTEROID_MAX_FRAG - ASTEROID_MIN_FRAG)) + ASTEROID_MIN_FRAG);
    for(int i=0; i<num; i++){
        // Create an asteroid with size = e->size/num
        
        new_size = frand() * e->size * ASTEROID_COLLISION_OVERUNITY;

        if(new_size > MIN_ASTEROID_SIZE){
            new_entity(current_game->scene, asteroid,
                    e->x + ((frand() - 0.5) * e->size*3),
                    e->y + ((frand() - 0.5) * e->size*3),
                    (frand() - 0.5) * (ASTEROID_COLLISION_EXPLOSIVENESS_BOOST + e->dx) * ASTEROID_COLLISION_EXPLOSIVENESS,
                    (frand() - 0.5) * (ASTEROID_COLLISION_EXPLOSIVENESS_BOOST + e->dy) * ASTEROID_COLLISION_EXPLOSIVENESS,
                    frand() * M_PI*2,
                    frand() * M_PI*2,
                    new_size,
                    -1, ASTEROIDS_COLLIDE, ASTEROIDS_EXPERIENCE_FRICTION);
            current_game->asteroid_count ++;
        }

    }
}


// Handle collision game logic
void collision_check(positional_entity_t* e, int self){

    float distance;
    positional_entity_t* o; // other

    for(int i=0; i<=current_game->scene->max; i++){
        if(current_game->scene->entities[i].type != null &&
           i != self){

            // Get a nicer name for the Other item
            o = &current_game->scene->entities[i];


            // Compute distance using pythag
            distance = sqrt(  (o->x - e->x) * (o->x - e->x) + 
                              (o->y - e->y) * (o->y - e->y) );


            // debug
            /* if(e->type == ship && o->type == asteroid){ */
            /*     printf("[%.0f, %.0f, %.1f, %s]", o->x, e->x, distance, (distance < (o->size/2 + e->size/2) ? "T" : "F")); */
            /* } */

            // Check 
            if(distance < (o->size/2 + e->size/2)){
                // collision
                // Firstly, check type
                if(e->type == bullet){

                    // Check what we hit
                    if(o->type == ship){
                        if(FRIENDLY_FIRE){
                            hurt_player();
                            e->type = null;
                        }
                    }else if(o->type == asteroid){
                        // Shooting an asteroid
                        e->type = null;
                        // Score player based on asteroid difficulty
                        current_game->score += o->dx + o->size;
                        split_asteroid(o);
                    }else if(o->type == bullet){
                        // Two bullets both die
                        e->type = null; 
                        o->type = null;
                    }   
                }else if(e->type == ship && o->type == asteroid){
                    // Kill player, I guess
                    hurt_player();
                }else{
                    // Asteroid on asteroid violence
                    if(e->type == asteroid && o->type == asteroid && ASTEROIDS_COLLIDE){
                        split_asteroid(e);
                        split_asteroid(o);
                    }
                }
            }
        }
    }
}


// Perform collision and wave logic.
// Returns false if the game should end.
bool handle_game_logic(){

    // Check the scene
    scene_t* scene = current_game->scene;
    if(!NO_COLLISIONS){
        // Check each colliding object
        for(int i=0; i<=scene->max; i++){
            if(scene->entities[i].type != null &&
               scene->entities[i].collides){
                collision_check(&scene->entities[i], i);
            }
        }
    }

    // Speed things up after killing objects
    eoptimise_scene(current_game->scene);

    // New wave if the asteroid count == 0
    if(current_game->asteroid_count == 0){
        current_game->score += SCORE_PER_WAVE;
        gnew_wave();
    }

    // Gradually remove invulnerability
    current_game->temporary_invulnerability--;

    // Check player has lives
    return current_game->lives >= 0;
}



// Returns the current game
game_state_t* gcurrent(){
    return current_game;
}

