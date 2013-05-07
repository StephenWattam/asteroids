#include <stdlib.h>
#include <time.h>  // for PRNG seeding
#include <ncurses.h>
#include "game.c"
#include "renderer.c"
#include "input.c"
#include "physics.c"

/* In 10ths of a second */
#define GAME_LOOP_DELAY 5

#define ROTATE_SPEED 0.004
#define ACCEL_SPEED 20 

// Bullet properties
// TODO: move into game.c
#define BULLET_LIFESPAN 500
#define BULLET_SPEED 200
#define RECOIL_SPEED 10

void game_loop(){
    int c = 0;
    positional_entity_t * ship;

    /* halfdelay( GAME_LOOP_DELAY ); */
    timeout( GAME_LOOP_DELAY );

	while((c = getch()) != KEY_F(1)){	
        piterate(gcurrent());
        rrender(gcurrent());
        if( ! handle_game_logic()) break;

        /* wprintw(rgetwin(), "%c", c); */
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
                new_entity(gcurrent()->scene, bullet, ship->x, ship->y, 
                                pget_fwd_x(ship, ship->orientation) * BULLET_SPEED, 
                                -1 * pget_fwd_y(ship, ship->orientation) * BULLET_SPEED, 
                                ship->orientation, 0.0, // bullets don't rotate
                                BULLET_SIZE,
                                BULLET_LIFESPAN, true, false
                        );

                // Recoil
                paccel( ship, 
                        -1 * pget_fwd_x(ship, ship->orientation) * RECOIL_SPEED, 
                        pget_fwd_y(ship, ship->orientation) * RECOIL_SPEED, 
                        0 );

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

    // Create a new game
    gnew();

    // Create some asteroids
    gnew_wave();

    // Take input
    game_loop();


    iteardown();    // input down
    rteardown();    // renderer down
	return 0;
}


