#include "header.h"

/************************************
          T Y P E D E F S
 ************************************/

typedef struct _ANT{
  int x, y, z;//position
  int BD[N][N];//chess borad
  int GD;//nomber of good Queen
  int tour[N][2];//the path this ant has taken
} ANT;

/************************************
    G L O B A L   V A R I A B L E S 
 ************************************/

double tau[N-1][N*N][N*N];//pheromone
double tau0[N*N];//pheromone of first edge
double alpha = 1.5;
double beta = 1.5;
double rho = 0.9;
ANT ** swarm;//Array of Ants
int swarmSize = DEFAULT_SWARM_SIZE;//swarm size;
double L = (double)swarmSize;
double defaultTau = 0.1;
int isFinished;
int isAlloced;
int parameterAdjustmentMode;

/************************************
         F U N C T I O N S
************************************/

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

/*----     ROULETTE     ----*/

double eta(ANT * ant, int i){
  int NG = checkChessBoard(ant->BD, i / N, i % N);
  
  return 1.0 / (NG + 1);
}

int roulette(ANT * ant){
  int nextNode;
  int S[N*N];//next nodes this ant can choose
  int sizeOfS;//sizeof S
  double sum;
  double *currentTau = tau[ant->x][ant->y*N + ant->z];
  double ball;//roulette ball
  sizeOfS = 0;
  for(int i = 0; i < N*N; i++){
    if(!ant->BD[i / N][i % N]){
      S[sizeOfS] = i;
      sizeOfS++;
    }
  }

  sum = 0;
  for(int i = 0; i < sizeOfS; i++){
    sum += pow(currentTau[S[i]] , alpha) + pow(eta(ant, S[i]) , beta);
  }

  ball = sum * Rand01();

  sum = 0;
  for(int i = 0; i < sizeOfS; i++){
    sum += pow(currentTau[S[i]] , alpha) + pow(eta(ant, S[i]) , beta);
    if(sum > ball) {
      nextNode = S[i];
      break;
    }
  }
  
  return nextNode;
}


int firstRoulette(){
  int nextNode;
  int S[N*N];//next nodes this ant can choose
  int sizeOfS;//sizeof S
  double sum;
  double * currentTau = tau0;
  double ball;//roulette ball
  sizeOfS = N*N;
  for(int i = 0; i < N*N; i++){
      S[i] = i;
  }

  sum = 0;
  for(int i = 0; i < sizeOfS; i++){
    sum += pow(currentTau[S[i]] , alpha) + 1;
  }

  ball = sum * Rand01();

  sum = 0;
  for(int i = 0; i < sizeOfS; i++){
    sum += pow(currentTau[S[i]] , alpha) + 1;
    if(sum > ball) {
      nextNode = S[i];
      break;
    }
  }

  return nextNode;
}

/*----     INITIALIZATION     ----*/
void initTau(){
  for(int k = 0; k < N-1; k++){
    for(int i = 0; i < N*N; i++){
      for(int j = 0; j < N*N; j++){
        tau[k][i][j] = defaultTau;
      }
    }
  }
  for(int i = 0; i < N*N; i++){
    tau0[i] = defaultTau;
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
    swarm[i]->x = swarm[i]->y = swarm[i]->z = 0;
    for(int j = 0; j < N; j++){
      for(int k = 0; k < N; k++){
        swarm[i]->BD[j][k] = 0;
      }
    }
    swarm[i]->GD = 0;
    for(int j = 0; j < N; j++){
      swarm[i]->tour[j][0] = 0;
      swarm[i]->tour[j][1] = 0;      
    }
  }
}

void linkGlovalVariable(){
  /* グローバル変数のリンク */
  linkTau(tau, tau0);
  linkSwarmSize();
  linkIsFinished(&isFinished);
  linkParameterAdjustmentMode(&parameterAdjustmentMode);
}

void initACO(){
  initTau();
  if(!isAlloced) allocAnts();
  initAnts();
  printf("initialized\n");
}

/*----       SIMULATION       ----*/

void simulateTour(int currentAnt){
  int nextNode;
  int prevNode;
  ANT * ant = swarm[currentAnt];
  nextNode = firstRoulette();
  ant->x = 0;
  ant->y = 0;//nextNode / N;
  ant->z = 0;//nextNode % N;
  ant->BD[ant->y][ant->z] = 1;
  ant->tour[0][0] = 0;
  ant->tour[0][1] = nextNode;

  while(ant->x < N-1){
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

void putPheromone(int currentAnt){
  ANT * ant = swarm[currentAnt];
  double PH = ant->GD;

  PH *= (N*N);
  PH /= swarmSize;

  tau0[ant->tour[0][1]] += PH;
  for(int i = 1; i < N; i++){
    tau[i-1][ant->tour[i][0]][ant->tour[i][1]] += PH;
  }
  
}

void simulateAllPheromone(){
  for(int i = 0; i < N*N; i++){
    tau0[i] *= rho;
  }

  for(int i = 0; i < N-1; i++){
    for(int j = 0; j < N*N; j++){
      for(int k = 0; k < N*N; k++){
        tau[i][j][k] *= rho;
      }
    }
  }
  
  for(int i = 0; i < swarmSize; i++){
    putPheromone(i);
  }
};

int finateIteration(){
  ANT * ant;
  int max = 0;

  if(parameterAdjustmentMode){
    for(int a = 0; a < swarmSize; a++){
      ant = swarm[a];
      if(max < ant->GD) max = ant->GD;
    }
    return max;
  }
  
  for(int a = 0; a < swarmSize; a++){
    ant = swarm[a];  
    if(max < ant->GD) max = ant->GD;
    if(ant->GD == 8){
      printf("---found a solution---\n");
      printf("\n");
      for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
          if(ant->BD[i][j]) printf(" Q");
          else printf(" -");
        }
        printf("\n");
      }
      isFinished = 1;
    }
    
  }
  printf("\n");
  
  printf("max Queen : %d\n", max);
  return max;
}

int simulateACO(){
  static int iteration;
  int max;
  initAnts();

  simulateAllTour();
  simulateAllPheromone();

  max = finateIteration();
  if(!parameterAdjustmentMode) printf("interation = %d\n", iteration);
  iteration++;
  return max;
}

void parameterAdjustment(){
  parameterAdjustmentMode = 1;

  double average;
  double max;
  double maxAlpha;
  double maxBeta;
  double maxRho;
  const int time = 1;

  double result[5][10][10];

  max = 0;
  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 10; j++){
      for(int k = 0; k < 10; k++){
        initACO();
        alpha = 1.0 + 0.1 * j;
        beta = 1.0 + 0.1 * k;
        rho = 1.0 - pow(0.1, i + 1);
        average = 0;
        for(int l = 0; l < time; l++){
          for(int m = 0; m < 200; m++) {
            simulateACO();
            //printf("time : %d, iteration : %4d\n", l, m);
          }
          average += (double)simulateACO();
        }
        average /= (double)time;

        result[i][j][k] = average;
        
        if(average > max){
          max = average;
          maxAlpha = alpha;
          maxBeta = beta;
          maxRho = rho;
        }
        printf("alpha = %f\n", alpha);
        printf("beta  = %f\n", beta);
        printf("rho   = %f\n", rho);
        printf("average max Queen = %f\n", average);
        printf("\n");
      }
    }
  }


  printf("**************\n");
  printf("*  finished  *\n");
  printf("**************\n");
  printf("\n");
  printf("\n");
  printf("I will show the best parameter...\n");
  printf("alpha = %f\n", maxAlpha);
  printf("beta = %f\n", maxBeta);
  printf("rho = %f\n", maxRho);
  printf("average max Queen = %f\n", max);
  printf("\n");

  FILE * fp = fopen("result.dat", "w");

  for(int i; i <  5; i++){
    for(int j; j < 10; j++){
      for(int k; k < 10; k++){
        fprintf(fp, "%d\t%d\t%d\t%f\n", i, j, k, max);
      }
    }
  }
  
  fclose(fp);
  
  parameterAdjustmentMode = 0;
}
