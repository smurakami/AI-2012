#include "header.h"
#include <stdio.h>
#include <stdlib.h>

#define G_SIZE 100//世代のサイズ
#define MAX_GEN 100//何世代シミュレートするか
#define P_CROSS 850//交叉のおきる確率*1000
#define P_MUTATE 10//突然変異の起きる確率*1000
#define P_COPY 1000 - P_MUTATE - P_CROSS//複製の起きる確率*1000

GENE * allocGene(int len){
  GENE * gene = (GENE *)safeMalloc(sizeof(GENE));
  gene->dna = (DNA *)safeMalloc(sizeof(GENE) * len);
  gene->len = len;
  gene->val = 0;
  return gene;
}

void initGeneVal(GENE * gene){
  gene->val = 0;
}

void setGeneAtRondom(GENE * gene){
  for(int i = 0; i < gene->len; i++){
    switch(rand()%6){
    case 0:
      gene->dna[i] = FRL;
      break;
    case 1:
      gene->dna[i] = FLR;
      break;
    case 2:
      gene->dna[i] = RFL;
      break;
    case 3:
      gene->dna[i] = RLF;
      break;
    case 4:
      gene->dna[i] = LFR;
      break;
    case 5:
      gene->dna[i] = LRF;
      break;
    }
  }
  gene->val = 0;
}

void printGene(GENE * gene){
  for(int i = 0; i < gene->len; i++){
    switch(gene->dna[i]){
    case FRL:
      printf("Frl ");
      break;
    case FLR:
      printf("Flr ");
      break;
    case RFL:
      printf("Rfl ");
      break;
    case RLF:
      printf("Rlf ");
      break;
    case LFR:
      printf("Lfr ");
      break;
    case LRF:
      printf("Lrf ");
      break;
    }
  }
  printf("\n");
}

//評価値に比例した確率で選択する
int selectByRoulette(GENE * current[G_SIZE]){
  int valSum = 0;
  for(int i = 0; i < G_SIZE; i++){
    valSum += current[i]->val;
  }
  //printf("val sum = %d\n", valSum);
  int roulette = rand() % valSum;
  int sum = 0;
  for(int i = 0; i < G_SIZE; i++){
    sum += current[i]->val;
    if(sum > roulette){
      //printf("%d selected, val = %d\n", i, current[i]->val);
      return i;
    }
  }

  die("roulette failed\n");
  return -1;
}

//評価値の平方に比例した確率で選択する
int selectBySQRoulette(GENE * current[G_SIZE]){
  int valSum = 0;
  for(int i = 0; i < G_SIZE; i++){
    valSum += current[i]->val * current[i]->val;
  }
  //printf("val sum = %d\n", valSum);
  int roulette = rand() % valSum;
  int sum = 0;
  for(int i = 0; i < G_SIZE; i++){
    sum += current[i]->val * current[i]->val;
    if(sum > roulette){
      //printf("%d selected, val = %d\n", i, current[i]->val);
      return i;
    }
  }

  die("roulette failed\n");
  return -1;
}

/* 交叉 */
void cross(GENE * prev1, GENE * prev2, GENE * next){
  int start = rand() % prev1->len;
  int end = rand() % prev1->len;

  if(start > end){
    int buf = start;
    start = end;
    end = buf;
  }

  for(int i = 0; i < start; i++){
    next->dna[i] = prev1->dna[i];
  }

  for(int i = start; i < end; i++){
    next->dna[i] = prev2->dna[i];
  }

  for(int i = end; i < prev1->len; i++){
    next->dna[i] = prev1->dna[i];
  }
  next->len = prev1->len;
  next->val = prev1->val;
}
/* 複製 */
void copy(GENE * prev, GENE * next){
  for(int i = 0; i < prev->len; i++){
    next->dna[i] = prev->dna[i];
  }
  next->len = prev->len;
  next->val = prev->val;
}
/* 突然変異 */
void mutate(GENE * prev, GENE * next){
  int roulette = rand() % prev->len;
  for(int i = 0; i < prev->len; i++){
    if(i == roulette){
      do{
        switch(rand() % 6){
        case 0:
          next->dna[i] = FRL;
          break;
        case 1:
          next->dna[i] = FLR;
          break;
        case 2:
          next->dna[i] = RFL;
          break;
        case 3:
          next->dna[i] = RLF;
          break;
        case 4:
          next->dna[i] = LFR;
          break;
        case 5:
          next->dna[i] = LRF;
          break;
        default:
          die("failed mutation\n");
          break;
        }
      }while(prev->dna[i] == next->dna[i]);
    }else{
      next->dna[i] = prev->dna[i];
    }
  }
  next->len = prev->len;
  next->val = prev->val;
}

/* 世代交代 */
void swapGeneration(GENE * current[], GENE * next[]){
  for(int i = 0; i < G_SIZE; i++){
    GENE * tmp = current[i];
    current[i] = next[i];
    next[i] = tmp;
  }
}

GENE * GA(PROTAIN * prot){
  MAP * map = allocMap(prot->len);
  GENE * current[G_SIZE];
  GENE * next[G_SIZE];
  for(int i = 0; i < G_SIZE; i++){
    current[i] = allocGene(prot->len);
    next[i] = allocGene(prot->len);
    setGeneAtRondom(current[i]);
    //printGene(current[i]);
  }
  int counter_cross = 0;
  int counter_copy = 0;
  int counter_mutate = 0;
  
  for(int generation = 0; generation < MAX_GEN; generation++){
//    if((generation == 0) || (generation == MAX_GEN - 1)){
          
    printf("**********************************************\n");
    printf("**********************************************\n");
    printf("****             generation %3d           ****\n", generation);
    printf("**********************************************\n");
    printf("**********************************************\n");
//    }
//各遺伝子の評価値の計算
    int max = -1;
    int maxIndex = -1;
    for(int i = 0; i < MAX_GEN; i++){
      initGeneVal(current[i]);
      geneValue(map, prot, current[i]);
      if(max < current[i]->val){
        max = current[i]->val;
        maxIndex = i;
      }
      //printf("%d : val = %d\n", i, current[i]->val);
    }
    geneValue(map, prot, current[maxIndex]);
//    if((generation == 0) || (generation == MAX_GEN - 1)){
      printMap(map);
      printf("max val is %d (index : %d)\n", current[maxIndex]->val, maxIndex);
//    }

    /*
     *   select and generate
     */

    int counter = 0;
    while(counter < G_SIZE){
      int roulette = rand() % 1000;
      if(roulette < P_CROSS){//crossing
        int index1 = selectByRoulette(current);
        int index2;
        do{
          index2 = selectByRoulette(current);
        }while(index1 == index2);
        cross(current[index1], current[index2], next[counter]);
        counter_cross++;
      }else if(roulette < P_CROSS + P_COPY){//copying
        int index = selectByRoulette(current);
        copy(current[index], next[counter]);
        counter_copy++;
      }else{//mutation
        int index = selectByRoulette(current);
        mutate(current[index], next[counter]);
        counter_mutate++;
      }
      counter++;
    }

    swapGeneration(current, next);
  }
  printf("crossing %d times occurred\n", counter_cross);
  printf("copying  %d times occurred\n", counter_copy);
  printf("mutation %d times occurred\n", counter_mutate);  
  
  return NULL;
}
