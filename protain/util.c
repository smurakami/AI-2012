#include <stdio.h>
#include <stdlib.h>

void die(const char * s){
  perror(s);
  exit(1);
}

void * safeMalloc(size_t size){
  void * pointer = malloc(size);
  if(pointer == NULL) die("malloc error\n");
  return pointer;
}

int isnum(int c){
  if((c >= '0') && (c <= '9')) return 1;
  else return 0;
}

int isparen(int c){
  if((c == '(') || (c == ')')) return 1;
  else return 0;
}

void redChar(){
  printf("\033[31m");//赤の文字に変更
}

void defaultChar(){
  printf("\033[39m");//通常の文字に変更
}

void blueBack(){
  printf("\033[44m");//青の背景に変更
}

void defaultBack(){
  printf("\033[49m");//通常の背景に変更
}

