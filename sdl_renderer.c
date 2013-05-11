#include "renderer.h"
#include <math.h> //rounding



#include "SDL/SDL.h"
#include "SDL/SDL_rotozoom.h"   // bitmap rotation



SDL_Surface* bmp_asteroid;
SDL_Surface* bmp_ship;
SDL_Surface* bmp_bullet;


SDL_Surface* screen;

SDL_Surface* main_win;
struct SDL_Rect main_pos = {0, 50, 0, 0};

SDL_Surface* score_win;



/* ------------------------------------------------------------------------- */
// Transforms and coordinate conversion

float sw, sh, tx, ty;

void compute_tranforms(bounds_t* bounds){

    // Compute centre
    float wcc = main_win->w/2.0;
    float wrc = main_win->h/2.0;
   
    // Compute bound centre
    float bxc = (bounds->xmax + bounds->xmin) / 2.0;
    float byc = (bounds->ymax + bounds->ymin) / 2.0;

    // Compute scale
    sw  = (float)main_win->w / (float)bounds->width;
    sh  = (float)main_win->h / (float)bounds->height;

    // Compute translation
    // TODO/FIXME: currently clamped to 0, 0
    tx    = wcc;
    ty    = wrc;

}

// Convert from curses space to game space
float inv_trans_horz(float c){
    return (c - tx) / sw;
}
float inv_trans_vert(float r){
    return (r - ty) / sh;
}

// Full transform from game->curses
float trans_horz(float x){
    return x * sw + tx;
}
float trans_vert(float y){
    return y * sh + ty;
}

// Scale only, no translation from game->curses
float scale_horz(float x){
    return x * sw;
}
float scale_vert(float y){
    return y * sh;
}


/* ------------------------------------------------------------------------- */




bool rinit(){
    SDL_Init( SDL_INIT_EVERYTHING );

    screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );

    bmp_asteroid = SDL_LoadBMP("./resources/asteroid.bmp");
    bmp_ship     = SDL_LoadBMP("./resources/ship.bmp");
    bmp_bullet   = SDL_LoadBMP("./resources/bullet.bmp");

    // Create surfaces
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    score_win = SDL_CreateRGBSurface(0, screen->w, main_pos.y, 32, rmask, gmask, bmask, amask);
    if(!score_win) return false;
    main_win =  SDL_CreateRGBSurface(0, screen->w, screen->h - score_win->h, 32, rmask, gmask, bmask, amask);


    return (main_win != 0);
}

//
void rteardown(){
    SDL_FreeSurface( bmp_asteroid );
    SDL_FreeSurface( bmp_ship );
    SDL_FreeSurface( bmp_bullet );
    SDL_FreeSurface( main_win );
    SDL_FreeSurface( score_win );
    SDL_Quit();
}




// Renders a given entity using whatever method is appropriate
//
// TODO: See http://www.libsdl.org/docs/html/sdlblitsurface.html about making black transparent.
void render_entity( positional_entity_t* e ){

    // Compute actual values
    float ex    = trans_horz(e->x);
    float ey    = trans_vert(e->y);

    // Find top-left x-y for the entity
    struct SDL_Rect dst = {round(trans_horz(e->x)) - round(scale_horz(e->size)/2), 
                           round(trans_horz(e->y)) - round(scale_vert(e->size)/2), 0, 0};

    

    // Bitmap rendering....
    SDL_Surface* bmp;
    switch(e->type){
        case ship:
            printf("--> (%f, %f), %d %d\n", e->orientation, e->y, dst.x, dst.y);
            bmp = bmp_ship;
            break;
        case asteroid:
            bmp = bmp_asteroid;
            break;
        case bullet:
            bmp = bmp_bullet;
            break;
    }
    

    // TODO: scale, rotate (SDL_gfx ?)
    //
    // TODO: take into account original bitmap size (current scale factor is way off)
		/* (SDL_Surface * src, double angle, double zoomx, double zoomy, int smooth); */
    SDL_Surface* rotated = rotozoomSurfaceXY( bmp, 0.0, //e->orientation * (180.0/M_PI), 
            1.,  /* sw * (bitmap->width / e->size) */
            1., /* sh * (bitmap->height / e->size) */
            0 // SMOOTHING_ON
            );


    printf("Rotated: %d %d\n", rotated->w, rotated->h);
    
    // Draw
    SDL_BlitSurface( bmp, NULL, main_win, &dst);

    // TODO: don't alloc/free over and over...
    SDL_FreeSurface( rotated );
}





void rrender(game_state_t* current){

    compute_tranforms(&current->bounds);

    // Loop over entities in game and render them if not null
    for(int i=0; i<=current->scene->max; i++){
        /* mvwprintw(main_win, 0, 0, "[i%d/e%d]", i, current->scene->count); */
        if(current->scene->entities[i].type != null){
            render_entity( &current->scene->entities[i] );
        }
    }


    SDL_BlitSurface( score_win, NULL, screen, NULL);
    SDL_BlitSurface( main_win, NULL, screen, &main_pos);

    // Update
    SDL_Flip( screen );
}



void rpause_dialog( game_state_t* current ){
    // Pause
}

void rclear_pause_dialog(){
    // unpause
}


void rsummarise_game( game_state_t* current ){
    // print summary
}


float rget_aspect(){
    return (float)main_win->w / (float)main_win->h;
}

#include <math.h>
// Returns in inches
//
// Assume 90dpi, which means around....
float rget_diag_size(){
    return sqrt( pow((float)main_win->w, 2) + pow((float)main_win->h, 2) ) / 90.0;
}

/* ------------------------------------------------------------------------- */



