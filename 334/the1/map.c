#include "map.h"
#include <stdlib.h>
#include <stdio.h>



Hunter createHunter(int r, int c, int e){
    Hunter hunter;
    hunter.row = r;
    hunter.col = c;
    hunter.energy = e;
    hunter.pid = -1;
    hunter.fd = -1;
    return hunter;
}


Prey createPrey(int r, int c, int e){
    Prey prey;
    prey.row = r;
    prey.col = c;
    prey.energy = e;
    prey.pid = -1;
    prey.fd = -1;
    return prey;
}

Obstacle createObstacle(int r, int c){
    Obstacle obstacle;
    obstacle.row = r;
    obstacle.col = c;
    return obstacle;
}




Map* createMap(int w, int h){
    Map* map = malloc(sizeof(Map));
    map->nObstacles = 0;
    map->nHunters = 0;
    map->nPreys = 0;
    map->width = w;
    map->height = h;
    map->hunters = NULL;
    map->preys = NULL;
    map->obstacles = NULL;
    return map;
}

void destroyMap(Map* map){
    free(map->hunters);
    free(map->preys);
    free(map->obstacles);
    free(map);
}


Map* readFromInput(){
    int x, y, z;

    scanf("%d %d", &x, &y);
    Map *map = createMap(x, y);

    scanf("%d", &map->nObstacles);
    map->obstacles = malloc(sizeof(Obstacle) * map->nObstacles);
    for(int j = 0; j < map->nObstacles; j++){
       scanf("%d %d", &x, &y);
       Obstacle o = createObstacle(x,y);
       map->obstacles[j] = o;
    }
    scanf("%d", &map->nHunters);
    map->hunters = malloc(sizeof(Hunter) * map->nHunters);
    for(int j = 0; j < map->nHunters; j++){
        scanf("%d %d %d", &x, &y, &z );
        Hunter h = createHunter(x,y,z);
        map->hunters[j] = h;
    }

    scanf("%d", &map->nPreys);
    map->preys = malloc(sizeof(Prey) * map->nPreys);
    for(int j = 0; j < map->nPreys; j++){
        scanf("%d %d %d", &x, &y, &z);
        Prey p = createPrey(x,y,z);
        map->preys[j] = p;
    }
    return map;
}

Type coordType(Map* map, int row, int column){
    for(int j = 0; j < map->nObstacles; j++){
        Obstacle o = map->obstacles[j];
        if(o.row == row && o.col == column)
            return OBSTACLE;
    }

    for(int j = 0; j < map->nHunters; j++){
        Hunter h = map->hunters[j];
        if(h.row == row && h.col == column){
            return HUNTER;
        }
    }

    for(int j = 0; j < map->nPreys; j++){
        Prey p = map->preys[j];
        if(p.row == row && p.col == column){
            return PREY;
        }
    }

    return EMPTY;

}

void printMap(Map *map){
    printf("+");
    for(int j = 0; j < map->width; j++)
        printf("-");
    printf("+\n");

    for(int j = 0; j < map->height; j++){
        printf("|");
        for(int k = 0; k < map->width; k++){
            Type type = coordType(map, j, k);
            switch(type){
                case PREY:
                    printf("P");
                    break;
                case OBSTACLE:
                    printf("X");
                    break;
                case HUNTER:
                    printf("H");
                    break;
                default:
                    printf(" ");

            }
        }
        printf("|\n");
    }



    printf("+");
    for(int j = 0; j < map->width; j++)
        printf("-");
    printf("+\n");

}

