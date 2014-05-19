#include "game.h"
#include <stdlib.h>
state_t allocState(){
  state_t st = (state_t)malloc(sizeof(struct state));
  if(st == NULL) myError("allocState failed\n");
  allocPieceList(st);
  st->attackingPiece = allocPiece();
  return st;
}

int initState(state_t st){
  int i, j;
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      st->bd[i][j] = NONE;
    }
  }
  initPieceList(st);
  st->player = 0;
  st->attack = 0;
  st->value = 0;
  st->pieceToMove = NULL;
  st->mode = H2H;
  st->winner = NONE;
  st->valueIndex = 0;
  st->valueDirection = 0;
  return 0;
}

int allocPieceList(state_t st){
  int i;
  for(i = 0; i < (SIZE*3)/2; i++){
    st->piece[i] = (piece_t)malloc(sizeof(struct piece));
    if(st->piece[i] == NULL) myError("malloc failed\n");
  }
  return 0;
}

int initPieceList(state_t st){
  int i;
  for(i = 0; i < (SIZE*3)/2; i++){
    st->piece[i]->p.x = 0;
    st->piece[i]->p.y = 0;
    st->piece[i]->enable = 0;
    st->piece[i]->left = 0;
    st->piece[i]->right = 0;
    st->piece[i]->leftSt = NULL;
    st->piece[i]->rightSt = NULL;
  }
  return 0;
}

int setBoard(state_t st){
  int i, j;
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      if((i + j) % 2){
        if(i < 3) st->bd[i][j] = B;
        if(i > SIZE - 4) st->bd[i][j] = A;
      }
    }
  }
  return 0;
}

point_t allocPoint(){
  point_t p = (point_t)malloc(sizeof(struct point));
  if(p == NULL) myError("malloc failed\n");
  return p;
}

piece_t allocPiece(){
  piece_t p = (piece_t)malloc(sizeof(struct piece));
  if(p == NULL) myError("malloc failed\n");
  
  return p;
}

int initPiece(piece_t p){
  if(p == NULL) return 1;
  p->enable = 0;
  p->p.x = 0;
  p->p.y = 0;
  p->left = 0;
  p->right = 0;
  p->leftSt = NULL;
  p->rightSt = NULL;
  return 0;
}

int copyPiece(piece_t p1, piece_t p0){
  if((p1 == NULL) || (p0 == NULL)) myError("cpyPiece: you choose NULL Piece\n");
  p1->enable = p0->enable;
  p1->p.x = p0->p.x;
  p1->p.y = p0->p.y;
  p1->left = p0->left;
  p1->right = p0->right;
  p1->leftSt = p0->leftSt;
  p1->rightSt = p0->rightSt;
  
  return 0; 
}

int copyState(state_t s1, state_t s0){
  if((s1 == NULL) || (s0 == NULL)) myError("cpyPiece: you choose NULL State\n");
  int i, j;
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      s1->bd[i][j] = s0->bd[i][j];
    }
  }
  for(i = 0; i < (SIZE/2)*3; i++){
    copyPiece(s1->piece[i], s0->piece[i]);
  }
  s1->player = s0->player;
  s1->attack = s0->attack;
  s1->value = s0->value;
  s1->pieceToMove = s0->pieceToMove;
  s1->mode = s0->mode;
  s1->winner = s0->winner;
  s1->valueIndex = s0->valueIndex;
  s1->valueDirection = s0->valueDirection;

  return 0;
}
