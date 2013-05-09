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
#define GAME_LOOP_DELAY 5

#define ROTATE_SPEED 0.004
#define ACCEL_SPEED 20 

// Bullet properties
// TODO: move into game.c
#define BULLET_LIFESPAN 500
#define BULLET_SPEED 200
#define RECOIL_SPEED 5


void game_loop(){
    int c = 0;
    positional_entity_t * ship;

    /* halfdelay( GAME_LOOP_DELAY ); */
    timeout( GAME_LOOP_DELAY );

	while((c = getch()) != KEY_F(1)){	
        piterate(gcurrent());
        rrender(gcurrent());
        if( ! handle_game_logic()) break;

        switch(c){	
            case KEY_LEFT:
                // Rotate left
                ship = (gcurrent())->player;
                paccel( ship, 0, 0, -1 * ROTATE_SPEED);
				break;
			case KEY_RIGHT:
                // Rotate right 
                ship = (gcurrent())->player;
                paccel( ship, 0, 0, ROTATE_SPEED);
				break;
			case KEY_UP:
                // Accelerate 'forwards' based on current orientation
                ship = (gcurrent())->player;
                paccel( ship, 
                        pget_fwd_x(ship, ship->orientation) * ACCEL_SPEED, 
                        -1 * pget_fwd_y(ship, ship->orientation) * ACCEL_SPEED, 
                        0 );
				break;
			/* case KEY_DOWN: */
                // bugger all.
			/* 	break;	 */
            case ' ':
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
            case '-':
                // Zoom out/make plane larger
                gredefine_bounds( gcurrent()->bounds.width * (1+ZOOM_AMOUNT), 
                        gcurrent()->bounds.height * (1+ZOOM_AMOUNT) );
                break;
            case '+':
                // Zoom in/make plane smaller
                gredefine_bounds( gcurrent()->bounds.width * (1-ZOOM_AMOUNT), 
                        gcurrent()->bounds.height * (1-ZOOM_AMOUNT) );
                break;
            case 'p':
                // Pause
                rpause_dialog( gcurrent() );
                timeout(-1);
                c = getch();
                rclear_pause_dialog();
                timeout( GAME_LOOP_DELAY );
                break;
                
		}
	}


    // Summary
    rsummarise_game( gcurrent() );
    timeout(-1);
    c = getch();
}

int main()
{
    // Seed the PRNG
    srand(time(NULL));

    rinit();    // renderer
    iinit();    //input


    // Create a new game,
    // taking note of the aspect ratio of the screen
    gnew(rget_width()*GAME_SCALE, rget_height()*GAME_SCALE);

    // Create some asteroids
    gnew_wave();

    // Take input
    game_loop();


    iteardown();    // input down
    rteardown();    // renderer down
	return 0;
}


