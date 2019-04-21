//getch emulator
#include<unistd.h>
#include<termios.h>
#include<iostream>
#include<vector>
#include "rlk.hpp"
using std::vector;
using std::string;

extern int clearScreen();
extern int initScreen();
extern int drawScreen(string statBar, char* world, int w, int h, string actions);
extern int exitCurse();

enum commands {
    quitGame,
    invalid
};

int getch(void);
int getInput();
int Start();
int Update();
int Draw();
int Quit();

int input;

string inputYes("Yy");
World *gameWorld = new World();
string topBar("");
string bottomBar("");


int Start() {
  int startStatus = 0;
  startStatus = initScreen();
  startStatus = clearScreen();
  gameWorld->generateMap();
  gameWorld->flattenMap();
  return startStatus;
}

int Update() {
  bottomBar = "";
  int updateStatus = 0;
  updateStatus = getInput();
  bottomBar += input;
  return updateStatus;
}

int Draw() {
  int drawStatus = 0;
  drawStatus = drawScreen(topBar,gameWorld->flatMap,WIDTH,HEIGHT,bottomBar);
  return drawStatus;
}

int Quit() {
  int quitStatus = 0;
  quitStatus = exitCurse();
  delete gameWorld;
  return quitStatus;
}


// Input Handler (put in different file or keep it here?)

int getInput() {
  input = getch();
  switch(input) {
    case 'Q'://just copy how it works in rudie
      printf(" Quit? y/N");
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
