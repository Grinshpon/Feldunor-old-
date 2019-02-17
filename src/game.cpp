//getch emulator
#include<unistd.h>
#include<termios.h>
#include<iostream>
#include "draw.hpp"
#include "rlk.hpp"
using std::string;

enum commands {
    quitGame,
    invalid
};

int getch(void);
int getInput();
int Start();
int Update();

string inputYes = "Yy";
Map *gameMap = new Map();

int Start() {
    int startStatus = 0;
    startStatus = clearScreen();
    return startStatus;
}

int Update() {
    int updateStatus = 0;
    updateStatus = getInput();
    return updateStatus;
}

int Draw() {
    int drawStatus = 0;
    drawStatus = drawScreen();
    return drawStatus;
}

int Quit() {
    int quitStatus = 0;
    delete gameMap;
    return quitStatus;
}


// Input Handler (put in different file or keep it here?)

int getInput() {
    int input = getch(); 
    switch(input) {
	case 'Q'://just copy how it works in rudie
	    printf("Quit? y/N");
	    input = getch();
	    if (inputYes.find(input) != string::npos) {
		Quit();
		clearScreen();
		return 10;
	    }
	    break;
	default:
	    break;
    }
    return 0;
}

int getch(void) {
    int ch;
    struct termios oldt;
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
    newt = oldt; /* copy old settings to new settings */
    newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */
    ch = getchar(); /* standard getchar call */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
    return ch; /*return received char */
}
