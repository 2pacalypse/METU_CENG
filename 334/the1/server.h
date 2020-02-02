#include "map.h"
#include <poll.h>


typedef struct Server{
    Map* map;
} Server;


Server* createServer();
void destroyServer(Server* server);

void sendHunterState(Map*, int);
void sendPreyState(Map*, int);

void init(Server* server);
void loop(Server* server);

