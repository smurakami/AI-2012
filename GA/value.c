#include "GAI.h"

/*AIの評価をする関数*/
/*

  
  以下に網膜から視角受容野に効率的に情報を伝達できているかを評価対象とする。
  1.視角受容野に達していない場合
  視角受容野から、網膜にアクセスできるAIまでの距離*10だけvalueからマイナス
  余分な神経細胞の数*10だけマイナス

  2.視角受容野に達している場合
  視角受容野到達ボーナスとしてプラスSIZE
  余分な神経細胞の数*10だけマイナス

  網膜の座標(3, 0)
  視角受容野の座標(SIZE * 4/3, SIZE * 4/3)
  とする。
  
 */
int valueOfAI(myAI* AI){
  int i, j;//counter
  int value = 0;
  int connected = 0;
  int min = SIZE * 2;
  point * current = AI->dest;
  runAIforGA(AI);
  initNeuron(AI);
  while(current->next != NULL){
    current = current->next;
    if(AI->neu[current->x][current->y].state & RETINA){//もしも網膜からたどりつけていたら
      value += SIZE*SIZE;
    }else{//もしも網膜から受容野にたどりつけていなかったら
      min = SIZE * 10;
      for(i = 0; i < SIZE; i++){
        for(j = 0; j < SIZE; j++){
          if((AI->neu[i][j].state & RETINA))
            if(distance(current->x, current->y, i, j) < min) min = distance(current->x, current->y, i, j);
        }
      }
      value -= min * 10;
    }
    //ニューロンが少ないほど高得点に
    for(i = 0; i < SIZE; i++){
      for(j = 0; j < SIZE; j++){
        if(AI->neu[i][j].state & RETINA) value -= 5;
      }
    }
  }
  //printf("value\n");
  //printAIforGA(AI);
  //cvWaitKey(0);
  AI->value = value;
  return 0;
}
