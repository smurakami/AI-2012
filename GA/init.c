#include "GAI.h"

/*人工頭脳の確保*/
myAI* allocAI(){
  myAI* AI = (myAI*)malloc(sizeof(myAI));
  if(AI == NULL){perror("allocAI"); exit(1);}
  return AI;
}
/*人工頭脳の初期化*/
myAI* initAI(myAI* AI){
  int i, j;//counter
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      AI->neu[i][j].state = 0;
      AI->neu[i][j].from = 0;
    }
  }
  AI->value = 0;
  AI->output = cvCreateImage(cvSize(SIZE, SIZE), IPL_DEPTH_8U, 3);
  AI->retina = getPoint(-1, -1);
  AI->dest = getPoint(-1, -1);
  //cvNamedWindow("output", 1);
  return AI;
}

/*興奮状態を初期化*/
int initNeuron(myAI * AI){
  int i, j;//counter
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      AI->neu[i][j].state = AI->neu[i][j].state & b1111_1100;
      AI->neu[i][j].from = 0;
    }
  }
  return 0;
}

int initRetina(myAI * AI){
  int i, j;//counter
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      AI->neu[i][j].state = AI->neu[i][j].state & b1111_0111;
    }
  }
  return 0;  
}

/*人工頭脳の解放*/
int freeAI(myAI* AI){
  cvReleaseImage(&(AI->output));
  free(AI);
  return 0;
}


/*世代を確保、初期化する*/
int initGeneration(myAI* generation[G_SIZE]){
  int i;//counter
  for(i = 0; i < G_SIZE; i++){
    generation[i] = allocAI();
    initAI(generation[i]);
  }
  return 0;
}
/*世代に割り当てたメモリを解放*/
int freeGeneration(myAI* generation[G_SIZE]){
  int i;//counter
  for(i = 0; i < G_SIZE; i++){
    freeAI(generation[i]);
  }
  return 0;
}

/*AIのシナプスをランダムにセット*/
int setAIatRandom(myAI* AI){
  int i, j;
  for(i = 0; i < SIZE; i++)
    for(j = 0; j < SIZE; j++){
      if(rand()%2){//50%の確率で右向きにシナプスをのばす
        if(!((j < SIZE-1) && 
             (AI->neu[i][j+1].state & LEFT)
             )){//左からシナプスがのびていない場合に限って
          AI->neu[i][j].state = AI->neu[i][j].state | RIGHT;//右向きにシナプスをのばす
        }   
      }
      //以下同様
      /* if(rand()%2){//50%の確率で上向きにシナプスをのばす */
      /*   if(!((i > 1) && */
      /*        (AI->neu[i-1][j].state & DOWN) */
      /*        )) AI->neu[i][j].state = AI->neu[i][j].state | UP;  */
      /* } */
      /* if(rand()%2){//50%の確率で左向きにシナプスをのばす */
      /*   if(!((j > 1) && */
      /*        (AI->neu[i][j-1].state & RIGHT) */
      /*        )) AI->neu[i][j].state = AI->neu[i][j].state | LEFT; */
      /* } */
      if(rand()%2){//50%の確率で下向きにシナプスをのばす
        if(!((i < SIZE - 1) &&
             (AI->neu[i+1][j].state & UP)
             )) AI->neu[i][j].state = AI->neu[i][j].state | DOWN; 
      }
    }  
  return 0;
}

//pointの確保
point* allocPoint(){
  point * p = (point *)malloc(sizeof(point));
  if(p == NULL){perror("point\n"); exit(1);}
  return p;
}

int initPoint(point * p){
  p->x = 0;
  p->y = 0;
  p->next = NULL;
  return 0;
}

point * getPoint(int x, int y){
  point * p = allocPoint();
  initPoint(p);
  p->x = x;
  p->y = y;
  return p;
}

int addPoint(point * p, int x, int y){
  point * current = p;
  while(current->next != NULL){
    current = current->next;
    if((current->x == x) && (current->y == y)) return 1;
  }
  current->next = getPoint(x, y);
  return 0;
}

//ニューロンの状態をコピー
int copyNeuron(myAI* dest, myAI* orig){
  int i, j;
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      dest->neu[i][j].state = orig->neu[i][j].state;
      dest->neu[i][j].from = orig->neu[i][j].from;
    }
  }
  
  return 0;
}
