#include <math.h> //rounding
#include "resource.c"

#define M_PI 3.14159265358979323846264338327

// Where to find graphical resources.
// Not used if images are turned off.
#define SHIP_FILEPATH "./resources/ship.bmp"
#define ASTEROID_FILEPATH "./resources/asteroid.bmp"

// This saves us some pain,
// but lowers the beauty.
//
/* #define ASTEROIDS_ROTATE  */


// ncurses resources
WINDOW* main_win;
WINDOW* main_border;
WINDOW* score_win;


// Transformation variables, re-used during rendering.
float sw, sh; // scale factors
float tx, ty; // translations


// Bitmaps for ships, asteroids.
bitmap_t* ship_bmp;
bitmap_t* asteroid_bmp;

// Caches.
bitmap_t* ship_cache;

#ifdef ASTEROIDS_ROTATE 
bitmap_t* asteroid_cache;
#endif

/* ------------------------------------------------------------------------- */

/* Initialise the rendering system */
void rinit(){

    // Start curses mode
    initscr();			

    // Refresh for some reason
	refresh();

    // Construct windows
    main_win    = newwin( LINES - 5, COLS-2, 4, 1);
    main_border = newwin( LINES - 3, COLS, 3, 0 );
    score_win   = newwin( 3, COLS, 0, 0 );

    // Set backgrounds
    wbkgdset(main_win, ' ');
    wbkgdset(score_win, ' ');

    // Load bitmaps (TODO: make this gooderer.)
    ship_bmp = load_resource(SHIP_FILEPATH);
    asteroid_bmp = load_resource(ASTEROID_FILEPATH);

    // TODO: some kind of function to do this for me
    // FIXME: re-use a cache, THIS WILL LEAK LIKE FUCK.
    ship_cache          = new_bmp(ship_bmp->width, ship_bmp->height);
#ifdef ASTEROIDS_ROTATE 
    asteroid_cache          = new_bmp(asteroid_bmp->width, asteroid_bmp->height);
#endif
}

/* Tear down the rendering system */
void rteardown(){
	endwin();			/* End curses mode		  */

    // Free bitmap resources
    free_bmp(ship_bmp);
    free_bmp(ship_cache);
    free_bmp(asteroid_bmp);

#ifdef ASTEROIDS_ROTATE 
    free_bmp(asteroid_cache);
#endif
}

/* ------------------------------------------------------------------------- */
// Transforms and coordinate conversion

void compute_tranforms(bounds_t* bounds){
    // Find window bounds of curses window
    int wcmax, wrmax;
    getmaxyx(main_win, wrmax, wcmax);

    // Compute centre
    float wcc = wcmax/2.0;
    float wrc = wrmax/2.0;
   
    // Compute bound centre
    float bxc = (bounds->xmax + bounds->xmin) / 2.0;
    float byc = (bounds->ymax + bounds->ymin) / 2.0;

    // Compute bound width, height
    float bw  = bounds->xmax - bounds->xmin;
    float bh  = bounds->ymax - bounds->ymin;
   
    // Compute scale
    sw  = wcmax / bw;
    sh  = wrmax / bh;

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


// Plot at r, c on the curses plane
void plot_with_wrap(int c, int r, char ch){
    int wcmax, wrmax;
    getmaxyx(main_win, wrmax, wcmax);

    // and t'other way
    while(c >= wcmax) c -= wcmax;
    while(r >= wrmax) r -= wrmax;

    // Wrap
    while(c < 0) c += wcmax;
    while(r < 0) r += wrmax;

    // Then plot
    mvwprintw(main_win, r, c, "%c", ch); 
}




/* ------------------------------------------------------------------------- */
// Ordinary rendering

// Return the ship's character for a given rotation
#define PIPI (2*M_PI)
char get_ship_char( positional_entity_t* e, float x, float y){
    orientation_t eo = e->orientation;
    
    char c = 's';
    if(eo < (1 * PIPI/8) || eo > (7 * PIPI/8) ) c = '^';     
    if(eo > (1 * PIPI/8) && eo < (3 * PIPI/8) ) c = '>';
    if(eo > (3 * PIPI/8) && eo < (5 * PIPI/8) ) c = 'V';
    if(eo > (5 * PIPI/8) && eo < (7 * PIPI/8) ) c = '<';

    mvwprintw( main_win, round(y), round(x), "%c", c);

    return c;
}


/* ------------------------------------------------------------------------- */
// Bitmap rendering support
//
//
// Rotate around centre
// Thanks to http://www.programmersheaven.com/download/15441/0/ZipView.aspx
void rotate(bitmap_t* bmp, bitmap_t* newbmp, orientation_t orientation){

    // todo: Check newbmp is the same size...?

    // Precompute these
    float cosT = cos(orientation);
    float sinT = sin(orientation);

    // Loop over each pixel, with a half offset to move the origin
    int srcx, srcy;
    for(int i=-bmp->width/2; i<bmp->width/2; i++){
        for(int j=-bmp->height/2; j<bmp->height/2; j++){

            // Do magic stuff
            srcx = i*cosT + j*sinT + bmp->width/2.0;
            srcy = j*cosT - i*sinT + bmp->height/2.0;

            // Check it's in the image bounds
            if(srcx >= 0 && srcx < bmp->width &&
               srcy >= 0 && srcy < bmp->height){
               
                // Get from src, put into i+offsetx, j+offsety
                bmp_put( newbmp, 
                        i + newbmp->width/2, 
                        j + newbmp->height/2, 
                        bmp_get(bmp, srcx, srcy)
                        );

            }
        }
    }
}


// Render a bitmap using sampling at a given place
void subpixel( bitmap_t* bmp, positional_entity_t* e, char ch ){

    // Compute location in curses-space
    float x  = trans_horz(e->x);
    float y  = trans_vert(e->y);

    // Compute size in curses-space
    float w  = scale_horz(e->size);
    float h  = scale_vert(e->size);

    // Iterate char-by-char and see if each point is in the 
    // bitmap.  If it is, plot it.
    for(int i=floor(x - (w/2)); i<=ceil(x + (w/2)); i++){
        for(int j=floor(y - (h/2)); j<=ceil(y + (h/2)); j++){

            // Re-normalise for 0,0 addressing
            //
            // TODO: this is very carefully limited to avoid
            // oversampling the bitmap,
            // but it is a big mash of float error....
            float bmpx = ((i - x) + (w/2));
            float bmpy = ((j - y) + (h/2));

            if(bmpx < 0) bmpx = 0;
            if(bmpy < 0) bmpy = 0;

            // Then move from curses to bitmap address space
            bmpx /= w;
            bmpy /= h;

            bmpx *= bmp->width;
            bmpy *= bmp->height;

            if(bmpx > bmp->width  -1) bmpx = bmp->width  - 1;
            if(bmpy > bmp->height -1) bmpy = bmp->height - 1;

            /* unsigned int bmpx = ((((i - x) + (w/2)) / w) * (float)bmp->width) - 1; */
            /* unsigned int bmpy = ((((j - y) + (h/2)) / h) * (float)bmp->height) - 1; */
              /* debug: mvwprintw(main_win, j, i, "[%.1f,%.1f]", bmpx, bmpy); */

            // If this pixel is on, plot it on the curses plane
            if(bmp_get( bmp, bmpx, bmpy ))
                plot_with_wrap(i, j, ch);
        }
    }
}

// Renders a given entity using whatever method is appropriate
void render_entity( positional_entity_t* e ){

    // Compute actual values
    float ex    = trans_horz(e->x);
    float ey    = trans_vert(e->y);


    // Non-bitmap rendering
    if(e->type == bullet){
        mvwprintw( main_win, round(ey), round(ex), "*");
        return;
    }


    // Bitmap rendering....
    switch(e->type){
        case ship:
            rotate( ship_bmp, ship_cache, e->orientation );
            char ch = get_ship_char(e, ex, ey);
            subpixel(ship_cache, e, ch);
            mvwprintw( main_win, round(ey), round(ex), "%c", ch);
            break;
        case asteroid:
#ifdef ASTEROIDS_ROTATE
            rotate( asteroid_bmp, asteroid_cache, e->orientation );
            subpixel(asteroid_cache, e, 'a');
#else
            subpixel(asteroid_bmp, e, 'a');
#endif
            mvwprintw( main_win, round(ey), round(ex), "a");
            break;
    }
}


/* Render the game */
void rrender(game_state_t* current){
    // Blank the windowas
    werase(main_win);
    werase(score_win);


    compute_tranforms(&current->bounds);
    // TODO: check if terminal has been resized( is_term_resized, man resizeterm(3x) and wresize).

    // Write score from game state
    mvwprintw(score_win, 1, 1, "Lives: %d \t Score: %d \t Wave: %d \t Ship: %.0f%% (%.1f, %.1f) %s %d,%d %d,%d", 
            current->lives, current->score, current->wave, 
            current->damage,
            current->player->dx,
            -1 * current->player->dy,
            ((current->temporary_invulnerability > 0) ? "(inv.)" : ""),
            current->bounds.xmin, current->bounds.ymin,
            current->bounds.xmax, current->bounds.ymax
            );


    // Loop over entities in game and render them if not null
    for(int i=0; i<=current->scene->max; i++){
        /* mvwprintw(main_win, 0, 0, "[i%d/e%d]", i, current->scene->count); */
        if(current->scene->entities[i].type != null){
            render_entity( &current->scene->entities[i] );
        }
    }

    // Draw curses cruft.
    box(main_border, 0, 0);
    box(score_win, 0, 0);
    wrefresh(main_border);
    wrefresh(score_win);
    wrefresh(main_win);

	refresh();			/* Print it on to the real screen */
}

/* ------------------------------------------------------------------------- */

void rsummarise_game( game_state_t* current ){
    
    // Cnstruct windows
    WINDOW* sum_win   = newwin( 13, 30, 6, 6 );
    wbkgdset(main_win, ' ');
    werase(sum_win);
    box(sum_win, 0, 0);

    // Write summary info
    mvwprintw(sum_win, 1, 11, "Game Over");
    mvwprintw(sum_win, 2, 1, "----------------------------");
    mvwprintw(sum_win, 4, 4, "Score: %d", current->score);
    mvwprintw(sum_win, 5, 4, "Waves: %d", current->wave);
    /* mvwprintw(sum_win, 1, 9, ""); */
    mvwprintw(sum_win, 7, 2, "Brought to you by Steve W");
    mvwprintw(sum_win, 8, 4, "<stephenwattam.com>");
    mvwprintw(sum_win, 10, 5, "Press a key to quit.");


    // Refresh
    wrefresh(sum_win);
    refresh();
}

/* ------------------------------------------------------------------------- */

// Width of render pane, i.e. game world
int rget_width(){
    int wcmax, wrmax;
    getmaxyx(main_win, wrmax, wcmax);
    return wcmax;
}
int rget_height(){
    int wcmax, wrmax;
    getmaxyx(main_win, wrmax, wcmax);
    return wrmax;
}
