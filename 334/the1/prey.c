
#include <stdio.h>
#include <unistd.h>

#include "messages.h"
#include <math.h>
#include <stdlib.h>

int isObstructed(coordinate c, int obj_count, coordinate* obj_pos){
    for(int j = 0; j < obj_count; j++){
        if(c.x == obj_pos[j].x && c.y == obj_pos[j].y){
            return 1;
        }
    }
    return 0;
}

int manhattan(coordinate c1, coordinate c2){
    return abs(c1.x - c2.x) + abs(c1.y - c2.y);

}

void possibleMove(coordinate c, server_message msg, ph_message* outmsg, int w, int h){
    if(c.x >= 0 && c.x < h && c.y >= 0 && c.y < w){
        int currentMH = manhattan(msg.pos, msg.adv_pos);
        if(!isObstructed(c, msg.object_count, msg.object_pos)){
            int newMH = manhattan(c, msg.adv_pos);
            if(newMH > currentMH){
                outmsg->move_request = c;
            }
        }
    }
}


int main(int argc, char* argv[]){
    coordinate currentCoord;
    int w = strtol(argv[1], NULL, 10);
    int h = strtol(argv[2], NULL, 10);
    while(1){
        server_message inMsg;
        read(0, &inMsg, sizeof(inMsg));
        currentCoord = inMsg.pos;

        coordinate n1, n2, n3, n4;

        n1.x = currentCoord.x + 1;
        n1.y = currentCoord.y;

        n2.x = currentCoord.x - 1;
        n2.y = currentCoord.y;

        n3.x = currentCoord.x;
        n3.y = currentCoord.y + 1;

        n4.x = currentCoord.x;
        n4.y = currentCoord.y - 1;

        ph_message outmsg;
        outmsg.move_request = currentCoord;
        possibleMove(n1, inMsg, &outmsg, w,h);
        possibleMove(n2, inMsg, &outmsg, w,h);
        possibleMove(n3, inMsg, &outmsg, w,h);
        possibleMove(n4, inMsg, &outmsg, w,h);
        write(1, &outmsg, sizeof(outmsg));

        usleep(10000*(1+rand()%9));

    }

    return 0;
}



