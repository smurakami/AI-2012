#include "header.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(){
  srand(time(NULL));
  PROTAIN * prot = allocProtain();
  initProtain(prot);
  readProtain(prot);

  GA(prot);
  
  return 0;
}
