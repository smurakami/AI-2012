#include "MyFirstAI.h"
#include <sys/time.h>

//現在時間を返す関数
double currentTime(){
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return tp.tv_sec + tp.tv_usec * 1.0E-6;
}

//人口頭脳の初期化
int initAI(AI_t AI){
    int i, j, k;//counter

    for(i = 0; i < SIZE; i++){
        for(j = 0; j < SIZE; j++){
            AI->neuron[i][j].act = 0;
            for(k = 0; k < 4; k++){
                AI->neuron[i][j].axon[k] = 0;
            }
        }
    }

    return 0;
}

int MyFirstAI(){

    AI_t AI = (AI_t)malloc(sizeof(struct AI));

    initAI(AI);
    
    while(1){
       
    }

    return 0;
}
