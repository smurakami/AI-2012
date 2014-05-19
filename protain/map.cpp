#include "header.h"
#include <stdio.h>

MAP * allocMap(int protLen){
  MAP * map = (MAP *)safeMalloc(sizeof(MAP));
  map->sq = (SQ**)safeMalloc(sizeof(SQ*) * 2 * protLen);
  for(int i = 0; i < 2 * protLen; i++){
    map->sq[i] = (SQ *)safeMalloc(sizeof(SQ) * 2 * protLen);
  }
  map->len = protLen * 2;
  return map;
}
void initMap(MAP * map){
  for(int i = 0; i < map->len; i++){
    for(int j = 0; j < map->len; j++){
      map->sq[i][j].on = 0;
      map->sq[i][j].HHC = 0;
    }
  }
}

int setMap(MAP * map, PROTAIN * prot, GENE * gene){
  /* タンパク質をマップの中心から描き始める */
  int x = map->len / 2;
  int y = map->len / 2;
  map->sq[x][y].prev = 2;
  int dir1, dir2, dir3;
  int prev, next;
  for(int i = 0; i < gene->len; i++){
    map->sq[x][y].on = 1;
    map->sq[x][y].pep = prot->pep[i];
    if(i == gene->len - 1){
      map->sq[x][y].next = -1;
      break;} 
    prev = map->sq[x][y].prev;
    switch(gene->dna[i]){
    case FRL:
      dir1 = (prev+2)%4;
      dir2 = (prev+3)%4;
      dir3 = (prev+1)%4;
      break;
    case FLR:
      dir1 = (prev+2)%4;
      dir2 = (prev+1)%4;
      dir3 = (prev+3)%4;
      break;
    case RFL:
      dir1 = (prev+3)%4;
      dir2 = (prev+2)%4;
      dir3 = (prev+1)%4;
      break;
    case RLF:
      dir1 = (prev+3)%4;
      dir2 = (prev+1)%4;
      dir3 = (prev+2)%4;
      break;
    case LFR:
      dir1 = (prev+1)%4;
      dir2 = (prev+2)%4;
      dir3 = (prev+3)%4;
      break;
    case LRF:
      dir1 = (prev+1)%4;
      dir2 = (prev+3)%4;
      dir3 = (prev+2)%4;
      break;
    }
    next = dir1;
    if(checkDirection(map, x, y, next)){
      next = dir2;
      if(checkDirection(map, x, y, next)){
        next = dir3;
        if(checkDirection(map, x, y, next)){
          //行き止まり
          gene->val = -1;//致死性遺伝子の印
          return 0;
        }
      }
    }
    map->sq[x][y].next = next;
    switch(next){
    case 0:
      x--;
      break;
    case 1:
      y++;
      break;
    case 2:
      x++;
      break;
    case 3:
      y--;
      break;
    default:
      break;
    }
    map->sq[x][y].prev = (next + 2) % 4;
  }

  x = map->len / 2;
  y = map->len / 2;
  map->sq[x][y].prev = -1;
  return 1;
}

int checkDirection(MAP * map, int x, int y, int dir){
  switch(dir){
  case 0:
    return map->sq[x-1][y].on;
  case 1:
    return map->sq[x][y+1].on;
  case 2:
    return map->sq[x+1][y].on;
  case 3:
    return map->sq[x][y-1].on;
  default:
    return 0;
  } 
}

int geneValue(MAP * map, PROTAIN * prot, GENE * gene){
  initMap(map);
  setMap(map, prot, gene);
  /*致死性遺伝子は評価値0*/
  if(gene->val == -1){
    gene->val = 0;
    return gene->val;
  }
  
  int x = map->len / 2;
  int y = map->len / 2;
  int val = 0;
  for(int i = 0; i < prot->len; i++){
    if(map->sq[x][y].pep == H){
      for(int dir = 0; dir < 4; dir++){
        if((dir != map->sq[x][y].prev) && (dir != map->sq[x][y].next)){
          if(checkDirection(map, x, y, dir)){
            PEPTID neib;
            switch(dir){
            case 0:
              neib = map->sq[x-1][y].pep;
              break;
            case 1:
              neib = map->sq[x][y+1].pep;
              break;
            case 2:
              neib = map->sq[x+1][y].pep;
              break;
            case 3:
              neib = map->sq[x][y-1].pep;
              break;
            default:
              break;
            }
            if(neib == H){
              val++;
              map->sq[x][y].HHC = 1;
            }
          }
        }
      }
    }
    switch(map->sq[x][y].next){
    case 0:
      x--;
      break;
    case 1:
      y++;
      break;
    case 2:
      x++;
      break;
    case 3:
      y--;
      break;
    default:
      break;
    }
  }

  gene->val = val / 2;
  
  return gene->val;
}

void printMap(MAP * map){
  printf("*--------------------------*\n");
  printf("*      P R I N T M A P     *\n");
  printf("*--------------------------*\n");
  printf("\n");
  //タンパク質の存在する領域を調べる
  int min_x;
  int max_x;
  int min_y = map->len;
  int max_y = 0;
  int started = 0;
  for(int i = 0; i < map->len; i++){
    for(int j = 0; j < map->len; j++){
      if(map->sq[i][j].on){
        if(started == 0){
          min_x = i;
          started = 1;
        }
        max_x = i;
        if(min_y > j) min_y = j;
        if(max_y < j) max_y = j;
      }
    }
  }
  max_x++;
  max_y++;

  //print map
  printf("   ");
  for(int i = min_y; i < max_y; i++){
    printf("%3d", i);
  }
  printf("\n");
  for(int i = min_x; i < max_x; i++){
    printf("%3d ", i);
    for(int j = min_y; j < max_y; j++){
      if(map->sq[i][j].on){
        if((i == map->len/2)&&( j == map->len/2)) blueBack();
        switch(map->sq[i][j].pep){
        case H:
          if(map->sq[i][j].HHC) redChar();
          printf("H");
          if(map->sq[i][j].HHC) defaultChar();
          break;       
        case P:
          printf("P");
          break;
        default:
          break;
        }
        if((i == map->len/2)&&( j == map->len/2)) defaultBack();
      }else{
        printf(" ");
      }
      if((map->sq[i][j].on)&&((map->sq[i][j].next == 1) || (map->sq[i][j].prev == 1))){
        printf("--");
      }else{
        printf("  ");
      }
    }
    printf("\n");
    printf("    ");
    for(int j = min_y; j < max_y; j++){
      if((map->sq[i][j].on) && ((map->sq[i][j].next == 2) || (map->sq[i][j].prev == 2))){
        printf("|  ");
      }else{
        printf("   ");
      }
    }
    printf("\n");
  }
    
  printf("*--------------------------*\n");
  printf("*       E N D  M A P       *\n");
  printf("*--------------------------*\n");
  printf("\n");

}
