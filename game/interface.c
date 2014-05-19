#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "game.h"

/*板を操作するためのインターフェース*/
//ある文字が数字か判断
int isnum(char c){
  if((c > '0')||
     (c <= '9')) return 1;
  else return 0;
}
//文字列を読み込み、座標に変換
void stringToInt(point_t p){
  int i;
  char buffer[10];
//数字が二個入るまでfgetsしつづける
  while(1){
    i = 0;
    fgets(buffer, 10, stdin);
    while(!isnum(buffer[i])){
        i++;
        if(i > 9) break;
    }
    if(i > 9) continue;
    p->x = buffer[i] - '0';
    i++;
    if(i > 9) continue;    
    while(!isnum(buffer[i])){
      i++;
      if(i > 9) break;
    }
    if(i > 9) continue;
    p->y = buffer[i] - '0';
    break;
  }
}

void choosePoint(point_t p){
  stringToInt(p);  
  while((p->x >= SIZE)||
        (p->y >= SIZE)||
        (p->x < 0)||
        (p->y < 0)){
    printf("choose point in the board\n");
    stringToInt(p);
  }
}

int choosePiece(state_t st, point_t p){
  char player;
  int i;
  if(st->player == A) player = 'A';
  else player = 'B';
  while(1){
    printBoard(st);
    printf("TURN %c : choose your piece: (ex. enter XY to chooce piece at (X, Y))\n", player);
    printf("you can move:");
    for(i = 0; i < st->activePiece; i++) printf("(%d, %d), ", st->piece[i]->p.x, st->piece[i]->p.y);
    printf("\n");
    
    choosePoint(p);
    if(st->bd[p->x][p->y] != st->player){
      printf("the piece on (%d, %d) is not yours\n", p->x, p->y);
      continue;
    }
    for(i = 0; i < st->activePiece; i++){
      if((st->piece[i]->p.x==p->x)&&
         (st->piece[i]->p.y==p->y))
        break;
    }
    if(i == st->activePiece){//選んだ駒が動かせないやつだった
      printf("you can't move the piece on (%d, %d)\n", p->x, p->y);
      continue;
    }
    st->pieceToMove = st->piece[i];
    break;//ここまで来たら駒の選択は終了
  }

  return 0;//終了
}

int move(state_t st, point_t p, direction_t d){
  if(d == other) return 1;
  int dx, dy;//たてよこのうごき
  pKind_t piece = st->bd[p->x][p->y];

  switch(piece){
  case A:
    dx = -1;
    break;
  case B:
    dx = 1;
    break;
  default:
    return 1;
  }
  /*駒をとらない場合*/
  if(st->attack == 0){
    switch(d){
    case R:
      if(st->pieceToMove->right == 0) return 1;
      dy = 1;
      break;
    case L:
      if(st->pieceToMove->left == 0) return 1;
      dy = -1;
      break;
    default:
      return 1;
    }
    st->bd[p->x][p->y] = NONE;
    st->bd[p->x + dx][p->y + dy] = st->player;
  }
  

  /* pKind_t opposit; */
  /* if(piece == A) opposit = B; */
  /* else opposit = A; */
  
  /* if(st->bd[p->x+dx][p->y+dy] == NONE){ */
  /*   st->bd[p->x+dx][p->y+dy] = piece; */
  /*   st->bd[p->x][p->y] = NONE; */
  /*   return 0; */
  /* }else if(st->bd[p->x+dx][p->y+dy] == opposit){ */
  /*   if(st->bd[p->x+dx+dx][p->y+dy+dy] == NONE){ */
  /*   st->bd[p->x+dx+dx][p->y+dy+dy] = piece; */
  /*   st->bd[p->x+dx][p->y+dy] = piece; */
  /*   st->bd[p->x][p->y] = NONE; */
  /*   //ここでさらにうごけるかチェックしたい */
  /*   return 0; */
  /*   } */
  /* }else{ */
  /*   return 0; */
  /* } */
  /* return -1;//ここに処理がくることはない */
  return -1;
}

int movePieace(state_t st, point_t p){
  direction_t d;
  while(1){
    choosePiece(st, p);
    printf("you chose (%d, %d): choose direction\n", p->x, p->y);
    printf("press 'L' or 'R'\n");
    
    int c = tolower(getchar());
    getchar();
    switch(c){
    case 'r':
      d = R;
      break;
    case 'l':
      d = L;
      break;
    default:
      d = other;
      break;
    }
    if(move(st, p, d) == 1){
      printf("moving failed");//動かすのに失敗したらやりなおし
      continue;
    }
    break;
  }

  return 0;
}
