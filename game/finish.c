#include "game.h"
#include <stdlib.h>

int freeState(state_t st){
  if(st == NULL) return 0;
  free(st);
  return 0;
}
