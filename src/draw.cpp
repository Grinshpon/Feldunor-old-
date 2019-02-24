//all ncurses code to draw the game goes here
#include<ncurses.h>
#include<iostream>
#include<vector>
using std::string;
using std::vector;

int MAXROWS;
int MAXCOLS;

int clearScreen() {
    //printf("\033[H\033[J");
    clear();
    return 0;
}

int initScreen() {
    initscr();
    cbreak();
    noecho();
    clear();
    MAXROWS = LINES - 1;
    MAXCOLS = COLS - 1;
    return 0;
}
//drawScreen(stats bar, level plus entities on it, width of worldmap, height of world map, bottom actions bar)
int drawScreen(string statBar, char* world, int w, int h, string actions) { //will take char* 's and print them in the terminal
    int status = clearScreen();
    addstr(statBar.c_str());
    addch(ACS_ULCORNER);//REMOVE
    addch('\n');
    /*for(int i = 0, count=0; i < w*h; i++,count++) {
	if (count == w) {
	    printf("\n");
	    count = 0;
	}
	printf("%c",world[i]);
    }*/
    for(int row = 0; row < h; row++) {
	for(int col = 0; col < w; col++) {
	    addch(world[row*w+col]);
	}
	addch('\n');
    }
    addstr(actions.c_str());
    refresh();
    return status;
}

int exitCurse() {
    endwin();
}
