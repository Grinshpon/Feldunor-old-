#ifndef MAPGEN_HPP
#define MAPGEN_HPP

#include<vector>
using std::vector;
//Might be necessary, might put mapGen code into Map object (?)

enum mapGenSymbols {
    empty, // nothing
    vWall, // vertical wall |
    hWall, // horizontal wall -
    room, // room space .
    hall, // hallway #
    door, // door +
    mapGenMax
};

int *GenerateMap(int width, int height) {
    int *level = new int[width*height];
    for(int i = 0; i < width*height; i++)
        level[i] = 0;
    srand (time(NULL));

    int wsize = rand()%7+5, hsize = rand()%6+5;
    int x = rand()%(width - wsize), y = rand()%(height - hsize);
    for(int j = 0; j < hsize; j++) {
        for(int i = 0; i < wsize; i++) {
            if(j == 0 || j == (hsize-1))
                level[(y+j)*width+(x+i)] = hWall;
            else if(i == 0 || i == (wsize-1))
                level[(y+j)*width+(x+i)] = vWall;
            else
                level[(y+j)*width+(x+i)] = room;
        }
    }
    return level;
}
//decide whether to use vector or int*, and also work out map generation algorithm
/*vector<int> GenerateMap_vec(int width, int height) {
    vector<int> level(width*height,0);
    srand(time(NULL));
    
    int wsize = rand()&7+5, hsize = rand()%6+5;
    int x = rand()%(width - wsize), y = rand()%(height - hsize);

    return level;
}*/

#endif
