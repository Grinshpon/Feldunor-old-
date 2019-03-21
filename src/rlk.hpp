#ifndef RLK_HPP
#define RLK_HPP

#include <math.h>
#include <vector>
#include <array>
using std::vector;
using std::array;

enum mapGenSymbols {
    empty, // nothing
    vWall, // vertical wall |
    hWall, // horizontal wall -
    mapRoom, // room space .
    hall, // hallway #
    door, // door +
    mapGenMax
};

std::string walkable = ".#+";

struct Point {
    int x;
    int y;
};
struct Room {
    Point origin;
    int height;
    int width;
};
struct Door {
    Point origin;
    int id;
};

class PathTree {
    private:
	int x, y;
	vector<PathTree*> branches;
	bool start, end, fin;
	PathTree *parent = NULL;//cannot be deleted
	PathTree(int ax, int ay, PathTree* p) {
	    x = ax;
	    y = ay;
	    parent = p;
	    start = p == NULL;
	    end = true;
	    fin = false;
	}
    public:   
	PathTree(int x, int y)
	    : PathTree(x,y,NULL){}
	PathTree()
	    : PathTree(0,0,NULL){}
	Point point(){
	    return Point{x,y};
	}
	void add(int ax, int ay) {
	    branches.push_back(new PathTree(ax, ay, this));
	    end = false;
	}
	void remove(int i) {
	    delete branches.at(i);
	    branches.erase(branches.begin()+i);
	    branches.shrink_to_fit();
	    if (branches.empty()) end = true;
	}
	int children() {
	    return branches.size();
	}
	bool finish() {
	    return fin;
	}
	void setFinish() {
	    fin = !fin;
	}
	PathTree *branch(int i) {
	    return branches.at(i);
	}
	void for_each(void(*f)(PathTree*)) {//for the current point and each point below
	    f(this);//testing placeholder
	}
	void for_each_end(void(*f)(PathTree*)) {//only for endpoints
	    f(this);//testing placeholder
	}
	~PathTree() {
	    for(PathTree* x : branches) {
		delete x;
	    }
	    branches.clear();
	    branches.shrink_to_fit();
	}

};

// World Class will also contain vector or list of entities on map, as well as pathfinding algo (A*)
class World {//replace map class (will also handle putting entities on map)
    public:
	static const int WIDTH = 80;
	static const int HEIGHT = 20;
	int depth;//will be used later to determine difficulty
	char *flatMap = NULL;
	World() {
	    depth = 0;
	    flatMap = new char[WIDTH*HEIGHT];
	}
    private:
	array<int,(WIDTH*HEIGHT)> mapData{};
	vector<Room> rooms;
	vector<Door> doors;

	void AStar(Point origin, Point target) {//for generating hallways and entity pathfinding (entities will reside in world class) (remember to remove comments for myself) (chicken butt)
	    //not sure yet, maybe return linked list / path?
	}
	void generateHalls() {
	    //generate halls for each door leading to another door of different room(id) using A*
	    int *length = new int (doors.size());
	    float *halfLF = new float();
	    *halfLF = (float)(*length)/2;
	    int *uDL = new int();
	    *uDL = ceil(*halfLF/4);
	    char *usedDoors = new char[*uDL]();
	    int *halfL = new int();
	    *halfL = (int)(ceil(*halfLF));

	    for(int i = 0,j=0; i < *halfL; i++,j++) {
		while(usedDoors[(int)j/8] & (j < 8 ? (1 << j) : (1 << j-8))) {
		    j++;
		}
		usedDoors[(int)j/8] |= (j < 8 ? (1 << j) : (1 << j-8));
		int pair = j+rand()%(*length-j-2)+1;
		while( (doors.at(j).id == doors.at(pair).id) || ( (usedDoors[(int)j/8] & (pair < 8 ? (1 << pair) : (1 << pair-8))) && (i != *halfL-1) ) ) {
		    pair++;
		    if (pair >= *length) pair = 0;
		}
		AStar(doors.at(j).origin,doors.at(pair).origin);
	    }
	    printf(" %d",*uDL);//DEBUG
	    delete halfLF;
	    delete halfL;
	    delete uDL;
	    delete length;
	    delete[] usedDoors;
	}
	void generateDoors(int id) {
	    //printf(" %d",id);
	    int numofDoors = rand()%3+1;
	    for(int i = 0; i < numofDoors; i++) {
		if(rand()%2 == 0)
		    doors.push_back({{rooms.at(id).origin.x+(rand()%2)*(rooms.at(id).width-1),rooms.at(id).origin.y+1+(rand()%(rooms.at(id).height-2))},id});
		else {
		    doors.push_back({{rooms.at(id).origin.x+1+(rand()%(rooms.at(id).width-2)),rooms.at(id).origin.y+(rand()%2)*(rooms.at(id).height-1)},id});
		}
		mapData.at(WIDTH*doors.back().origin.y+doors.back().origin.x) = door;
		//printf( "(%d:%d:{%d,%d}), ",id,i,doors.at(i).origin.x,doors.at(i).origin.y);
	    }
	}
	bool checkCollision(int x1, int y1, int h1, int w1, int i) {
	    int x0 = rooms.at(i).origin.x, y0 = rooms.at(i).origin.y, h0 = rooms.at(i).height, w0 = rooms.at(i).width;
	    return !(y1 > y0 + h0+1 || y1 < y0 - (h1+1) || x1 > x0 + w0+1 || x1 < x0 - (w1+1));//true if collision, false if no collision
	}
	void generateRooms() {
	    //generate rooms
	    //printf("planning rooms"); //debug
	    int numofRooms = rand()%4+4;
	    for(int i = 0; i <= numofRooms; i++) {
		int attempt = 0;
		bool iterAttempt = false;
		int height = rand()%7+5, width = rand()%8+5;
		int x = 1+rand()%(WIDTH-(width+1)), y = 1+rand()%(HEIGHT-(height+1));
		if(i > 0) {
		    while(attempt != -1 && attempt < 10) {// -1 means successfull formation
			for(int rs = 0; rs < rooms.size(); rs++) {
			    iterAttempt = checkCollision(x,y,height,width,rs);
			    if (iterAttempt)
				break;
			}
			if (iterAttempt) {
			    ++attempt;
			    height = rand()%7+5;
			    width = rand()%8+5;
			    x = 1+rand()%(WIDTH-(width+1));
			    y = 1+rand()%(HEIGHT-(height+1));
			}
			else attempt = -1;
		    }
		}
		if(attempt < 10)
		    rooms.push_back({{x,y},height,width});
	    }
	}
    public:
	void generateMap() {
	    //generate map layout
	    //using generateRooms and generateDoors
	    generateRooms();
	    //printf("digging dungeon...");
	    //printf("%d",rooms.size());
	    for(int i = 0; i < rooms.size(); i++) {
		for(int j = 0; j < rooms.at(i).height; j++) {
		    for(int k = 0; k < rooms.at(i).width; k++) {
			if(j == 0 || j == (rooms.at(i).height-1))
			    mapData.at((rooms.at(i).origin.y+j)*WIDTH+(rooms.at(i).origin.x+k))=hWall;
			else if (k == 0 || k == (rooms.at(i).width-1))
			    mapData.at((rooms.at(i).origin.y+j)*WIDTH+(rooms.at(i).origin.x+k))=vWall;
			else
			    mapData.at((rooms.at(i).origin.y+j)*WIDTH+(rooms.at(i).origin.x+k))=mapRoom;
		    }
		}
		generateDoors(i);
	    }
	    generateHalls();
	}
	void flattenMap() {
	    //turn mapData into flatMap
	    for(int i = 0; i < HEIGHT; i++) {
                for(int j = 0; j < WIDTH; j++) {
                    switch (mapData.at(WIDTH*i+j)) {
                        case empty:
                            flatMap[WIDTH*i+j] = ' ';
                            break;
                        case vWall:
                            flatMap[WIDTH*i+j] = '|';
                            break;
                        case hWall:
                            flatMap[WIDTH*i+j] = '-';
                            break;
                        case mapRoom:
                            flatMap[WIDTH*i+j] = '.';
                            break;
                        case hall:
                            flatMap[WIDTH*i+j] = '#';
                            break;
                        case door:
                            flatMap[WIDTH*i+j] = '+';
                            break;
                        default:
                            break;
                    }
                }
            }
	}

	~World() {
	    delete[] flatMap;
	}

};

#endif
