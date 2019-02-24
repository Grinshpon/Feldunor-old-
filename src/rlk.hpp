#ifndef RLK_HPP
#define RLK_HPP

#include <math.h>
#include <vector>
#include <array>
#include "mapgen.hpp"
using std::vector;
using std::array;

std::string walkable = ".#+";
int consoleQueue[10];
int queuePlace = 0;
int queueCounter = -1;

enum consoleMessage {
    init,
    enterCommand,
    quitCheck,
    moveUp,
    moveDown,
    moveLeft,
    moveRight,
    moveUpLeft,
    moveUpRight,
    moveDownLeft,
    moveDownRight,
    cantMove,
    helpScreen,
    gameOver,
    slightHungry,
    reallyHungry,
    deathCause,
    levelUp,
    peonAttack,
    consoleMessageMax
};

std::string CONSOLE[consoleMessageMax] = {
    "Feldunor v 0.0.2-1 (Press '?' for help)",
    "Enter Command: ",
    "Are you sure you want to quit? y/N\n",
    "Move North",
    "Move South",
    "Move West",
    "Move East",
    "Move North-West",
    "Move North-East",
    "Move South-West",
    "Move South-East",
    "There's a wall in the way",
    "-----------Help-----------\n"
    "Move North\t:\tk\n"
    "Move South\t:\tj\n"
    "Move West\t:\th\n"
    "Move East\t:\tl\n"
    "Move North-West\t:\ty\n"
    "Move North-East\t:\tu\n"
    "Move South-West\t:\tb\n"
    "Move South-East\t:\tn\n"
    "Character Sheet\t:\tC\n"
    "Quit\t\t:\tQ\n"
    "Help\t\t:\t?\n"
    "--------Press Space-------",
    "\n\n\n\n\t\t\t\t-------------"
    "\n\t\t\t\tYou Have Died"
    "\n\t\t\t\t-Press Space-"
    "\n\t\t\t\t-------------",
    "Your stomach rumbles",
    "Your stomach is in great pain",
    "\n\t\t\t\tCause of death:\n\n\t\t\t\t   ",
    "You feel a great rush of energy! (Level Up)",
    "The peon attacks!",
};

enum CauseOfDeath {
    dHunger,
    dDefault,
    CauseOfDeathMax
};

std::string deathCauseText[CauseOfDeathMax] = {
    "Hunger",
    "dDefault"
};

enum livingEntityStats {
    vit, //max health
    str, //damage(scale)/armor
    dex, //damage(scale)/evasion
    xp,
    lvl,
    MAX
};

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

class Map {//to deprecate
    public:
        static const int WIDTH = 80; //80: default terminal screen width
        static const int HEIGHT = 20; //20: 24 minus four lines of text for console (so that default terminal screen compatible)
        int depth;
        char *mapData = new char[HEIGHT*WIDTH];
        int *seed = NULL;
	bool nullseed = true;
        void genSeed(int depth) {
	    if(!nullseed)
		delete[] seed;
	    else
		nullseed = false;
            seed = GenerateMap(WIDTH,HEIGHT);
        }
        void buildLevel() {
            for(int i = 0; i < HEIGHT; i++) {
                for(int j = 0; j < WIDTH; j++) {
                    switch (seed[WIDTH*i+j]) {
                        case empty:
                            mapData[WIDTH*i+j] = ' ';
                            break;
                        case vWall:
                            mapData[WIDTH*i+j] = '|';
                            break;
                        case hWall:
                            mapData[WIDTH*i+j] = '-';
                            break;
                        case mapRoom:
                            mapData[WIDTH*i+j] = '.';
                            break;
                        case hall:
                            mapData[WIDTH*i+j] = '#';
                            break;
                        case door:
                            mapData[WIDTH*i+j] = '+';
                            break;
                        default:
                            break;
                    }
                }
            }
        }
	~Map() {
	    delete[] seed;
	    delete[] mapData;
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

class Entity {
    public:
        int x, y;
        char id;
};

class Living: public Entity {
    public:
        int health,bdmg = 0,tdmg,reqXP = 0;
        double hunger = 10.5;
        int stats[MAX] = {10,1,1,0,1};//default
        void setStats() {
            health = stats[vit];
            if(reqXP == 0)
                reqXP = 100;
            else
                reqXP = reqXP*1.25;
        }
        void updateDMG() {
            tdmg = bdmg+stats[str]*0.5+stats[dex]*0.5;
        }
        void randStats() {
            srand (time(NULL));
            stats[vit] = rand()%5+10;
            stats[str] = rand()%4+1;
            stats[dex] = rand()%4+1;
        }
        void logicUpdate() {
            if(hunger > 0)
                hunger -= 0.025;
            if(stats[xp] >= reqXP && stats[lvl] != 48) {//48 Max Level
                ++stats[lvl];
                stats[vit] += rand()%2+1; //choose stat to specialize points in (receives extra 3 points)
                stats[str] += rand()%2+1;
                stats[dex] += rand()%2+1;
                hunger = 10;
                stats[xp] -= reqXP;
                setStats();
            }
        }
        std::string printStats() {
            //"hp: " + health + " (" + stats[vit] + ")\tdmg: " + (dmg+str*0.5+dex*0.5);
            updateDMG();
            std::string s = "Hp: ";
            s += std::to_string(health);
            s += "(";
            s += std::to_string(stats[vit]);
            s += ")\tDmg: ";
            s += std::to_string(tdmg);
            s += "\tHunger: ";
            s += std::to_string((int)hunger);
            s += "\tXp: ";
            s += std::to_string(stats[xp]);
            s += "(";
            s += std::to_string(reqXP);
            s += ")";
            s += "\t(";
            s += std::to_string(x);
            s += ",";
            s += std::to_string(y);
            s += ")\n";
            return s;
        }
        std::string printCharSheet() {
            std::string s = "Character Sheet\n\n";
            s += "Level:\t\t";
            s += std::to_string(stats[lvl]);
            s += "\nExperience:\t";
            s += std::to_string(stats[xp]);
            s += "/";
            s += std::to_string(reqXP);
            s += "\n\nVitality:\t";
            s += std::to_string(stats[vit]);
            s += "\nStrength:\t";
            s += std::to_string(stats[str]);
            s += "\nDexterity:\t";
            s += std::to_string(stats[dex]);
            s += "\n\nHealth:\t\t";
            s += std::to_string(health);
            s += "/";
            s += std::to_string(stats[vit]);
            s += "\nDamage:\t\t";
            s += std::to_string(tdmg);
            s += "\nHunger:\t\t";
            s += std::to_string((int)hunger);
            s += "\n\n-Press Space-";
            return s;
        }
        int distance(Entity &target) {
            return sqrt((x-target.x)*(x-target.x)+(y-target.y)*(y-target.y));
        }
        void attack(Living& target) {
            queueCounter += 1;
            consoleQueue[queueCounter] = peonAttack;
        }
};

class AIHandler {
    public:
        void moveLiving(Map& map, Living& living, int modY, int modX) {//check later
                if(walkable.find(map.mapData[map.WIDTH*living.y-modY+living.x-modX]) != std::string::npos) {
                    living.y -= modY;
                    living.x -= modX;
                }
        }
        void updatePeon(Living& peon, Living& player, Map& Gmap) {
            if(peon.distance(player) < 10) {
                if(peon.distance(player) == 1) {
                    peon.attack(player);
                }
                else {
                    srand (time(NULL));
                    if(abs(peon.x-player.x) > abs(peon.y-player.y)) {
                        if((peon.x-player.x) < 0)
                            moveLiving(Gmap, peon, 0, -1);
                        else
                            moveLiving(Gmap, peon,0,1);
                    }
                    else {
                        if((peon.y-player.y) < 0)
                            moveLiving(Gmap, peon, -1, 0);
                        else
                            moveLiving(Gmap, peon,1,0);
                    }
                }
            }
        }
};

#endif
