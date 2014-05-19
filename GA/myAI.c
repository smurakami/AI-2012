#include "GAI.h"

/********************************
         人工頭脳の設計
********************************/ 
/*
  以下のような構造体で人工頭脳を表現する。
  人工頭脳は、人工頭脳myAI, 人工神経細胞neuronの二層構造にする。

  人工頭脳AIは、平面上に人工神経細胞が正方形にSIZE*SIZE個集まったものである。
  
  神経細胞では、八桁の二進数により、神経細胞の状態を示す。
  神経細胞の状態を表す変数を、neuron.stateとする.
  便宜のため、コメント中ではstateのx桁目をstate-xと記載する

  このプログラムのなかでは、二進数の表記法を以下のようにする。
  bxxxx_xxxx
  ex.)b0010_0001 = 65
  これは、"bin_system.h"の中で定義されている。
  
  stateが神経細胞の状態を表す規則は以下のようにする。
  state:    表す情報     
  b0000_000x      そのニューロンが興奮しているか 
  b0000_00x0      隣に興奮しているニューロンがあって、その細胞からシナプスがのびているか.[近所の情報]
  b0000_0x00      (空き)
  b0000_x000      今回のみ使う場所。その神経細胞に網膜からたどりつく事ができるか。
  b000x_0000      右向きにシナプスがのびているか 
  b00x0_0000      上向きにシナプスがのびているか 
  b0x00_0000      左向きにシナプスがのびているか 
  bx000_0000      下向きにシナプスがのびているか

  
*/





/*神経細胞間の情報の伝達*/
/*
  神経細胞間の情報の伝達は、前半部communicate1と、後半部communicate2で分けて行う。
  communicate1ではAIの中の全神経細胞のうち興奮している(state-1 == 1)ものを調べ、
  その神経細胞と隣接していて、シナプスでつながっている細胞を探し、その細胞のstate-2を1にする。
  communicate2では,再びAIの中の全神経細胞を調べ、state-2が1である場合、state-1 == 1(興奮)
  とする。
*/

int communicate1(myAI* AI){
  int i, j;//counter
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      if(AI->neu[i][j].state & b0000_0001){//もしも興奮していたら
        if(AI->neu[i][j].state & RIGHT){//rightにシナプスがのびていたら
          if(j < SIZE - 1) AI->neu[i][j+1].state = AI->neu[i][j+1].state | NEIB; 
        }
        if(AI->neu[i][j].state & UP){//upにシナプスがのびていたら
          if(i > 1) AI->neu[i-1][j].state = AI->neu[i-1][j].state | NEIB; 
        }
        if(AI->neu[i][j].state & LEFT){//leftにシナプスがのびていたら
          if(j > 1) AI->neu[i][j-1].state = AI->neu[i][j-1].state | NEIB; 
        }
        if(AI->neu[i][j].state & DOWN){//downにシナプスがのびていたら
          if(i < SIZE - 1) AI->neu[i+1][j].state = AI->neu[i+1][j].state | NEIB; 
        }
      }
    }
  }
  return 0;
}

int communicate2(myAI* AI){
  int i, j;//counter
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      if(AI->neu[i][j].state & b0000_0010){//興奮しているご近所さんとつながってたら
        AI->neu[i][j].state = AI->neu[i][j].state | b0000_0001;//興奮させる
        AI->neu[i][j].state = AI->neu[i][j].state | b0000_1000;//網膜からたどりつけることをメモ
        AI->neu[i][j].state = AI->neu[i][j].state & b1111_1101;//近所の情報をリセット
      }else{
        AI->neu[i][j].state = AI->neu[i][j].state & b1111_1110;//興奮状態を解除
      }
    }
  }
  return 0;
}




