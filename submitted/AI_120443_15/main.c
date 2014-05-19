#include "game.h"
#include <stdio.h>

int main(){
  //パラメーター設定
  valueOfPiece = 20;//駒の基本ポイント
  valueOfProtecter = 10;//駒が守られている時の加算ポイント
  valueOfAffecter = -10;//駒が脅威にさらされている時の加算ポイント マイナス
  valueOfAdvance = 5;//駒が一マス進むごとに加算
  valueOfKing = 50;//駒が向こう岸に到達した場合加算

  state_t st = allocState();
  initState(st);
  setBoard(st);
  pKind_t winner = runGameH2C(st);
  printBoard(st);
  if(winner == A) printf("SCORE : %d WIN!!\n", st->value);
  else if(winner == B) printf("SCORE : %d LOSE...\n", st->value);
  else printf("SCORE : %d DRAW\n", st->value);
  
  freeState(st);
  return 0;
}
