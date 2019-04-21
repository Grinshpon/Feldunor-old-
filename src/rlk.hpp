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

#define WIDTH 80
#define HEIGHT 20

std::string walkable = ".#+";

struct Point {
  int x;
  int y;
  bool operator == (const Point &other) const {
    return (x == other.x && y == other.y);
  }
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

int distance(Point &s, Point &f) { //Chebyshev Distance
  int dx = abs(s.x-f.x);
  int dy = abs(s.y-f.y);
  return (dx > dy ? dx : dy);
}

struct Node {
  Point origin;
  int G;
  int H;
  int F;
  bool operator == (const Node &other) const {
    return (origin == other.origin);
  }
};
Node node(Point &o, Point s, Point &f) {
  int g = distance(o,s);
  int h = distance(s,f);
  return {s,g,h,g+h};
}

class Path {
  private:
  Point origin;
  Path *parent = NULL;
  Path *child = NULL;
  bool first,last;
  Path(int x, int y, Path *p) {
    origin = {x,y};
    parent = p;
    last = true;
    first = p == NULL;
  }

  public:
  Path(int x, int y) : Path(x,y,NULL){};
  Path *next() {
    return child;
  }
  Path *prev() { //previous
    return parent;
  }
  void add(int x, int y) {
    if (last) {
      child = new Path(x,y,this);
      last = false;
    }
    else {
      child->add(x,y);
    }
  }
  Point point() {
    return origin;
  }
  Path *reverse() { //reverses path and returns new first node
    Path *temp_res = NULL;
    if(!last) {
      temp_res = child->reverse();
    }
    Path *temp = parent;
    parent = child;
    child = temp;
    if(first) {
      first = false;
      last = true;
    }
    else if(last) {
      last = false;
      first = true;
      return this;
    }
    return temp_res; //placeholder
  }
  ~Path() {
    delete child;
  }
};

class PathTree {
  private:
	int x, y;
  int G,H,F;
	vector<PathTree*> branches;
	bool start, end, fin;
	PathTree *parent = NULL;//cannot be deleted
	PathTree(int ax, int ay, Point &f, PathTree* p) {
    x = ax;
    y = ay;
    parent = p;
    start = p == NULL;
    end = true;
    fin = false;
    G = (p == NULL ? 0 : p->getG()+1);
    Point temp = {x,y};
    H = distance(temp,f);
    F = G+H;
	}

  public:
	PathTree(int x, int y,Point &f)
    : PathTree(x,y,f,NULL){}
	PathTree(Point f)
    : PathTree(0,0,f,NULL){}
  PathTree(Point s,Point &f)
    : PathTree(s.x,s.y,f,NULL){}
	Point point(){
	    return Point{x,y};
	}
  int getX() {
    return x;
  }
  int getY() {
    return y;
  }
  int getG() {
    return G;
  }
  int getH() {
    return H;
  }
  int getF() {
    return F;
  }
	void add(int ax, int ay, Point f) {
    branches.push_back(new PathTree(ax, ay, f, this));
    end = false;
	}
  void add(Point a, Point f) {
    branches.push_back(new PathTree(a.x, a.y, f, this));
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
  bool begin() {
    return start;
  }
	bool finish() {
    return fin;
	}
	void setFinish() {
    fin = !fin;
	}
  PathTree *prev() {
    return parent;
  }
	PathTree *branch(int i) {
    return branches.at(i);
	}
	void for_each(void(*f)(PathTree*)) {//for the current point and each point below
    f(this);
    if(!end) {
      for(auto i : branches) {
        i->for_each(f);
      }
    }
	}
	void for_each_end(void(*f)(PathTree*)) {//only for endpoints
    if(end)
      f(this);
    else {
      for(auto i : branches) {
        i->for_each_end(f);
      }
    }
	}
  void end_tiles(vector<Node> &p) {//WIP
    if(end){}
      //TODO
    else {
      for(auto i : branches) {
        i->end_tiles(p);
      }
    }
	}
  PathTree *getFinish() {
    PathTree *res = this;
    if(fin){}
    else {
      for(auto i : branches) {
        res = i->getFinish();
      }
    }
    return res;
  }
  Path* for_finish(Path*(*f)(PathTree*)) {
    return f(this->getFinish());
	}
  Path *genPath() {
    Path *res = NULL;
    res = this->for_finish([](PathTree *n) mutable -> Path* {
      Path *path = new Path(n->getX(),n->getY());
      PathTree *iter = n->prev();
      int timeout = 0;
      while(timeout != -1) {
        ++timeout;
        if(iter != NULL) {
          path->add(iter->getX(),iter->getY());
          iter = iter->prev();
        }
        else {
          timeout = -1;
        }
        if (timeout > 2000){
          break;
        }
      }
      return path;
    });
    if (res == NULL) std::cout << "oopsy\n";
    return res->reverse();
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
	//static const int WIDTH = 80;
	//static const int HEIGHT = 20;
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
  //this entire pathfinding implementation is pretty bad, but I'm tired and figure I'll redo it later. The importing thing is for it to return a Path*
	void AStar(Point &start, Point &finish) {//for generating hallways and entity pathfinding (entities will reside in world class) (remember to remove comments for myself) (chicken butt)
    //TODO: if distance > 1 then proceed, otherwise just need path of one (x2-x1,y2-y1)
    Path *path = NULL;
    bool finished = false;
    vector<Node> open;//replace with (unordered_)map for better performance, requires making hash and crap for Point though and I don't feel like it now
    //open.push_back(node(origin,origin,target));
    int min = WIDTH*HEIGHT+1;
    PathTree *closed = new PathTree(start,finish);
    for(int i = -1; i <= 1; i++) {
      for(int j = -1; j <= 1; j++) {
        if (i != 0 || j != 0) {
          if ( (start.x != 0 || i != -1) && (start.y != 0 || j != -1) ) {
            open.push_back(node(start,{i,j},finish));
          }
        }
      }
    }
    for(auto tile : open) {
      if (tile.F < min) min = tile.F;
    }
    /*for(auto tile : open) {
      if (tile.F == min) {
        closed->add(tile.origin,target);

      }
    }*/
    for(auto tile = open.begin(); tile != open.end();){
      if((*tile).F == min) {
        closed->add((*tile).origin,finish);
        tile = open.erase(tile);
      }
      else
        ++tile;
    }
    while(!finished) {
      //TODO
      finished = true;
    }
    delete closed;
    delete path; //eventually will return final path
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
  			    if (iterAttempt) break;
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
    //will have separate entity data then draw items/nps's/player onto flatmap
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
