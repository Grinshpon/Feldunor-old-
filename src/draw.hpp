#ifndef DRAW_HPP

int clearScreen() {
    printf("\033[H\033[J");
    return 0;
}

int drawScreen() { //will take char* 's and print them in the terminal
    int status = clearScreen();
    return status;
}

#endif
