#pragma once
#include <stdlib.h>
#define EXPAND_BYTES 256
#define GB_DEFAULT_SIZE 256


/* Gapbuffer implementation to represent text
 https://en.wikipedia.org/wiki/Gap_buffer
       schema:
       gapbuffer is an array of chars

       Text before cursor [       gap         ] Text after cursor

       Text is inserted at the start of the gap
       */



typedef struct gapbuffer {
    char *buffer;
    char *bufferend;
    char *point;
    char *gapstart;
    char *gapend;
} gapbuffer;

void gb_print(gapbuffer *gb);
int gb_position(gapbuffer *gb);
int gb_findfirst(gapbuffer *gb, char * chars);

void gb_line_down(gapbuffer *gb);
void gb_line_up(gapbuffer *gb);

int gb_init_buffer(gapbuffer *gb, size_t size); 
int gb_expand(gapbuffer *gb, size_t size);
int gb_insert(gapbuffer *gb, char c);
size_t gb_gapsize(gapbuffer *gb);
void gb_left(gapbuffer *gb, int pos);
void gb_right(gapbuffer *gb, int pos);
void gb_cursor(gapbuffer *gb, int pos);
void gb_delete_left(gapbuffer *gb, size_t n);
void gb_delete_right(gapbuffer *gb, size_t n);

/* search return position in text to current cursor*/

int gb_search_left(gapbuffer *gb, char c);
int gb_search_right(gapbuffer *gb, char c);
