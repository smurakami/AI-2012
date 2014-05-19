#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

PROTAIN * allocProtain(){
  PROTAIN * prot = (PROTAIN *)safeMalloc(sizeof(PROTAIN));
  return prot;
}
void initProtain(PROTAIN * prot){
  prot->pep = NULL;
  prot->len = 0;
}

void readProtain(PROTAIN * prot){
  int len;
  int c;
  int depth;
  int index;
  int indexStuck[70];

  /* 長さを計る */
  while(!isnum(c = getchar()));
  len = 0;
  len = c - '0';
  while(isnum(c = getchar())){
    len *= 10;
    len += c - '0';
  }

  /* タンパク質配列を確保 */
  prot->pep = (PEPTID *)calloc(sizeof(PEPTID), len);
  prot->len = len;

  /* タンパク質を読む */
  depth = 0;//スタックの管理
  index = 0;
  while((c != EOF) && (c != '\n')){
    /* 空白をとばす */
    if(!isalnum(c) && !isparen(c)){
      c = getchar();
      continue;
    }
    if(isalpha(c)){
      if(c == 'H') prot->pep[index] = H;
      else if(c == 'P') prot->pep[index] = P;
      index++;
      c = getchar();
      continue;
    }
    
    if(c == '('){
      indexStuck[depth] = index;
      depth++;
      c = getchar();
      continue;
    }

    if(c == ')'){
      /* 数字の読み込み */
      int num = 0;
      while(isnum(c = getchar())){
        num *= 10;
        num += c - '0';
      }      
      depth--;
      int width = index - indexStuck[depth];
      for(int repeat = 1; repeat < num; repeat++){
        for(int i = indexStuck[depth]; i < index; i++){
          prot->pep[i + width * repeat] = prot->pep[i];
        }
      }
      index = index + width * (num - 1);
      continue;
    }

    if(isnum(c)){
      int num = 0;
      while(isnum(c)){
        num *= 10;
        num += c - '0';
        c = getchar();
      }
      for(int i = 0; i < num - 1; i++){
        prot->pep[index + i] = prot->pep[index - 1];
      }
      index = index + num - 1;
      continue;
    }
  }

  for(int i = 0; i < len; i++){
    switch(prot->pep[i]){
    case H:
      printf("H");
      break;
    case P:
      printf("P");
      break;
    default:
      printf("X");
      break;
    }
    if(i%4 == 3) printf(" ");
  }
    
  printf("\n");
  
  while((c = getchar()) == ' '){}
  /*get prot*/
  
  
}
