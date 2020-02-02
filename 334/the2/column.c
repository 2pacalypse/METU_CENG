#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>


typedef enum ANT_STATE  {WITH_FOOD, WITHOUT_FOOD, TIRED, SLEEPING_WITHOUT_FOOD, SLEEPING_WITH_FOOD} ANT_STATE;

typedef struct Position{
    int x;
    int y;
} Position;

typedef struct Ant{
    Position pos;
    ANT_STATE state;
    pthread_t thread;
    pthread_cond_t cond;
    pthread_mutex_t mut;
} Ant;


Ant create_ant(int x, int y){
    Ant ant;
    ant.pos.x = x;
    ant.pos.y = y;
    ant.state = WITHOUT_FOOD;
    pthread_mutex_init(&(ant.mut), NULL);
    pthread_cond_init(&(ant.cond), NULL);
    return ant;
}

void* ant_worker(void* ant_void_ptr);

void startAnt(Ant* ant){
    pthread_t thread;
    pthread_create(&thread, NULL, ant_worker, ant);
    ant->thread = thread;
}

int check_neighbor(Ant *ant, char c, Position *retpos){
    Position p = ant->pos;
    int index = 0;
    for(int j = -1; j <= 1; j++){
        for(int k = -1; k <= 1; k++){ 
            if(j == 0 && k == 0) continue;
            if(p.x + j >= 0 && p.x + j < GRIDSIZE && p.y + k >= 0 && p.y + k < GRIDSIZE && lookCharAt(p.x + j,p.y + k) == c){
                retpos[index].x = p.x + j;
                retpos[index].y = p.y + k;
                index++;
            }
        }
    }
    return index;
}



pthread_mutex_t N, L;
pthread_mutex_t M[GRIDSIZE];

void* ant_worker(void* ant_void_ptr){
     //ENABLING THE CANCEL FUNCTIONALITY
    int prevType;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &prevType);

    Ant *ant = (Ant*) ant_void_ptr;
    while(1){

        pthread_mutex_lock(&(ant->mut));
        while(ant->state == SLEEPING_WITH_FOOD || ant->state == SLEEPING_WITHOUT_FOOD){
            pthread_cond_wait(&(ant->cond), &(ant->mut));
        }
        pthread_mutex_unlock(&(ant->mut));
        Position pos = ant->pos;

        pthread_mutex_lock(&L);
        pthread_mutex_lock(&N);
      //  pthread_mutex_lock (&M);
       for(int j = pos.x -1; j <= pos.x + 1; j++){
           if(j >= 0 && j < GRIDSIZE)
               pthread_mutex_lock(&M[j]);
       }

        pthread_mutex_unlock(&N);
        pthread_mutex_unlock(&L);

        if(ant->state == WITHOUT_FOOD){
            Position retpos[8];
            int isFood = check_neighbor(ant, 'o', retpos);
            if(isFood){
                putCharTo(ant->pos.x, ant->pos.y, '-');
                putCharTo(retpos[0].x, retpos[0].y, 'P');
                ant->state = WITH_FOOD;
                ant->pos.x = retpos[0].x;
                ant->pos.y = retpos[0].y;
            }else{
                int isEmpty = check_neighbor(ant, '-', retpos);
                if(isEmpty){
                    int dice = rand() % isEmpty;
                    putCharTo(ant->pos.x, ant->pos.y, '-');
                    putCharTo(retpos[dice].x, retpos[dice].y, '1');
                    ant->pos.x = retpos[dice].x;
                    ant->pos.y = retpos[dice].y;
                }
            }
        }else if(ant->state == WITH_FOOD){
            Position retpos[8];
            int isFood = check_neighbor(ant, 'o', retpos);
            int isEmpty = check_neighbor(ant, '-', retpos);
            if(isFood){
                if(isEmpty){
                    putCharTo(ant->pos.x, ant->pos.y, 'o');
                    putCharTo(retpos[0].x, retpos[0].y, '1');
                    ant->state = TIRED;
                    ant->pos.x = retpos[0].x;
                    ant->pos.y = retpos[0].y;
                }
            }else{
                if(isEmpty){
                    int dice = rand() % isEmpty;
                    putCharTo(ant->pos.x, ant->pos.y, '-');
                    putCharTo(retpos[dice].x, retpos[dice].y, 'P');
                    ant->pos.x = retpos[dice].x;
                    ant->pos.y = retpos[dice].y; 
                }
            }
        }else if(ant->state == TIRED){
            Position retpos[8];
            int isEmpty = check_neighbor(ant, '-', retpos);
            if(isEmpty){
                int dice = rand() % isEmpty;
                putCharTo(ant->pos.x, ant->pos.y, '-');
                putCharTo(retpos[dice].x, retpos[dice].y, '1');
                ant->state = WITHOUT_FOOD;
                ant->pos.x = retpos[dice].x;
                ant->pos.y = retpos[dice].y;
            }

        }

    
    for(int j = pos.x -1; j <= pos.x + 1; j++){
        if(j >= 0 && j < GRIDSIZE)
            pthread_mutex_unlock(&M[j]);
    }

   // pthread_mutex_unlock (&M);
 //   pthread_mutex_unlock(&L);

    usleep((getDelay() + rand() % 10) * 1000);

    }
}




int main(int argc, char *argv[]) {
    srand(time(NULL));
     
    int nAnts, nFoods, simTime;
    nAnts = strtol(argv[1], NULL, 10);
    nFoods = strtol(argv[2], NULL, 10);
    simTime = strtol(argv[3], NULL, 10);

    Ant* ant_list = malloc(sizeof(Ant) * nAnts); 

    int i,j;
    for (i = 0; i < GRIDSIZE; i++) {
        for (j = 0; j < GRIDSIZE; j++) {
            putCharTo(i, j, '-');
        }
    }
    int a,b;
    for (i = 0; i < nAnts; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');
        putCharTo(a, b, '1');
   
        ant_list[i] = create_ant(a,b);
//        startAnt(&ant_list[i]);

    }
    for (i = 0; i < nFoods; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');
        putCharTo(a, b, 'o');
    }

    for(i = 0; i < nAnts; i++){
        startAnt(&ant_list[i]);
    }


    // you have to have following command to initialize ncurses.
    startCurses();
    
    // You can use following loop in your program. But pay attention to 
    // the function calls, they do not have any access control, you 
    // have to ensure that.


    time_t start = time(NULL);
    char c;
    while (TRUE) {
        time_t end = (double)(time(NULL) - start);
        if(end > simTime){
           // free(ant_list);
            for(int i = 0; i < nAnts; i++){
                pthread_cancel(ant_list[i].thread);
                pthread_join(ant_list[i].thread, NULL);
            }
            free(ant_list);
            endCurses();
            return 0;
        }

        pthread_mutex_lock(&N);
        //pthread_mutex_lock(&M);

       for(int j = 0; j < GRIDSIZE; j++){
               pthread_mutex_lock(&M[j]);
       }

        pthread_mutex_unlock(&N);
        drawWindow();
        //pthread_mutex_unlock(&M);

       for(int j = 0; j < GRIDSIZE; j++){
               pthread_mutex_unlock(&M[j]);
       }


        c = 0;
        c = getch();

        if (c == 'q' || c == ESC) break;
        if (c == '+') {
            setDelay(getDelay()+10);
        }
        if (c == '-') {
            setDelay(getDelay()-10);
        }
        if (c == '*') {
            setSleeperN(getSleeperN()+1);
            Ant *a = &ant_list[getSleeperN() - 1];

            pthread_mutex_lock(&(a->mut));
            
            
          //  pthread_mutex_lock (&M);

            ANT_STATE s = a->state;
            if(s == WITHOUT_FOOD){
                a->state= SLEEPING_WITHOUT_FOOD;
                putCharTo(a->pos.x, a->pos.y, 'S');

            }else if(s == WITH_FOOD){
                a->state = SLEEPING_WITH_FOOD;
                putCharTo(a->pos.x, a->pos.y, '$');

            }
          //  pthread_mutex_unlock (&M);
            pthread_mutex_unlock(&(a->mut));

        }
        if (c == '/') {
            setSleeperN(getSleeperN()-1);
            Ant *a = &ant_list[getSleeperN()];

            pthread_mutex_lock(&(a->mut));
         //   pthread_mutex_lock (&M);

            ANT_STATE s = ant_list[getSleeperN()].state;
            if(s == SLEEPING_WITHOUT_FOOD){
                a->state= WITHOUT_FOOD;
                putCharTo(a->pos.x, a->pos.y, '1');
            }else if(s == SLEEPING_WITH_FOOD){
                a->state = WITH_FOOD;
                putCharTo(a->pos.x, a->pos.y, 'P');
            }
            pthread_cond_signal(&(a->cond)); 
          //  pthread_mutex_unlock (&M);
            pthread_mutex_unlock(&(a->mut));

        }
        usleep(DRAWDELAY);
        
        // each ant thread have to sleep with code similar to this
        //usleep(getDelay() * 1000 + (rand() % 5000));
    }
    
    // do not forget freeing the resources you get
    endCurses();


    return 0;
}
