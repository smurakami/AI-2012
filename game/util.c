#include <stdio.h>
#include <stdlib.h>

void myError(char * s){
  perror(s);
  exit(1);
}

