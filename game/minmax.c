#include "game.h"
#include <stdio.h>
#include <stdlib.h>

int minmax(state_t st, int depth){
  int i;
  if(depth < 1) myError("minmax: too small depth\n");
  if(st->attack != 2) depth--;
  if(depth == 0){
    estimate(st);
    return 0;
  }
  int dx;
  pKind_t opposit;
  if(st->player == A){
    opposit = B;
    dx = -1;
  }
  else if(st->player == B){
    opposit = A;
    dx = 1;
  }
  else myError("player undefined\n");
  
  checkBoard(st);
  if(st->activePiece == 0){//終局になっていたら
    estimate(st);
    return 0;
  }
  int min = 1000000;
  int max = -1000000;
  int maxIndex;
  int minIndex;
  direction_t maxDirection;
  direction_t minDirection;
  state_t nextSt = allocState();
  point_t p = allocPoint();
  direction_t d;
  if(st->attack == 0){//攻撃しない場合
    for(i = 0; i < st->activePiece; i++){
      if(st->piece[i]->enable){
        if(st->piece[i]->left){
          copyState(nextSt, st);
          p->x = st->piece[i]->p.x;
          p->y = st->piece[i]->p.y;
          d = L;
          nextSt->pieceToMove = nextSt->piece[i];
          moveForMinMax(nextSt, p, d);
          nextSt->player = opposit;
          minmax(nextSt, depth);
          if(nextSt->value > max){
            max = nextSt->value;
            maxIndex = i;
            maxDirection = d;
          }
          if(nextSt->value < min){
            min = nextSt->value;
            minIndex = i;
            minDirection = d;
          }
        }
        if(st->piece[i]->right){
          copyState(nextSt, st);
          p->x = st->piece[i]->p.x;
          p->y = st->piece[i]->p.y;
          d = R;
          nextSt->pieceToMove = nextSt->piece[i];
          moveForMinMax(nextSt, p, d);
          nextSt->player = opposit;
          minmax(nextSt, depth);
          if(nextSt->value > max){
            max = nextSt->value;
            maxIndex = i;
            maxDirection = d;
          }
          if(nextSt->value < min){
            min = nextSt->value;
            minIndex = i;
            minDirection = d;
          }
        }
      }
    }
    if(st->player == A){
      st->value = max;
      st->valueIndex = maxIndex;
      st->valueDirection = maxDirection;
    }else if(st->player == B){
      st->value = min;
      st->valueIndex = minIndex;
      st->valueDirection = minDirection;
    }
  }else if(st->attack == 1){//攻撃する場合
    for(i = 0; i < st->activePiece; i++){
      if(st->piece[i]->enable){
        if(st->piece[i]->left){
          copyState(nextSt, st);
          p->x = st->piece[i]->p.x;
          p->y = st->piece[i]->p.y;
          d = L;
          int dy = -1;
          nextSt->pieceToMove = nextSt->piece[i];
          moveForMinMax(nextSt, p, d);
          /*さらにとれる場所が無いかチェック*/
          struct point p2;
          p2.x = p->x+dx+dx;
          p2.y = p->y+dy+dy;
          nextSt->attack = 2;//二度目以降のアタックのサイン
          nextSt->attackingPiece->p.x = p2.x;
          nextSt->attackingPiece->p.y = p2.y;
          nextSt->attackingPiece->left = 0;;
          nextSt->attackingPiece->right = 0;
          
          if(checkPieceToAttack(nextSt, p2)){
            minmax(nextSt, depth);//二度目の攻撃
          }else{
            nextSt->attack = 0;
            nextSt->player = opposit;
            minmax(nextSt, depth);
          }
          if(nextSt->value > max){
            max = nextSt->value;
            maxIndex = i;
            maxDirection = d;
          }
          if(nextSt->value < min){
            min = nextSt->value;
            minIndex = i;
            minDirection = d;
          }
        }
        if(st->piece[i]->right){
          copyState(nextSt, st);
          p->x = st->piece[i]->p.x;
          p->y = st->piece[i]->p.y;
          d = R;
          int dy = 1;
          nextSt->pieceToMove = nextSt->piece[i];
          moveForMinMax(nextSt, p, d);
          /*さらにとれる場所が無いかチェック*/
          struct point p2;
          p2.x = p->x+dx+dx;
          p2.y = p->y+dy+dy;
          nextSt->attack = 2;//二度目以降のアタックのサイン
          nextSt->attackingPiece->p.x = p2.x;
          nextSt->attackingPiece->p.y = p2.y;
          nextSt->attackingPiece->left = 0;;
          nextSt->attackingPiece->right = 0;
          
          if(checkPieceToAttack(nextSt, p2)){
            nextSt->pieceToMove = st->attackingPiece;
            minmax(nextSt, depth);//二度目の攻撃
          }else{
            nextSt->attack = 0;
            nextSt->player = opposit;
            minmax(nextSt, depth);
          }
          if(nextSt->value > max){
            max = nextSt->value;
            maxIndex = i;
            maxDirection = d;
          }
          if(nextSt->value < min){
            min = nextSt->value;
            minIndex = i;
            minDirection = d;
          }
        }
      }
    }
    if(st->player == A){
      st->value = max;
      st->valueIndex = maxIndex;
      st->valueDirection = maxDirection;
    }else if(st->player == B){
      st->value = min;
      st->valueIndex = minIndex;
      st->valueDirection = minDirection;
    }

  }else if(st->attack == 2){//二度目以降の攻撃の場合
    if(st->pieceToMove->left){
      copyState(nextSt, st);
      p->x = st->pieceToMove->p.x;
      p->y = st->pieceToMove->p.y;
      d = L;
      int dy = -1;
      nextSt->pieceToMove = st->pieceToMove;
      moveForMinMax(nextSt, p, d);
      /*さらにとれる場所が無いかチェック*/
      struct point p2;
      p2.x = p->x+dx+dx;
      p2.y = p->y+dy+dy;
      nextSt->attack = 2;//二度目以降のアタックのサイン
      nextSt->attackingPiece->p.x = p2.x;
      nextSt->attackingPiece->p.y = p2.y;
      nextSt->attackingPiece->left = 0;;
      nextSt->attackingPiece->right = 0;
      
      if(checkPieceToAttack(nextSt, p2)){
        nextSt->pieceToMove = st->attackingPiece;
        minmax(nextSt, depth);//二度目以降の攻撃
      }else{
        nextSt->attack = 0;
        nextSt->player = opposit;
        minmax(nextSt, depth);
      }
      if(nextSt->value > max){
        max = nextSt->value;
        maxIndex = i;
        maxDirection = d;
      }
      if(nextSt->value < min){
        min = nextSt->value;
        minIndex = i;
        minDirection = d;
      }
    }
    if(st->pieceToMove->right){
      copyState(nextSt, st);
      p->x = st->pieceToMove->p.x;
      p->y = st->pieceToMove->p.y;
      d = R;
      int dy = 1;
      nextSt->pieceToMove = st->pieceToMove;
      moveForMinMax(nextSt, p, d);
      /*さらにとれる場所が無いかチェック*/
      struct point p2;
      p2.x = p->x+dx+dx;
      p2.y = p->y+dy+dy;
      nextSt->attack = 2;//二度目以降のアタックのサイン
      nextSt->attackingPiece->p.x = p2.x;
      nextSt->attackingPiece->p.y = p2.y;
      nextSt->attackingPiece->left = 0;;
      nextSt->attackingPiece->right = 0;
      
      if(checkPieceToAttack(nextSt, p2)){
        nextSt->pieceToMove = st->attackingPiece;
        minmax(nextSt, depth);//二度目以降の攻撃
      }else{
        nextSt->attack = 0;
        nextSt->player = opposit;
        minmax(nextSt, depth);
      }
      if(nextSt->value > max){
        max = nextSt->value;
        maxIndex = i;
        maxDirection = d;
      }
      if(nextSt->value < min){
        min = nextSt->value;
        minIndex = i;
        minDirection = d;
      }
    }
    if(st->player == A){
      st->value = max;
      st->valueIndex = maxIndex;
      st->valueDirection = maxDirection;
    }else if(st->player == B){
      st->value = min;
      st->valueIndex = minIndex;
      st->valueDirection = minDirection;
    }
  }

  //テスト用プリンタ
  /* printBoard(st); */
  /* printf("st->value = %d\n", st->value); */
  /* printf("this value is supported by (%d, %d)\n", st->piece[st->valueIndex]->p.x, st->piece[st->valueIndex]->p.y); */
  /* if(st->pieceToMove != NULL) printf("this value is supported by (%d, %d)\n", st->pieceToMove->p.x, st->pieceToMove->p.y); */
  /* switch(st->valueDirection){ */
  /* case R: */
  /*   printf("by direction R\n"); */
  /*   break; */
  /* case L: */
  /*   printf("by direction L\n"); */
  /*   break; */
  /* default: */
  /*   break; */
  /* } */
  return 0;
}
