//FORSENNITE v.1.1.2 - Vittorio Mignini
#include "gamelib.h"

int main(void){
    bool playcheck=0;
    time_t t;
    srand((unsigned)time(&t)); //to initialize random seed
    do{
        do{
            Menu(&playcheck);
        }while(!playcheck);
        play();
        cleanplayer();
    }while(1);
}