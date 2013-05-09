#include <math.h>
#include "entities.c"
// Handy for trig
#define M_PI 3.14159265358979323846264338327

// Sizes of things in the game world
#define SHIP_SIZE 2000
#define BULLET_SIZE 10

// Bullet properties
/* #define BULLET_LIFESPAN 500 */
/* #define BULLET_SPEED 200 */
/* #define RECOIL_SPEED 10 */

// and asteroid properties
#define ASTEROID_SIZE_VARIABILITY 2000
#define MIN_START_ASTEROID_SIZE 2000
#define ASTEROID_MOTION_VARIABILITY 200
#define ASTEROIDS_COLLIDE false 
#define ASTEROIDS_EXPERIENCE_FRICTION false 

#define MIN_ASTEROID_SIZE 500   // die if smaller than this
#define ASTEROID_MIN_FRAG 2 // minimum of two
#define ASTEROID_MAX_FRAG 4 // max of 4
#define ASTEROID_COLLISION_EXPLOSIVENESS 4 // inject energy during collisions 1=accurate
#define ASTEROID_COLLISION_OVERUNITY 1 // inject mass during collisions, 1= accurate

// Wave properties
#define ASTEROIDS_PER_WAVE 4
#define NEW_LIVES_PER_WAVE 1
#define SCORE_PER_WAVE 20000

// Game properties
#define START_LIVES 2
#define NO_COLLISIONS false // turn off collisions entirely
#define FRIENDLY_FIRE false // Can kill self with bullets
#define INVULNERABILITY_COUNTER 1000 // Can't die within this time
#define DAMAGE_PER_COLLISION 1 // damage this much per collision (in percent)


typedef struct bounds{
    int xmin;
    int xmax;
    int ymin;
    int ymax;
} bounds_t;

typedef struct game_state{

    // Game accounting
    int score;
    int lives;
    int wave;
    int temporary_invulnerability;
    float damage;

    // Number of asteriods remaining in the wave
    int asteroid_count;
  
    // Limits of the game playing field
    bounds_t bounds;

    // List of entities in game
    scene_t* scene;
    positional_entity_t* player;
} game_state_t;


// Current game state
game_state_t* current_game;

// Make a new game with a given set of entities
void gnew(int width, int height){
    game_state_t* new_game = (game_state_t*)malloc(sizeof(game_state_t));
    
    new_game->score      = 0;
    new_game->wave       = 0;
    new_game->lives      = START_LIVES;
    new_game->scene      = new_scene();
    new_game->damage     = 100.0;
    new_game->temporary_invulnerability = INVULNERABILITY_COUNTER;
    /* new_game->player     = &new_game->scene[0]; */


    // plane bounds.
    // Computed from an aspect ratio and a diagonal size
    new_game->bounds.xmin   = -width/2;
    new_game->bounds.ymin   = -height/2;
    new_game->bounds.xmax   = width/2;
    new_game->bounds.ymax   = height/2;

    // Create a new ship
    new_game->player = new_ship(new_game->scene, 0, 0, SHIP_SIZE);

    /* new_game. */
    current_game = new_game;
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
            current_game->player = new_ship(current_game->scene, 0, 0, SHIP_SIZE);

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

    for(int i=0; i<ASTEROIDS_PER_WAVE; i++){
        x = frand() * (current_game->bounds.xmax - current_game->bounds.xmin) + current_game->bounds.xmin;
        y = frand() * (current_game->bounds.ymax - current_game->bounds.ymin) + current_game->bounds.ymin;
        dx = (frand() - 0.5) * ASTEROID_MOTION_VARIABILITY;
        dy = (frand() - 0.5) * ASTEROID_MOTION_VARIABILITY;
        or = frand() * (M_PI * 2);
        dor = frand() * (M_PI * 2);
        size = frand() * ASTEROID_SIZE_VARIABILITY + MIN_START_ASTEROID_SIZE;

        new_entity(current_game->scene, asteroid, 
                x, y, 
                dx, dy, 
                or, dor, 
                size, 
                -1, ASTEROIDS_COLLIDE, ASTEROIDS_EXPERIENCE_FRICTION);

        current_game->asteroid_count ++;
    }

    current_game->wave ++;
    current_game->lives += NEW_LIVES_PER_WAVE;
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
                    (frand() - 0.5) * e->dx * ASTEROID_COLLISION_EXPLOSIVENESS,
                    (frand() - 0.5) * e->dy * ASTEROID_COLLISION_EXPLOSIVENESS,
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

            // Check 
            if(distance < (o->size + e->size)){
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
