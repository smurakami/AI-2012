#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define N 8
#define DEFAULT_SWARM_SIZE 30 //default swarm size

/************************************
          T Y P E D E F S
 ************************************/

typedef struct _WAY{
  int num;
  double * tau;
  struct _WAY ** next;
} WAY;

typedef struct _ANT{
  int x, y;//position
  int BD[N][N];//chess borad
  int column[N];
  int GD;//nomber of good Queen
  int tour[N];//the path this ant has taken
  WAY * way;
} ANT;

/************************************
    G L O B A L   V A R I A B L E S 
 ************************************/

int swarmSize = DEFAULT_SWARM_SIZE;//swarm size;
ANT ** swarm;//Array of Ants
WAY * headWay;

double defaultTau = 0.1;
double alpha = 1.5;
double beta = 1.5;
double rho = 0.9;

int isAlloced;

/************************************
         F U N C T I O N S
************************************/

/*----- U T I L I T Y -----*/

void * safeMalloc(size_t size){
  void * p = malloc(size);
  if(p == NULL){
    perror("malloc\n");
    exit(1);
  }
  return p;
}



/*----       CHESS BOARD      ----*/
int checkChessBoard(int (*BD)[N], int x, int y){
  int i, j;//counter
  int NG;//nogood Queen
  NG = 0;
  for(i = 0; i < N; i++){
    if(BD[x][i] && (i != y)) NG++;
  }

  for(i = 0; i < N; i++){
    if(BD[i][y] && (i != x)) NG++;
  }

  i = x;
  j = y;
  while((i > 0) && (j > 0)){
    i--;
    j--;
  }
  while((i < N) && (j < N)){
    if(BD[i][j] && (x != i) && (y != j)) NG++; 
    i++;
    j++;
  }

  i = x;
  j = y;
  while((i < N-1) && (j > 0)){
    i++;
    j--;
  }
  while((i >= 0) && (j < N)){
    if(BD[i][j] && (x != i) && (y != j)) NG++; 
    i--;
    j++;
  }
  
  return NG;
}

/*----         RANDOMS        ----*/

int RandomEqualINT(int Low, int High)
{
  return rand() % (High-Low+1) + Low; 
}

#define RMAX 32767 /* 2^15-1 */
double Rand01()
{
  long int r;
  
  r = rand();
  return ((double)(r % RMAX)/(RMAX-1));

}

/*----     INITIALIZATION     ----*/
WAY * allocWay(int num){
  if(num == N) return NULL;
  WAY * way = (WAY *)malloc(sizeof(WAY));
  if(way == NULL) {perror("alloc Way\n"); exit (1);}
  way->num = num;
  way->tau = (double *)safeMalloc(sizeof(double) * (N - num));
  for(int i = 0; i < (N - num); i++){
    way->tau[i] = 0.0;
  }
  way->next = (WAY **)safeMalloc(sizeof(WAY*) * (N - num));
  for(int i = 0; i < (N - num); i++){
    way->next[i] = allocWay(num + 1);
  }
  return way;
}

void freeWay(WAY * way){
  if(way == NULL) return;
  int num = way->num;
  for(int i = 0; i < N - num; i++){
    freeWay(way->next[i]);
  }
  free(way);
}

void initWay(WAY * way){
  int num = way->num;
  for(int i = 0; i < N - num; i++){
    way->tau[i] = defaultTau;
  }
  for(int i = 0; i < N - num; i++){
    initWay(way->next[i]);
  } 
}

void allocAnts(){
  swarm = (ANT **)malloc(sizeof(ANT*)*swarmSize);
  for(int i = 0; i < swarmSize; i++){
    swarm[i] = (ANT *)malloc(sizeof(ANT));
  }
  printf("Alloc Ants\n");
  isAlloced = 1;
}

void initAnts(){
  for(int i = 0; i < swarmSize; i++){
    swarm[i]->x = swarm[i]->y = 0;
    for(int j = 0; j < N; j++){
      for(int k = 0; k < N; k++){
        swarm[i]->BD[j][k] = 0;
      }
    }
    swarm[i]->GD = 0;
    for(int j = 0; j < N; j++){
      swarm[i]->tour[j] = 0;
      swarm[i]->column[j] = 0;
    }
  }
}

void initACO(WAY * way){
  initWay(way);
  if(!isAlloced) allocAnts();
  initAnts();
  printf("initialized\n");
}

/*----     ROULETTE     ----*/
double eta(ANT * ant, int x, int y){
  int NG = checkChessBoard(ant->BD, x, y);
  
  return 1.0 / (NG + 1);
}

int getColumn(ANT * ant, int num){
  int column;
  for(int i = 0; i < N; i++){
    if(ant->column[i] == 0){
      column = i;
      break;
    }
  }
  while(num > 0){
    column++;
    for(int i = column; i < N; i++){
      if(anto->column[i] == 0){
        column = i;
        break;
      }
    }
    num--;
  }
  return column;
}

int roulette(ANT * ant){
  WAY * way = ant->way;
  int nextNode;
  int num = ant->way->num;
  int S[N - num];
  int sizeOfS = N - num;
  double sum;

  for(int i = 0; i < sizeOfS; i++){
    int column = getColumn(ant, i);
    sum += pow(way->tau[i], alpha) * pow(eta(ant, num + 1, column), beta);
  }

  double ball = sum * Rand01();

  sum = 0;
  for(itn i = 0; i < sizeOfS; i++){
    sum += pow(way->tau[i], alpha) * pow(eta(ant, num + 1, column), beta);
    if(sum > ball){
      nextNode = i;
      break;
    }
  }
  return nextNode;
}
/*----       SIMULATION       ----*/

void simulateTour(int currentAnt){
  int nextNode;
  int prevNode;
  ANT * ant = swarm[currentAnt];
  ant->way = headWay;
  nextNode = roulette(ant);
  ant->BD[0][getColumn(ant, nextNode)] = 1;
  ant->column[getColumn(ant, nextNode)] = 1;
  ant->tour[0] = nextNode;
  ant->way = ant->way->next[nextNode];

  while(ant->way){
    prevNode = nextNode;
    nextNode = roulette(ant);
    ant->x++;
    ant->y = nextNode / N;
    ant->z = nextNode % N;
    ant->BD[ant->y][ant->z] = 1;
    ant->tour[ant->x][0] = prevNode;
    ant->tour[ant->x][1] = nextNode;
  }

  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++){
      if((ant->BD[i][j]) && (!checkChessBoard(ant->BD, i, j)))
        ant->GD++;
    }
  }

  //printf("ant%2d : (%d, %d, %d) GD : %d\n", currentAnt, ant->x, ant->y, ant->z, ant->GD);
  
}


void simulateAllTour(){
  initAnts();
  for(int i = 0; i < swarmSize; i++){
    simulateTour(i);
  }
}

int simulateACO(){
  static int iteration;
  int max;
  initAnts();
  simulateAllTour();
  


}

int ACO(){
  while(1){
    for(int i = 0; i < 100; i++){
      printf("%d\n", i);
      if(simulateACO() == 8) break;
    }
    int c = getchar();
    if(c == 'q') break;
  }
}

int main(){
  srand(123);
  headWay = allocWay(0);
  ACO();
  
  freeWay(headWay);
  return 0;
}
