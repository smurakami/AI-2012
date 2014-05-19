#include <iostream>

/*---------------------------*
       P R O T A I N
 *---------------------------*/
typedef enum{
  P,
  H,
} PEPTID;

typedef struct{
  PEPTID * pep;
  int len;
} PROTAIN;

PROTAIN * allocProtain();
void initProtain(PROTAIN * prot);
void readProtain(PROTAIN * prot);

/*---------------------------*
             G A
 *---------------------------*/
typedef enum{
  FRL,
  FLR,
  RFL,
  RLF,
  LFR,
  LRF
} DNA;

typedef struct{
  DNA* dna;
  int len;
  int val;
} GENE;

/* 遺伝的アルゴリズム本体 */
GENE * GA(PROTAIN * prot);
GENE * allocGene(int len);
void setGeneAtRondom(GENE * gene);
void printGene(GENE * gene);

/* 世代交代 */
void swapGeneration(GENE * current[], GENE * next[]);

/* 選択 */
int selectByRoulette(GENE * current[]);
int selectBySQRoulette(GENE * current[]);
/* 交叉 */
void cross(GENE * prev1, GENE * prev2, GENE * next);
/* 複製 */
void copy(GENE * prev, GENE * next);
/* 突然変異 */
void mutate(GENE * prev, GENE * next);

/*---------------------------*
             M A P 
 *---------------------------*/
/*
 * 方向を以下のように定義する。
 * 
 *           0
 *           |
 *       3-- P --1  
 *           | 
 *           2
 * この定義のもと、MAP上のペプチド同士のつながりの方向を表現する。
 */
typedef struct{
  int on;//そこにペプチドがあるか
  PEPTID pep;//ペプチド種類
  int prev;//直前のペプチドの方向
  int next;//直後のペプチドの方向
  int HHC;//まわりにHH結合があるかどうか
} SQ;

/*
 * タンパク質の二次元構造を保存するマップ。
 */
typedef struct{
  SQ ** sq;
  int len;
} MAP;

MAP * allocMap(int protLen);
void initMap(MAP * map);
int setMap(MAP * map, PROTAIN * prot, GENE * gene);
int checkDirection(MAP * map, int x, int y, int dir);
int geneValue(MAP * map, PROTAIN * prot, GENE * gene);
void printMap(MAP * map);

/*---------------------------*
       U T I L I T Y
 *---------------------------*/
void die(const char * s);
void * safeMalloc(size_t size);
int isnum(int c);
int isparen(int c);

//色の変更
void redChar();
void defaultChar();
void blueBack();
void defaultBack();
