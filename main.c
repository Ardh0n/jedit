#include "fileio.h"
#include "gapbuffer.h"
#include <stdio.h>
#include <ncurses.h>
#include <string.h>

enum mode {
    EDIT,
    COMMAND
};

gapbuffer gb;
enum mode mode;
char file[256];


void input(char *buf, char *text, size_t n){
    int ch;
    int i = 0;
    int y = stdscr->_maxy;
    move(y, 0);
    for(char *p = text; *p!=0;p++){
        addch(*p);
    }
    while(i < n){
        ch = getch();
        switch(ch){
            case 27:
                memset(buf, 0, n);
                return;
            case '\n':
                i = n;
                break;
            case KEY_BACKSPACE:
                if (i > 0) buf[i - 1] = 0;
                i--;
                addch(KEY_BACKSPACE);
                break;
            default:
                if (ch >= ' ' && ch <= '~'){
                    buf [i] = ch;
                    addch(buf[i]);
                    i++;
                }
        }
        refresh();
    }
}
void textCommand(){
    char c[128];
    input(c,":", 128);
    if (strcmp(c, "w") == 0){
        if(*file == 0) input(file, "filename:", 256);
        if(*file == 0) return;
        saveFile(&gb, file);
    } else if (strcmp(c, "q") == 0){
        clear();
        endwin();
        free(gb.buffer);
        exit(0);
    } else if (strcmp(c, "wq") == 0){
        if(*file == 0) input(file, "filename:", 256);
        if(*file == 0) return;
        saveFile(&gb, file);
        clear();
        endwin();
        free(gb.buffer);
        exit(0);
    }
}

void render(){
        int cursor_x, cursor_y;
        move(0, 0);
        int x = 0;
        int y = 0;
        for(char *p = gb.buffer; p != gb.gapstart; p++){
            addch(*p);
            if(*p == '\n'){
                y++;
                x = 0;
            } else {
                x++;
            }
        }
        cursor_x = x;
        cursor_y = y;
        for(char *p = gb.gapend; p != gb.bufferend; p++){
            addch(*p);
            if(*p == '\n'){
                y++;
                x = 0;
            } else {
                x++;
            }
        }
        addch('\n');
        while(y < stdscr->_maxy - 1){
            addch('~');
            addch('\n');
            y++;
        }
        switch(mode){
            case EDIT:
                addch('E');
                break;
            case COMMAND:
                addch('C');
                break;
        }
        addch('\n');
        move(cursor_y, cursor_x);
}

void command(int c){
    switch(c){
        case 'h':
            gb_cursor(&gb, gb_position(&gb) - 1);
            break;
        case 'j':
            gb_line_down(&gb);
            break;
        case 'k':
            gb_line_up(&gb);
            break;
        case 'l':
            gb_cursor(&gb, gb_position(&gb) + 1);
            break;
        case 'i':
            mode = EDIT;
            break;
        case 'a':
            mode = EDIT;
            gb_cursor(&gb,gb_position(&gb) + 1);
            break;
        case 'A':
            mode = EDIT;
            gb_cursor(&gb, gb_search_right(&gb, '\n'));
            break;
        case ':':
            textCommand();
            break;
    }
}


void edit(int c){
    switch (c){
        case '\n':
            gb_insert(&gb, '\n');
            break;
        case 27:
            mode = COMMAND;
            return;
        case KEY_BACKSPACE:
            gb_delete_left(&gb, 1);
            break;
        case KEY_RIGHT:
            gb_cursor(&gb, gb_position(&gb) + 1);
            break;
        case KEY_LEFT:
            gb_cursor(&gb, gb_position(&gb) - 1);
            break;
        case KEY_UP:
            gb_line_up(&gb);
            break;
        case KEY_DOWN:
            gb_line_down(&gb);
            break;
    }
    if (c >= ' ' && '~' >= c){
        gb_insert(&gb, c);
    }
}

int main(int argc, char **argv){
    mode = COMMAND;
    if (argc < 2){
        gb_init_buffer(&gb, GB_DEFAULT_SIZE);
    } else {
        int e = loadFile(&gb, argv[1]);
        strcpy(file, argv[1]);
        if (e != 0){
            printf("Error loading file\n");
            exit(1);
        }
    }

    initscr();
    cbreak();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);

    int main_loop = 1;

    while(main_loop){
        render();
        refresh();
        int c = getch();
        switch(mode){
            case EDIT:
                edit(c);
                break;
            case COMMAND:
                command(c);
                break;
        }
    }
    endwin();
    free(gb.buffer);
}

