#include "GAI.h"

/**********************
        print.c
**********************/

/*人工頭脳のマップの表示
  その神経が興奮していたら'*'
  そうでなければ'-'をプリント
*/

int printAI(myAI* AI){
  int i, j;//counter
  unsigned char * p;
//  printf("counter : %d\n", AI->counter);
  //IplImage * output = AI->output;
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      p = (unsigned char *)(AI->output->imageData + AI->output->widthStep * i + AI->output->nChannels * j);
      if(AI->neu[i][j].state & b0000_0001){//もしも興奮していたら
      p[0] = 0;
      p[1] = 0;
      p[2] = 255;
      }else if(AI->neu[i][j].state & RETINA){//網膜と繋がっていたら
      p[0] = 100;
      p[1] = 255;
      p[2] = 0;
      }else{
      p[0] = 0;
      p[1] = 0;
      p[2] = 0;
      }
    }
  }

  point * current = AI->dest;//受容野をオレンジで表示
  while(current->next != NULL){
    current = current->next;
          p = (unsigned char *)(AI->output->imageData + AI->output->widthStep * current->x + AI->output->nChannels * current->y);
          p[0] = 0;
          p[1] = 100;
          p[2] = 200;
  }

  current = AI->retina;//網膜を青で表示
  while(current->next != NULL){
    current = current->next;
          p = (unsigned char *)(AI->output->imageData + AI->output->widthStep * current->x + AI->output->nChannels * current->y);
          p[0] = 255;
          p[1] = 0;
          p[2] = 0;
  }

  cvShowImage("output", AI->output);
  //AI->counter++;

  return 0;
}

int printAIforGA(myAI * AI){
  int i, j;//counter
  unsigned char * p;
//  printf("counter : %d\n", AI->counter);
  //IplImage * output = AI->output;
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      p = (unsigned char *)(AI->output->imageData + AI->output->widthStep * i + AI->output->nChannels * j);
      if(AI->neu[i][j].state & RETINA){//もしも網膜と繋がっていたら
      p[0] = 100;
      p[1] = 255;
      p[2] = 0;
      }else{
      p[0] = 0;
      p[1] = 0;
      p[2] = 0;
      }
    }
  }

  point * current = AI->dest;//受容野をオレンジで表示
  while(current->next != NULL){
    current = current->next;
          p = (unsigned char *)(AI->output->imageData + AI->output->widthStep * current->x + AI->output->nChannels * current->y);
          p[0] = 0;
          p[1] = 100;
          p[2] = 200;
  }

  current = AI->retina;//網膜を青で表示
  while(current->next != NULL){
    current = current->next;
          p = (unsigned char *)(AI->output->imageData + AI->output->widthStep * current->x + AI->output->nChannels * current->y);
          p[0] = 255;
          p[1] = 0;
          p[2] = 0;
  }

  
  cvShowImage("output", AI->output);
  //AI->counter++;

  return 0;
}
/*昔のprintAI*/
/* int printAI(myAI* AI){ */
/*   int i, j;//counter */
/*   for(i = 0; i < SIZE - 3; i++) putchar('*'); */
/*   printf(" myAI "); */
/*   for(i = 0; i < SIZE - 3; i++) putchar('*'); */

/*   printf("\n"); */
/*   printf("\n"); */

/*   for(i = 0; i < SIZE; i++){ */
/*     for(j = 0; j < SIZE; j++){ */
/*       if(AI->neu[i][j].state & b0000_0001){//もしも興奮していたら */
/*         printf(" *"); */
/*       }else{ */
/*         printf("  "); */
/*       } */
/*     } */
/*     printf("\n"); */
/*   } */
/*   return 0; */
/* } */

/*画面を一掃する*/
int clearScreen(){
  printf("\033[2J");
  return 0;
}

