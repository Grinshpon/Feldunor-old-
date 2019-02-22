//all ncurses code to draw the game goes here
#include<ncurses.h>
#include<iostream>
#include<vector>
using std::string;
using std::vector;

int clearScreen() {
    //printf("\033[H\033[J");
    return 0;
}

int initScreen() {
    initscr();
    cbreak();
    noecho();
    clear();
    return 0;
}
//drawScreen(stats bar, level plus entities on it, width of worldmap, height of world map, bottom actions bar)
int drawScreen(string statBar, char* world, int w, int h, string actions) { //will take char* 's and print them in the terminal
    int status = clearScreen();
    printf("%s\n",statBar.c_str());
    for(int i = 0, count=0; i < w*h; i++,count++) {
	if (count == w) {
	    printf("\n");
	    count = 0;
	}
	printf("%c",world[i]);
    }
    printf("\n%s",actions.c_str());
    printf("testing");
    refresh();
    return status;
}

int exitCurse() {
    endwin();
}
