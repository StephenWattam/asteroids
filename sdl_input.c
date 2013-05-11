#include "input.h"

#include "SDL/SDL.h"

bool iinit(){
    return true;
}

// Poll every 10ms when "blocking"
#define BLOCKING_DELAY 10

// Somewhere to keep the Event
SDL_Event event;

// Delay
int delay = 0;

input_instruction_t iget(){

    if(delay<0){
        // Blocking mode
        while( ! SDL_PollEvent( &event ) )
            SDL_Delay( BLOCKING_DELAY );
    }else{
        // Timeout mode
        SDL_Delay( delay );
    }

    // Process SDL events into asteroids events
    if( SDL_PollEvent( &event ) ){
        if( event.type == SDL_KEYDOWN ){
            printf("****\n");
            printf("SDL: %d\n", event.key.keysym.sym );
            switch( event.key.keysym.sym ){
                case SDLK_UP:       return in_up;
                case SDLK_DOWN:     return in_down;
                case SDLK_LEFT:     return in_left;
                case SDLK_RIGHT:    return in_right;
                case SDLK_SPACE:    return in_fire;
                case SDLK_p:        return in_pause;
                case SDLK_q:        return in_quit;
                case SDLK_PLUS:     return in_zoom_in;
                case SDLK_MINUS:    return in_zoom_out;
            }
        }else if(event.type == SDL_QUIT )
            return in_quit;
    }

    // and lastly return nil
    return in_null;
}


void iset_timeout(int t){
    delay = t;
}

void iset_blocking(){
    // TODO: some way of blocking input
    delay = -1;
}

void iteardown(){
    // nothing to do
}
