#include "fileio.h"
#include "gapbuffer.h"
#include <ncurses.h>
#include <stdio.h>

int saveFile(gapbuffer *gb, char *path){
    FILE* f = fopen(path, "w");
    if(f == NULL){
        return 1;
    }
    fwrite(gb->buffer, 1, gb->gapstart - gb->buffer, f);
    fwrite(gb->gapend, 1, gb->bufferend - gb->gapstart - gb_gapsize(gb), f);
    return 0;
}

int loadFile(gapbuffer *gb, char *path){
    FILE* f = fopen(path, "r");
    if(f == NULL) return 1;
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);
    char c;

    gb_init_buffer(gb, 2 * fsize + EXPAND_BYTES);
    for(int i=0; i < fsize;i++){
        fread(&c, 1, 1, f);
        gb_insert(gb, c);
    }
    //gb_delete_left(gb);
    gb_cursor(gb, 0);
    return 0;
}
