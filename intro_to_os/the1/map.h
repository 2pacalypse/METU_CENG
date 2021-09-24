#include <unistd.h>

typedef enum Type {EMPTY, OBSTACLE, HUNTER, PREY} Type;

typedef struct Hunter{
    int row;
    int col;
    int energy;
    pid_t pid;
    int fd;
}Hunter;


typedef struct Prey{
    int row;
    int col;
    int energy;
    pid_t pid;
    int fd;
} Prey;


typedef struct Obstacle{
    int row;
    int col;
} Obstacle;


Hunter createHunter(int row, int col, int energy);
Prey createPrey(int row, int col, int energy);
Obstacle createObstacle(int row, int col);




typedef struct Map{
    int width;
    int height;
    int nObstacles;
    int nHunters;
    int nPreys;
    Hunter* hunters;
    Prey* preys;
    Obstacle* obstacles;
} Map;



Map* createMap();
Map* readFromInput();

Type coordType(Map* map, int row, int col);

void destroyMap(Map* map);
void printMap(Map* map);

