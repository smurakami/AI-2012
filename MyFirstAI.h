#include <stdio.h>
#include <stdlib.h>
#define SIZE 100
#define CLR \033[2J

//
typedef struct neuron{

    /*神経細胞が興奮しているかどうかを表す。
      0:静止
      1:興奮
    */
    char act;
    
    /* axon:軸索を表す
    axon[0]:右向き
    axon[1]:上向き
    axon[2]:左向き
    axon[3]:下向き
    */
    char axon[4];

} neuron_t;

/*人口頭脳*/
typedef struct AI{
    neuron_t neuron[SIZE][SIZE];
}* AI_t;


int initAI(AI_t AI);
int think(AI_t AI);
int MyFirstAI();
