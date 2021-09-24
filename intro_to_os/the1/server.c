
#include "server.h"
#include "messages.h"

#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include<sys/socket.h>
#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)



Server* createServer(){
    Server * server = malloc(sizeof(Server));
    server->map = readFromInput();
    return server;
}

void destroyServer(Server* server){
    
    destroyMap(server->map);
    free(server);
}




void init(Server* server){

    Map *map = server->map;

    for(int j = 0; j < map->nPreys; j++){
        int fd[2];
        PIPE(fd);

        pid_t pid = fork();
        if(pid == 0){
            for(int i = 3; i < fd[1]; i++)
                close(i);
            dup2(fd[1], 0);
            dup2(fd[1], 1);
            close(fd[1]);

            char firstArg[33], secondArg[33];
            sprintf(firstArg, "%d", server->map->width);
            sprintf(secondArg, "%d", server->map->height);
            char *path =  "./prey";
            char *programName = "prey";
            char *args[] = {programName, firstArg, secondArg, NULL};
            execv(path, args);

        }else{
            map->preys[j].pid = pid;
            map->preys[j].fd = fd[0];
            sendPreyState(map, j);
        }

    }

    for(int j = 0; j < map->nHunters; j++){
        int fd[2];
        PIPE(fd);

        pid_t pid = fork();
        if(pid == 0){
            for(int i = 3; i < fd[1]; i++)
                close(i);
            dup2(fd[1], 0);
            dup2(fd[1], 1);
            close(fd[1]);

            char firstArg[33] = {0}, secondArg[33] = {0};
            sprintf(firstArg, "%d", server->map->width);
            sprintf(secondArg, "%d", server->map->height);
            char *path =  "./hunter";
            char *programName = "hunter";
            char *args[] = {programName, firstArg, secondArg, NULL};
            execv(path, args);
        }else{
            map->hunters[j].pid = pid;
            map->hunters[j].fd = fd[0];
            sendHunterState(map, j);
        }

    }

    int nPipes = map->nHunters + map->nPreys;
    for(int i = 0; i < nPipes; i++)
        close(2 * i + 4);




}

int manhattan(Map* map, int hIndex, int pIndex){
    return abs(map->hunters[hIndex].row - map->preys[pIndex].row) + abs(map->hunters[hIndex].col - map->preys[pIndex].col);
}

int closestPrey(Map* map, int hIndex){
    int mh = INT_MAX;
    int pIndex = -1;
    for(int j = 0; j < map->nPreys; j++){
        int newMh = manhattan(map, hIndex, j);
        if( newMh < mh){
            mh = newMh;
            pIndex = j;
        }
    }
    return pIndex;
}

int closestHunter(Map* map, int pIndex){
    int mh = INT_MAX;
    int hIndex = -1;
    for(int j = 0; j < map->nHunters; j++){
        int newMh = manhattan(map, j, pIndex);
        if( newMh < mh){
            mh = newMh;
            hIndex = j;
        }
    }
    return hIndex;

}

int obstacleAt(Map* map, int row, int col){
    for(int j = 0; j < map->nObstacles; j++){
        if(map->obstacles[j].row == row  && map->obstacles[j].col == col){
            return 1;
        }
    }
    return 0;
}

int preyAt(Map* map, int row, int col){
    for(int j = 0; j < map->nPreys; j++){
        if(map->preys[j].row == row && map->preys[j].col == col){
            return 1;
        }
    }
    return 0;
}

int hunterAt(Map *map, int row, int col){
    for(int j = 0; j < map->nHunters; j++){
        if(map->hunters[j].row == row && map->hunters[j].col == col){
            return 1;
        }
    }
    return 0;
}

void sendHunterState(Map* map, int hIndex){
    server_message msg;
    memset(&msg, 0, sizeof(msg));
    int row = map->hunters[hIndex].row;
    int col = map->hunters[hIndex].col;

    msg.pos.x = row;
    msg.pos.y = col;

    int pIndex = closestPrey(map, hIndex);
    msg.adv_pos.x = map->preys[pIndex].row;
    msg.adv_pos.y = map->preys[pIndex].col;


    msg.object_count = 0;
    if(hunterAt(map, row + 1, col) || obstacleAt(map, row + 1, col)){
        msg.object_pos[msg.object_count].x = row + 1;
        msg.object_pos[msg.object_count++].y = col;
    }
    if(hunterAt(map, row - 1, col) || obstacleAt(map, row - 1, col)){
        msg.object_pos[msg.object_count].x = row - 1;
        msg.object_pos[msg.object_count++].y = col;
    }
    if(hunterAt(map, row, col + 1) || obstacleAt(map, row, col + 1)){
        msg.object_pos[msg.object_count].x = row;
        msg.object_pos[msg.object_count++].y = col + 1;
    }
    if(hunterAt(map, row, col - 1) || obstacleAt(map, row, col - 1)){
        msg.object_pos[msg.object_count].x = row;
        msg.object_pos[msg.object_count++].y = col - 1;
    }
    write(map->hunters[hIndex].fd, &msg, sizeof(msg));
}


void sendPreyState(Map* map, int pIndex){
    server_message msg;
    memset(&msg, 0, sizeof(msg));

    int row = map->preys[pIndex].row;
    int col = map->preys[pIndex].col;

    msg.pos.x = row;
    msg.pos.y = col;

    int hIndex = closestHunter(map, pIndex);
    msg.adv_pos.x = map->hunters[hIndex].row;
    msg.adv_pos.y = map->hunters[hIndex].col;


    msg.object_count = 0;
    if(preyAt(map, row + 1, col) || obstacleAt(map, row + 1, col)){
        msg.object_pos[msg.object_count].x = row + 1;
        msg.object_pos[msg.object_count++].y = col;
    }
    if(preyAt(map, row - 1, col) || obstacleAt(map, row - 1, col)){
        msg.object_pos[msg.object_count].x = row - 1;
        msg.object_pos[msg.object_count++].y = col;
    }
    if(preyAt(map, row, col + 1) || obstacleAt(map, row, col + 1)){
        msg.object_pos[msg.object_count].x = row;
        msg.object_pos[msg.object_count++].y = col + 1;
    }
    if(preyAt(map, row, col - 1) || obstacleAt(map, row, col - 1l)){
        msg.object_pos[msg.object_count].x = row;
        msg.object_pos[msg.object_count++].y = col - 1;
    }

    write(map->preys[pIndex].fd, &msg, sizeof(msg));


}




void fd2TypeIndex(Map* map, int fd, int* index, Type* type){
    for(int j = 0; j < map->nPreys; j++){
        if(map->preys[j].fd == fd){
            *index = j;
            *type = PREY;
            return;
        }
    }
    for(int j = 0; j < map->nHunters; j++){
        if(map->hunters[j].fd == fd){
            *index = j;
            *type = HUNTER;
            return ;
        }
    }
}




int updateMap(Map* map, int infd, coordinate req){
    Type type;
    int objectIndex;
    fd2TypeIndex(map, infd, &objectIndex, &type);
    if(type == HUNTER){
        if(!hunterAt(map, req.x, req.y) && !obstacleAt(map,req.x,req.y)){
            map->hunters[objectIndex].row = req.x;
            map->hunters[objectIndex].col = req.y;
            map->hunters[objectIndex].energy -= 1;
            sendHunterState(map, objectIndex);
            return 1; 
        }else{
            sendHunterState(map, objectIndex);
            return 0;
        }

    }else if(type == PREY){
        if(!preyAt(map, req.x, req.y) && !obstacleAt(map, req.x, req.y)){
            map->preys[objectIndex].row = req.x;
            map->preys[objectIndex].col = req.y;
            sendPreyState(map, objectIndex);
            return 1;
        }else{
            sendPreyState(map, objectIndex);
            return 0;
        }
    }

}

void removePrey(Map* map, int index){
    close(map->preys[index].fd);
    kill(map->preys[index].pid, SIGTERM);
    int status;
    waitpid(map->preys[index].pid, &status, 0);
    for(int j = index + 1; j < map->nPreys; j++){
        map->preys[j - 1] = map->preys[j];
    }
    map->nPreys--;
    return;
}

void killPreys(Map *map){
    for(int j = 0; j < map->nHunters; j++){
        for(int k = 0; k < map->nPreys; k++){
            if(map->hunters[j].row == map->preys[k].row && map->hunters[j].col == map->preys[k].col){
                map->hunters[j].energy += map->preys[k].energy;
                printf("ENERGY IS %d\n", map->hunters[j].energy);
                removePrey(map, k);
            }
        }
    }

}


void removeHunter(Map* map, int index){
    close(map->hunters[index].fd);
    kill(map->hunters[index].pid, SIGTERM);
    int status;
    waitpid(map->hunters[index].pid, &status, 0);
    for(int j = index + 1; j < map->nHunters; j++){
        map->hunters[j - 1] = map->hunters[j];
    }
    map->nHunters--;

}

void checkHunters(Map *map){
    for(int j = 0; j < map->nHunters; j++){
        if(map->hunters[j].energy == 0){
            removeHunter(map, j);
        }

    }


}

void loop(Server* server){
    int nPipes = server->map->nHunters + server->map->nPreys;
    struct pollfd* fds = malloc(nPipes * sizeof(struct pollfd));
    for(int i = 0; i < nPipes; i++){
        fds[i].fd = 3 + 2 * i;
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    };

    while(server->map->nHunters && server->map->nPreys){

        int ret = poll(fds, nPipes, -1);
        int updated = 0;
	for(int i = 0; i < nPipes; i++){
            if(fds[i].revents & POLLIN){
                fds[i].revents = 0;
                ph_message inMsg;
                read(fds[i].fd, &inMsg, sizeof(inMsg));
                updated = updateMap(server->map, fds[i].fd, inMsg.move_request) || updated;
            }else {

            }
        }
        killPreys(server->map);
        checkHunters(server->map);
        if(updated)
            printMap(server->map);
    }

    
    for(int j = 0; j < server->map->nHunters; j++){
        int status;
        close(server->map->hunters[j].fd);
        kill(server->map->hunters[j].pid, SIGTERM);
        waitpid(server->map->hunters[j].pid, &status, 0);
    }

    for(int j = 0; j < server->map->nPreys; j++){
        int status;
        close(server->map->preys[j].fd);
        kill(server->map->preys[j].pid, SIGTERM);
        waitpid(server->map->preys[j].pid, &status, 0);
    }


    free(fds);
}



int main(){
    Server *server = createServer();

    printMap(server->map);
    init(server);
    loop(server);
    destroyServer(server);
    return 0;
}
