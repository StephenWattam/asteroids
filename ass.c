#include "util.c"
#include <stdlib.h>
#include <time.h>  // for PRNG seeding
#include <ncurses.h>
#include "game.c"
#include "renderer.c"
#include "input.c"
#include "physics.c"

/* GAME_SCALE */
#define GAME_SCALE 400  /* Relative to renderer window size, so 'how many plane units per char' */
#define ZOOM_AMOUNT 0.1     /* Change the plane size by this amount each time */


/* In 10ths of a second */
#define GAME_LOOP_DELAY 10 

#define ROTATE_SPEED 0.004
#define ACCEL_SPEED 10 

// Bullet properties
// TODO: move into game.c
#define BULLET_LIFESPAN 500
#define BULLET_SPEED 200
#define RECOIL_SPEED 5


void game_loop(){
    input_instruction_t c = null;
    positional_entity_t * ship;

    iset_timeout( GAME_LOOP_DELAY );

	while((c = iget()) != in_quit){	

        // Iterate the game
        rrender(gcurrent());                // Render
        piterate(gcurrent());               // Physics
        if( ! handle_game_logic()) break;   // Game logic

        switch(c){	
            case in_left:
                // Rotate left
                ship = (gcurrent())->player;
                paccel( ship, 0, 0, -1 * ROTATE_SPEED);
				break;
			case in_right:
                // Rotate right 
                ship = (gcurrent())->player;
                paccel( ship, 0, 0, ROTATE_SPEED);
				break;
			case in_up:
                // Accelerate 'forwards' based on current orientation
                ship = (gcurrent())->player;
                paccel( ship, 
                        pget_fwd_x(ship, ship->orientation) * ACCEL_SPEED, 
                        -1 * pget_fwd_y(ship, ship->orientation) * ACCEL_SPEED, 
                        0 );
				break;
			case in_down:
                // warp
                gwarp_player();
				break;	
            case in_fire:
                // TODO: move into game.c
                // Shoot
                ship = (gcurrent())->player;

                // Create the bullet
                if(new_entity(gcurrent()->scene, bullet, ship->x, ship->y, 
                                pget_fwd_x(ship, ship->orientation) * BULLET_SPEED, 
                                -1 * pget_fwd_y(ship, ship->orientation) * BULLET_SPEED, 
                                ship->orientation, 0.0, // bullets don't rotate
                                BULLET_SIZE,
                                BULLET_LIFESPAN, true, false
                        )){

                    // Recoil
                    paccel( ship, 
                            -1 * pget_fwd_x(ship, ship->orientation) * RECOIL_SPEED, 
                            pget_fwd_y(ship, ship->orientation) * RECOIL_SPEED, 
                            0 );
                }

                break;
            case in_zoom_out:
                // Zoom out/make plane larger
                gredefine_bounds( gcurrent()->bounds.width * (1+ZOOM_AMOUNT), 
                        gcurrent()->bounds.height * (1+ZOOM_AMOUNT) );
                break;
            case in_zoom_in:
                // Zoom in/make plane smaller
                gredefine_bounds( gcurrent()->bounds.width * (1-ZOOM_AMOUNT), 
                        gcurrent()->bounds.height * (1-ZOOM_AMOUNT) );
                break;
            case in_pause:
                // Pause
                rpause_dialog( gcurrent() );
                iset_blocking();
                iget();
                rclear_pause_dialog();
                timeout( GAME_LOOP_DELAY );
                break;
		}
	}


    // Summary
    rsummarise_game( gcurrent() );
    iset_blocking();
    iget();
}

int main()
{

    // Seed the PRNG
    srand(time(NULL));

    // Pull up renderer and input system
    if(!rinit()) fail("Failed to initialise renderer\n");    // renderer
    if(!iinit()) fail("Failed to initialise input system\n");    //input

    // Create a new game,
    // taking note of the aspect ratio of the screen
    gnew(rget_width()*GAME_SCALE, rget_height()*GAME_SCALE);

    // Create some asteroids
    gnew_wave();

    // Take input
    game_loop();

    // Dealloc game
    gdestroy();  // Dealloc gamgdestroy();;

    iteardown();    // input down
    rteardown();    // renderer down
	return 0;
}


