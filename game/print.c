#include "game.h"
#include <stdio.h>

int printBoard(state_t st){
  int i, j;//counter
  printf("*****PRINT BOARD*****\n");
  printf("\n");
  printf("  ");
  for(i = 0; i < SIZE; i++){
    printf("%2d", i);
  }
  printf(" Y\n");
  for(i = 0; i < SIZE; i++){
    printf("%2d", i);
    for(j = 0; j < SIZE; j++){
      switch(st->bd[i][j]){
      case NONE:
        printf(" -");
        break;
      case A:
        printf(" a");
        break;
      case AK:
        printf(" A");
        break;
      case B:
        printf(" b");
        break;
      case BK:
        printf(" B");
        break;
      default:
        printf(" ?");
        break;
      }
    }
  printf("\n");
  }
  printf(" X\n");
  estimate(st);
  printf("SCORE : %d\n", st->value);
  return 0;
}
