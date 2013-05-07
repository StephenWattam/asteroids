
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h> // C99

#pragma pack(push,1)
/* Windows 3.x bitmap file header */
typedef struct {
    char         filetype[2];   /* magic - always 'B' 'M' */
    unsigned int filesize;
    short        reserved1;
    short        reserved2;
    unsigned int dataoffset;    /* offset in bytes to actual bitmap data */
} file_header;

/* Windows 3.x bitmap full header, including file header */
typedef struct {
    file_header  fileheader;
    unsigned int headersize;
    int          width;
    int          height;
    short        planes;
    short        bitsperpixel;  /* we only support the value 24 here */
    unsigned int compression;   /* we do not support compression */
    unsigned int bitmapsize;
    int          horizontalres;
    int          verticalres;
    unsigned int numcolors;
    unsigned int importantcolors;
} bitmap_header;
#pragma pack(pop)


typedef struct bitmap{
    int width;
    int height;
    bool* bits;
} bitmap_t;

// FIXME: check the mallocs and reads work,
// and all the file ops return sensible amounts
bitmap_t* load_resource(char* filepath){
   
    // Open the file
    FILE * fp = fopen(filepath, "rb");

    // Alloc and then read into the header format
    bitmap_header* head = malloc(sizeof(bitmap_header));
    fread(head, sizeof(bitmap_header), 1, fp);

    // Read image data
    fseek(fp, sizeof(char) * head->fileheader.dataoffset, SEEK_SET);
    unsigned char* data = calloc(head->bitmapsize, sizeof(unsigned char));
    fread(data, sizeof(char), head->bitmapsize, fp);

    // Check bitmap properties
    // FIXME
    //
    // if(!(head->bitsperpixel == 24 &&
    //    head->compression == 0 &&
    //    head->width == head->height )){ ... STUFF ... }
    /* printf("dim: %dx%d, bpp: %d compression: %d\n", head->width, head->height, head->bitsperpixel, head->compression); */
    /* printf("Offset: %d", head->fileheader.dataoffset); */
    /* printf("len: %d vs. %d\n", head->bitmapsize, (head->width * head->height * head->bitsperpixel) / sizeof(char)); */

    
    // Create some space...
    bitmap_t* new   = malloc(sizeof(bitmap_t));
    new->bits       = calloc(head->bitmapsize / 3, sizeof(bool));    // probably not an issue, but
                                                                        // on most systems this is wasteful of memory.

    // Load data by seeing what is over or under 128 in one channel.
    // This presumes there's 24BPP, and ignores many channels.
    bool* bit_ptr = new->bits;
    for(int i=0; i<head->bitmapsize; i+=3){
        (*bit_ptr) = ((data[i] > 128) ? true : false);
        bit_ptr++;
    }

    new->width = head->width;
    new->height = head->height;

    return new;
}

// Free the memory used by a bitmap
void free_bmp(bitmap_t* bmp){
    free(bmp->bits);
    free(bmp);
}

// Allocates a new bitmap
bitmap_t* new_bmp(int w, int h){
    bitmap_t* new = malloc(sizeof(bitmap_t));

    // Set things
    new->width = w;
    new->height = h;
    new->bits = calloc( w * h, sizeof(bool));

    return new;
}

bool bmp_get(bitmap_t* bmp, int x, int y){
    return bmp->bits[ bmp->width * y + x ];
}

void bmp_put(bitmap_t* bmp, int x, int y, bool val){
    bmp->bits[ bmp->width * y + x ] = val;
}

void debug_draw_bmp(bitmap_t* bmp){

    for(int i=0; i<bmp->width; i++){
        for(int j=0; j<bmp->height; j++){
            if(bmp->bits[i]) printf("#");
            else printf(" ");
        }
        printf("\n");
    }

}
