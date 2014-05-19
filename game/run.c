#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


pKind_t runGameH2H(state_t st){
  st->mode = H2H;
  runState_t rs;
  while(1){
    rs = Hturn(st, A);
    if(rs == finish) break;
    rs = Hturn(st, B);
    if(rs == finish) break;
  }
  return 0;
}

pKind_t runGameH2C(state_t st){//人vsCOM
  st->mode = H2C;
  runState_t rs;
  while(1){
    rs = Hturn(st, A);
    if(rs == finish) break;
    rs = Cturn(st, B);
    if(rs == finish) break;
  }
  estimate(st);
  if(st->value > 0) return A;
  else if(st->value < 0) return B;
  else return NONE;//引き分け
}
pKind_t runGameC2C(state_t st){//COMvsCOM
  return 0;
}

runState_t Hturn(state_t st, pKind_t player){//人のターン
  if((player==NONE)||(player > BK)) myError("undefined player\n");
  st->player = player;
  checkBoard(st);
  if(st->activePiece == 0) return finish;
  point_t p = allocPoint();
  runState_t rs = cont;
  movePieace(st, p);
  return rs;
}

runState_t Cturn(state_t st, pKind_t player){
  if((player==NONE)||(player > BK)) myError("undefined player\n");
  printBoard(st);
  printf("TURN B : CPU thinking\n");
  st->player = player;
  point_t p = allocPoint();
  direction_t d;
  int dx, dy;
    switch(player){
  case A:
    dx = -1;
    break;
  case B:
    dx = 1;
    break;
  default:
    return 1;
  }
  checkBoard(st);
  if(st->activePiece == 0) return finish;
  initPieceList(st);
  minmax(st, DEPTH);
  p->x = st->piece[st->valueIndex]->p.x;
  p->y = st->piece[st->valueIndex]->p.y;
  d = st->valueDirection;
  st->pieceToMove = allocPiece();
  st->pieceToMove->p.x = p->x;
  st->pieceToMove->p.y = p->y;
  if(d == L){
    st->pieceToMove->left = 1;
    st->pieceToMove->right = 0;
  }else if(d == R){
    st->pieceToMove->left = 0;
    st->pieceToMove->right = 1;
  }
    
  switch(d){
  case L:
    dy = -1;
    break;
  case R:
    dy = 1;
    break;
  default:
    return 1;
  }
  moveForMinMax(st, p, d);
  if(st->attack == 1){//多段攻撃の可能性を考える
    struct point pa;
    pa.x = p->x + dx + dx;
    pa.y = p->y + dy + dy;
    while(checkPieceToAttack(st, pa)){//多段攻撃が発生
      st->attack = 2;
      minmax(st, DEPTH);
      p->x = pa.x;
      p->y = pa.y;
      d = st->valueDirection;

      //st->pieceToMove = allocPiece();
      st->pieceToMove->p.x = p->x;
      st->pieceToMove->p.y = p->y;
      if(d == L){
        st->pieceToMove->left = 1;
        st->pieceToMove->right = 0;
      }else if(d == R){
        st->pieceToMove->left = 0;
        st->pieceToMove->right = 1;
      }
      
      moveForMinMax(st, p, d);
      switch(d){
      case L:
        dy = -1;
        break;
      case R:
        dy = 1;
        break;
      default:
        return 1;
      }
      pa.x = p->x + dx + dx;
      pa.y = p->y + dy + dy;
    }
  }
  
  return cont;
}


