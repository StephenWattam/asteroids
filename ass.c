#include <stdlib.h>
#include <time.h>  // for PRNG seeding
#include "game.h"
#include "renderer.h"
#include "input.h"
#include "util.h"

/* GAME_SCALE */
#define GAME_RESOLUTION 4000  /* Relative to renderer window size, so 'how many plane units per inch' */
#define ZOOM_AMOUNT 0.1     /* Change the plane size by this amount each time */


/* In 10ths of a second */
#define GAME_LOOP_DELAY 10 

#define ROTATE_SPEED 0.004
#define ACCEL_SPEED 10 

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
                gplayer_rotate(-1 * ROTATE_SPEED);
				break;
			case in_right:
                // Rotate right 
                gplayer_rotate(ROTATE_SPEED);
				break;
			case in_up:
                // Accelerate 'forwards' based on current orientation
                gplayer_accel( ACCEL_SPEED );
				break;
			case in_down:
                // warp
                gwarp_player();
				break;	
            case in_fire:
                // Shoot
                gplayer_shoot();
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
                iset_timeout( GAME_LOOP_DELAY );
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
    float aspect            = rget_aspect();
    float diagonal_length   = rget_diag_size() * GAME_RESOLUTION; // points per inch...
    gnew( 
            diagonal_length / sqrt( pow(1/aspect, 2) + 1),
            diagonal_length / sqrt( pow(aspect, 2) + 1)
        );

    // Create some asteroids
    gnew_wave();

    // Take input
    game_loop();

    // Dealloc game
    gdestroy();  

    iteardown();    // input down
    rteardown();    // renderer down
	return 0;
}


