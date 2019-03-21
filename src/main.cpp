//Feldunor v0.0.2-2
//By: Daniel Grinshpon
#include<iostream>
#include<stdio.h>
extern int Start();
extern int Update();
extern int Draw();

int checkStatus(int es) {
    if (es == 10)
	exit(0);
    else if(es != 0) {
	printf("Error: Exited Program\n");
	exit(es);
    }
}

int main() {
    int exit_status = 0;
    //exit_status code: 10 - quit game 1 - error 0 - normal
    
    //printf("  \\  /  |    |\\        | / ");
    printf("testing");
    srand(time(NULL));
    exit_status = Start();
    checkStatus(exit_status);
    exit_status = Draw();
    checkStatus(exit_status);
    for(;;) {
	exit_status = Update();
	checkStatus(exit_status);
	exit_status = Draw();
	checkStatus(exit_status);
    }
    return exit_status;
}

