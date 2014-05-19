#include "game.h"

//盤面の評価値の算出
int estimate(state_t st){
  int i, j;//counter
  int value = 0;//その盤面の評価値
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      if(st->bd[i][j] != NONE){
        value += estimatePiece(st, i, j);
      }
    }
  }
  st->value = value;
  return 0;
}

int estimatePiece(state_t st, int x, int y){
  int value = 0;
  int sign;
  int forward;//前
  pKind_t friend = st->bd[x][y];
  pKind_t opposit;
  
  if(st->bd[x][y] == A){
    sign = 1;
    forward = -1;
    opposit = B;
  }
  else if(st->bd[x][y] == B){
    sign = -1;
    forward = 1;
    opposit = A;
  }
  else return 0;

  //駒の基本ポイント加算
  value += valueOfPiece;
  //前進ポイント加算
  if(friend == A) value += valueOfAdvance * (SIZE - 1 - x);
  else value += valueOfAdvance * (x);

   //向こう岸に到達していたら、特別ボーナス
  if((friend == A)&&(x == 0)) value += valueOfKing;
  else if((friend == B)&&(x == (SIZE -1))) value += valueOfKing;
  /*ここで、駒が守られていたら加算、とられそうだったら減算の処理を追加したい*/
  else if(((friend == A)&&(x != SIZE - 1))||
          ((friend == B)&&(x != 0))){
    if((y == 0)||(y == SIZE - 1)) value += valueOfProtecter;//壁際で安全
    else{//この時点で、はじっこに無い事が確定している
      if(st->bd[x - forward][y-1] == friend) value += valueOfProtecter;//後ろ盾
      if(st->bd[x - forward][y+1] == friend) value += valueOfProtecter;//後ろ盾
      if(st->player == opposit){//今が相手の手番だったら
        if(((st->bd[x - forward][y-1] == NONE)&&
            (st->bd[x + forward][y+1] == opposit))||
           ((st->bd[x - forward][y+1] == NONE)&&
            (st->bd[x + forward][y-1] == opposit))) value += valueOfAffecter;//とられる
      }
    }
  }
  
  return value * sign;
}
