

#ifndef GAME
#define GAME

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "entities.h"
#include "util.h"


#define M_PI 3.14159265358979323846264338327

// Sizes of things in the game world
#define SHIP_SIZE 2000
#define BULLET_SIZE 10


// Cannot make the plane below this size
#define MIN_GAME_WIDTH 10000 
#define MIN_GAME_HEIGHT 10000

// Bullet properties
/* #define BULLET_LIFESPAN 500 */
/* #define BULLET_SPEED 200 */
/* #define RECOIL_SPEED 10 */

// and asteroid properties
#define ASTEROID_SIZE_VARIABILITY 2000
#define MIN_START_ASTEROID_SIZE 2000
#define ASTEROID_MOTION_VARIABILITY 150 
#define ASTEROIDS_COLLIDE false 
#define ASTEROIDS_EXPERIENCE_FRICTION false 

#define MIN_ASTEROID_SIZE 500   // die if smaller than this
#define ASTEROID_MIN_FRAG 2 // minimum of two
#define ASTEROID_MAX_FRAG 4 // max of 4
#define ASTEROID_COLLISION_EXPLOSIVENESS 4 // inject energy during collisions 1=accurate
#define ASTEROID_COLLISION_EXPLOSIVENESS_BOOST 10 // inject energy during collisions 0=accurate
#define ASTEROID_COLLISION_OVERUNITY 1 // inject mass during collisions, 1= accurate

// Wave properties
#define ASTEROIDS_PER_WAVE 4
#define NEW_LIVES_PER_WAVE 1
#define SCORE_PER_WAVE 20000
#define WARPS_PER_WAVE 2
#define ASTEROID_SPEED_INCREASE_PER_WAVE 20 // 30pt/tick faster
#define ASTEROID_SIZE_INCREASE_PER_WAVE 30  // 30pt bigger per wave, randomly
#define ASTEROID_COUNT_INCREASE_PER_WAVE 1  // One new asteroid per wave

// Game properties
#define START_LIVES 2
#define NO_COLLISIONS false // turn off collisions entirely
#define FRIENDLY_FIRE false // Can kill self with bullets
#define INVULNERABILITY_COUNTER 600 // Can't die within this time
#define DAMAGE_PER_COLLISION 1 // damage this much per collision (in percent)
#define SHIP_CAN_COLLIDE true   // Set to false for uber-cheat mode
#define SHIP_EXPERIENCES_FRICTION true // should the ship slow down?
#define START_WARPS 0

// Bullet properties
#define BULLET_LIFESPAN 500
#define BULLET_SPEED 200
#define RECOIL_SPEED 5

#include "entities.h"


typedef struct bounds{
    int width;
    int height;
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
    int warps;
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


#include "physics.h"


void gdestroy();
void gnew(int width, int height);
void gredefine_bounds(int width, int height);
void gplayer_shoot();
void gwarp_player();
void hurt_player();
void gnew_wave();
bool handle_game_logic();
game_state_t* gcurrent();
void gplayer_rotate(float direction);
void gplayer_accel(float speed);
#endif
