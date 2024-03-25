#include "gapbuffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void gb_print(gapbuffer *gb){
    fwrite(gb->buffer, 1, gb->gapstart - gb->buffer, stdout);
    printf("[%lu Gap]", gb_gapsize(gb));
    fwrite(gb->gapend, 1, gb->bufferend - gb->gapend, stdout);
}

void gb_line_down(gapbuffer *gb){
    int pos = gb_search_right(gb, '\n') + 1;
    gb_cursor(gb, pos);
}
void gb_line_up(gapbuffer *gb){
    int pos = gb_search_left(gb, '\n');
    gb_cursor(gb, pos);
}

int gb_position(gapbuffer *gb){
    return gb->gapstart - gb->buffer;
}

int gb_init_buffer(gapbuffer *gb, size_t size){
    gb->buffer = malloc(size);
    if (gb->buffer == NULL) return -1;

    gb->gapend = gb->buffer + size;
    gb->gapstart = gb->buffer;
    gb->point = gb->buffer;
    gb->bufferend = gb->gapend;

    return 0;
}

int gb_expand(gapbuffer *gb, size_t size){
    int gapend_offset = gb->gapend - gb->buffer;
    int gapstart_offset = gb->gapstart - gb->buffer;
    int point_offset = gb->point - gb->buffer;
    int bufferend_offset = gb->bufferend - gb->buffer;

    gb->buffer = realloc(gb->buffer, size + gb->bufferend - gb->buffer);
    if (gb->buffer == NULL) return -1;

    gb->gapstart = gapstart_offset + gb->buffer;

    memcpy(gb->buffer + gapend_offset, gb->buffer + gapend_offset + size, gb->bufferend - gb->gapend);

    if(point_offset < gapend_offset) gb->point = gb->buffer + point_offset;
    else gb->point = gb->buffer + point_offset + size;

    gb->gapend = gb->buffer + gapend_offset + size;
    gb->bufferend = gb->buffer + bufferend_offset + size;

    return 0;
}

size_t gb_gapsize(gapbuffer *gb){
    return gb->gapend - gb->gapstart;
}

int gb_insert(gapbuffer *gb, char c){
    if(gb_gapsize(gb) == 0)
        if(gb_expand(gb, gb->bufferend - gb->buffer + EXPAND_BYTES) == -1) return -1;

    *gb->gapstart = c;
    gb->gapstart++;

    return 0;
}

void gb_left(gapbuffer *gb, int pos){
    if(pos < 0) pos = 0;
    size_t n = gb->gapstart - gb->buffer - pos;

    memcpy(gb->gapend - n, gb->gapstart - n, n);

    gb->gapstart -= n;
    gb->gapend -= n;
}

void gb_right(gapbuffer *gb, int pos){
    if (pos >= gb->bufferend - gb->buffer - gb_gapsize(gb))
        pos = gb->bufferend - gb->buffer - gb_gapsize(gb);
    int n = pos + gb_gapsize(gb) - (gb->gapend - gb->buffer);

    memcpy(gb->gapstart, gb->gapend, n); 

    gb->gapstart += n;
    gb->gapend += n;
}

void gb_cursor(gapbuffer *gb, int pos){
    if (pos < gb->gapstart - gb->buffer){
        gb_left(gb, pos); 
    } else {
        gb_right(gb, pos);
    }
}

void gb_delete_left(gapbuffer *gb, size_t n){
    gb->gapstart -= n;
    if (gb->gapstart < gb->buffer) gb->gapstart = gb->buffer;
}
void gb_delete_right(gapbuffer *gb, size_t n){
    gb->gapend += n;
    if (gb->gapend > gb->bufferend) gb->gapend = gb->bufferend;
}

int gb_search_left(gapbuffer *gb, char c){
    for(char *p = gb->gapstart - 1; p >= gb->buffer; p--){
        if(*p == c) return p - gb->buffer;
    }
    return 0;
}

int gb_findfirst(gapbuffer *gb, char * chars){
    for(char *p = gb->gapend; p< gb->bufferend; p++){
        for(char *c = chars; *c != 0; c++){
            if(*c == *p) return gb->gapstart - gb->buffer + p - gb->gapend;
        }
    }
    return gb->gapend - gb->buffer;
}
int gb_search_right(gapbuffer *gb, char c){
    for(char *p = gb->gapend; p < gb->bufferend; p++){
        if(*p == c) return p - gb->gapend + gb->gapstart - gb->buffer;
    }
    return gb->bufferend - gb->buffer - gb_gapsize(gb);
}

