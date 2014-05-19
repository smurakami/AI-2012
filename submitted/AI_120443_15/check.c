#include "game.h"

int inTheBoard(int x, int y){
  if((x >= 0)&&
     (y >= 0)&&
     (x < SIZE)&&
     (y < SIZE)) return 1;
  else return 0;
}

//動かせる駒を調べる
int checkBoard(state_t st){
  int i, j;
  struct point p;
  initPieceList(st);
  st->activePiece = 0;
  st->attack = 0;
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      if(st->bd[i][j] == st->player)//自分の駒だったら
      {
        p.x = i;
        p.y = j;
        checkPiece(st, p);
      }
    }
  }
  if(st->attack == 1){
    checkBoardToAttack(st);
  }
  return 0;
}


//その駒を動かせるか、動かせるならその方向を調べる。
int checkPiece(state_t st, struct point p){
  //はしっこに到達していたらだめ
  if((st->player == A)&&(p.x == 0)) return 0;
  if((st->player == B)&&(p.x == SIZE-1)) return 0;
  int dx, dy;//動き Aなら上へ、Bなら下へ
  int x, y;//動いた先
  piece_t current = st->piece[st->activePiece];//今操作するpiece
  pKind_t opposit;//敵の駒
  //どっちのターンかで場合分け
  if(st->player == A) {
    dx = -1;
    opposit = B;
  }else{
    dx = 1;
    opposit = A;}
  //左に動けるか検討
  dy = -1;
  x = p.x + dx;
  y = p.y + dy;
  if(p.y > 0){
    if(st->bd[x][y] == NONE){//そこが空いてたら
      current->left = 1;
    }else if((st->bd[x][y] == opposit) && inTheBoard(x + dx, y + dy)){//相手がいたら、とれるかどうか検討
      if(st->bd[x+dx][y+dy] == NONE){
        current->left = 2;
      }
    }
  }
  //右に動けるか検討
  dy = 1;
  x = p.x + dx;
  y = p.y + dy;
  if(p.y < SIZE - 1){
    if(st->bd[x][y] == NONE){//そこが空いてたら
      current->right = 1;
    }else if((st->bd[x][y] == opposit) && inTheBoard(x + dx, y + dy)){//相手がいたら、とれるかどうか検討
      if(st->bd[x+dx][y+dy] == NONE){
        current->right = 2;
      }
    }
  }
  if(current->right || current->left){
    st->activePiece++;
    current->enable = 1;
    current->p.x = p.x;
    current->p.y = p.y;
  }
  if((current->right==2) || (current->left==2)){
    st->attack = 1;
    current->enable = 2;
  }
   
  return 0;
}

//攻撃できる駒をしらべる
int checkPieceToAttack(state_t st, struct point p){
  //はしっこに到達していたらだめ
  if((st->player == A)&&(p.x == 0)) return 0;
  if((st->player == B)&&(p.x == SIZE-1)) return 0;
  int dx, dy;//動き Aなら上へ、Bなら下へ
  int x, y;//動いた先
  piece_t current;
  if(st->attack == 2) current = st->attackingPiece;
  else current = st->piece[st->activePiece];//今操作するpiece
  pKind_t opposit;//敵の駒
  //どっちのターンかで場合分け
  if(st->player == A) {
    dx = -1;
    opposit = B;
  }else{
    dx = 1;
    opposit = A;}
  //左に動けるか検討
  dy = -1;
  x = p.x + dx;
  y = p.y + dy;
  if((st->bd[x][y] == opposit) && inTheBoard(x + dx, y + dy)){//相手がいたら、とれるかどうか検討
    if(st->bd[x+dx][y+dy] == NONE){
      current->left = 2;
    }
    
  }
  //右に動けるか検討
  dy = 1;
  x = p.x + dx;
  y = p.y + dy;
  if((st->bd[x][y] == opposit) && inTheBoard(x + dx, y + dy)){//相手がいたら、とれるかどうか検討
    if(st->bd[x+dx][y+dy] == NONE){
      current->right = 2;
    }
    
  }
  if((current->right==2) || (current->left==2)){
    current->enable = 2;
    current->p.x = p.x;
    current->p.y = p.y;
    if(st->attack != 2) st->activePiece++;
    return 1;
  }
   
  return 0;

}

//その駒を動かせるか、動かせるならその方向を調べる
int checkBoardToAttack(state_t st){
  int i, j;
  struct point p;
  initPieceList(st);
  st->activePiece = 0;
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      if(st->bd[i][j] == st->player)//自分の駒だったら
      {
        p.x = i;
        p.y = j;
        checkPieceToAttack(st, p);
      }
    }
  }
  return 0;
}
