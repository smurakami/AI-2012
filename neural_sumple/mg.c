#include <stdlib.h>
#include <stdio.h>
#include <math.h>


typedef int           BOOL;
typedef int           INT;
typedef double        REAL;

#define FALSE         0
#define TRUE          1
#define NOT           !
#define AND           &&
#define OR            ||

#define MIN_REAL      -HUGE_VAL
#define MAX_REAL      +HUGE_VAL
#define MIN(x,y)      ((x)<(y) ? (x) : (y))
#define MAX(x,y)      ((x)>(y) ? (x) : (y))

#define LO            0.1
#define HI            0.9
#define BIAS          1

#define sqr(x)        ((x)*(x))


typedef struct {                     /* A LAYER OF A NET:                     */
        INT           Units;         /* - number of units in this layer       */
        REAL*         Output;        /* - output of ith unit                  */
        REAL*         Error;         /* - error term of ith unit              */
        REAL**        Weight;        /* - connection weights to ith unit      */
        REAL**        WeightSave;    /* - saved weights for stopped training  */
        REAL**        dWeight;       /* - last weight deltas for momentum     */
} LAYER;

typedef struct {                     /* A NET:                                */
        LAYER**       Layer;         /* - layers of this net                  */
        LAYER*        InputLayer;    /* - input layer                         */
        LAYER*        OutputLayer;   /* - output layer                        */
        REAL          Alpha;         /* - momentum factor                     */
        REAL          Eta;           /* - learning rate                       */
        REAL          Gain;          /* - gain of sigmoid function            */
        REAL          Error;         /* - total net error                     */
} NET;


/******************************************************************************
        R A N D O M S   D R A W N   F R O M   D I S T R I B U T I O N S
 ******************************************************************************/


void InitializeRandoms()
{
  srand(4711);
}


INT RandomEqualINT(INT Low, INT High)
{
  return rand() % (High-Low+1) + Low;
}      


#define RMAX 32767 /* 2^15-1 */
float Rand01()
{
    long int r;
    
    r=rand();
    return ((double)(r % RMAX)/(RMAX-1));

}

REAL RandomEqualREAL()
{
    return((Rand01()-0.5)*2);
}



/******************************************************************************
               A P P L I C A T I O N - S P E C I F I C   C O D E
 ******************************************************************************/


#define NUM_LAYERS    3
#define N             10
#define M             1
INT                   Units[NUM_LAYERS] = {N, 15, M};

#define FIRST_YEAR    1
#define NUM_YEARS     600

#define TRAIN_LWB     (N)
#define TRAIN_UPB     (100)
#define TRAIN_YEARS   (TRAIN_UPB - TRAIN_LWB + 1)
#define TEST_LWB      (101)
#define TEST_UPB      (500)
#define TEST_YEARS    (TEST_UPB - TEST_LWB + 1)
#define EVAL_LWB      (501)
#define EVAL_UPB      (NUM_YEARS - 1)
#define EVAL_YEARS    (EVAL_UPB - EVAL_LWB + 1)

REAL                  Sunspots_[NUM_YEARS];
REAL                  Sunspots [NUM_YEARS];

REAL                  Mean;
REAL                  TrainError;
REAL                  TrainErrorPredictingMean;
REAL                  TestError;
REAL                  TestErrorPredictingMean;

FILE*                 f;


void NormalizeSunspots()
{
  INT  Year;
  REAL Min, Max;
        
  Min = MAX_REAL;
  Max = MIN_REAL;
  for (Year=0; Year<NUM_YEARS; Year++) {
    Min = MIN(Min, Sunspots[Year]);
    Max = MAX(Max, Sunspots[Year]);
  }
  Mean = 0;
  for (Year=0; Year<NUM_YEARS; Year++) {
    Sunspots_[Year] = 
    Sunspots [Year] = ((Sunspots[Year]-Min) / (Max-Min)) * (HI-LO) + LO;
    Mean += Sunspots[Year] / NUM_YEARS;
  }
}

double power(x,n)
  double x;
  int n;
{
  int i;
  double p;
  
  p=1.0;
  for (i=1; i<=n; ++i)
    p = p*x;
  return(p);
}

#define NORM_MAX 1.324328
#define NORM_MIN 0.405343
#define F_MAX NUM_YEARS+100
#define START 50

void f_initialize_time ()
{
  int i, j, k, Year;
  double xn, xn_1, dxt,xttau;
  double a,b, mean, err;
  int tau;
  REAL F_VALUE[F_MAX];

/* M-G version */

/* 501 */ F_VALUE[0] = 0.401310;
/* 502 */ F_VALUE[1] = 0.953833;
/* 503 */ F_VALUE[2] = 0.174821;
/* 504 */ F_VALUE[3] = 0.572708;
/* 505 */ F_VALUE[4] = 0.971513;
/* 506 */ F_VALUE[5] = 0.109872;
/* 507 */ F_VALUE[6] = 0.388265;
/* 508 */ F_VALUE[7] = 0.942936;
/* 509 */ F_VALUE[8] = 0.213617;
/* 510 */ F_VALUE[9] = 0.666899;
/* 511 */ F_VALUE[10] = 0.881914;
/* 512 */ F_VALUE[11] = 0.413442;
/* 513 */ F_VALUE[12] = 0.962755;
/* 514 */ F_VALUE[13] = 0.142354;
/* 515 */ F_VALUE[14] = 0.484694;
/* 516 */ F_VALUE[15] = 0.991570;
/* 517 */ F_VALUE[16] = 0.033185;
/* 518 */ F_VALUE[17] = 0.127373;
/* 519 */ F_VALUE[18] = 0.441263;
/* 520 */ F_VALUE[19] = 0.978804;


  a = 0.2;
  b = 0.1;
  tau = 17;

  for (i=20; i<F_MAX; i++) {
    xttau = F_VALUE[i-tau];
    xn_1 = F_VALUE[i-1];
    dxt = (a * xttau / (1 + power(xttau,10))) - b * xn_1;
    F_VALUE[i] = xn_1 + dxt;
  }

  for (Year=0; Year<NUM_YEARS; Year++) {
    Sunspots[Year] = F_VALUE[Year+START];
  }
}



void InitializeApplication(NET* Net)
{
  INT  Year, i;
  REAL Out, Err;

  Net->Alpha = 0.5;
  Net->Eta   = 0.05;
  Net->Gain  = 1;

  f_initialize_time();

  NormalizeSunspots();
  TrainErrorPredictingMean = 0;
  for (Year=TRAIN_LWB; Year<=TRAIN_UPB; Year++) {
    for (i=0; i<M; i++) {
      Out = Sunspots[Year+i];
      Err = Mean - Out;
      TrainErrorPredictingMean += 0.5 * sqr(Err);
    }
  }
  TestErrorPredictingMean = 0;
  for (Year=TEST_LWB; Year<=TEST_UPB; Year++) {
    for (i=0; i<M; i++) {
      Out = Sunspots[Year+i];
      Err = Mean - Out;
      TestErrorPredictingMean += 0.5 * sqr(Err);
    }
  }
  f = fopen("BPN.txt", "w");
}


void FinalizeApplication(NET* Net)
{
  fclose(f);
}


/******************************************************************************
                          I N I T I A L I Z A T I O N
 ******************************************************************************/


void GenerateNetwork(NET* Net)
{
  INT l,i;

  Net->Layer = (LAYER**) calloc(NUM_LAYERS, sizeof(LAYER*));
   
  for (l=0; l<NUM_LAYERS; l++) {
    Net->Layer[l] = (LAYER*) malloc(sizeof(LAYER));
      
    Net->Layer[l]->Units      = Units[l];
    Net->Layer[l]->Output     = (REAL*)  calloc(Units[l]+1, sizeof(REAL));
    Net->Layer[l]->Error      = (REAL*)  calloc(Units[l]+1, sizeof(REAL));
    Net->Layer[l]->Weight     = (REAL**) calloc(Units[l]+1, sizeof(REAL*));
    Net->Layer[l]->WeightSave = (REAL**) calloc(Units[l]+1, sizeof(REAL*));
    Net->Layer[l]->dWeight    = (REAL**) calloc(Units[l]+1, sizeof(REAL*));
    Net->Layer[l]->Output[0]  = BIAS;
      
    if (l != 0) {
      for (i=1; i<=Units[l]; i++) {
        Net->Layer[l]->Weight[i]     = (REAL*) calloc(Units[l-1]+1, sizeof(REAL));
        Net->Layer[l]->WeightSave[i] = (REAL*) calloc(Units[l-1]+1, sizeof(REAL));
        Net->Layer[l]->dWeight[i]    = (REAL*) calloc(Units[l-1]+1, sizeof(REAL));
      }
    }
  }
  Net->InputLayer  = Net->Layer[0];
  Net->OutputLayer = Net->Layer[NUM_LAYERS - 1];
  Net->Alpha       = 0.9;
  Net->Eta         = 0.25;
  Net->Gain        = 1;
}


void RandomWeights(NET* Net)
{
  INT l,i,j;
   
  for (l=1; l<NUM_LAYERS; l++) {
    for (i=1; i<=Net->Layer[l]->Units; i++) {
      for (j=0; j<=Net->Layer[l-1]->Units; j++) {
        Net->Layer[l]->Weight[i][j] = RandomEqualREAL(-0.5, 0.5);
      }
    }
  }
}


void SetInput(NET* Net, REAL* Input)
{
  INT i;
   
  for (i=1; i<=Net->InputLayer->Units; i++) {
    Net->InputLayer->Output[i] = Input[i-1];
  }
}


void GetOutput(NET* Net, REAL* Output)
{
  INT i;
   
  for (i=1; i<=Net->OutputLayer->Units; i++) {
    Output[i-1] = Net->OutputLayer->Output[i];
  }
}


/******************************************************************************
            S U P P O R T   F O R   S T O P P E D   T R A I N I N G
 ******************************************************************************/


void SaveWeights(NET* Net)
{
  INT l,i,j;

  for (l=1; l<NUM_LAYERS; l++) {
    for (i=1; i<=Net->Layer[l]->Units; i++) {
      for (j=0; j<=Net->Layer[l-1]->Units; j++) {
        Net->Layer[l]->WeightSave[i][j] = Net->Layer[l]->Weight[i][j];
      }
    }
  }
}


void RestoreWeights(NET* Net)
{
  INT l,i,j;

  for (l=1; l<NUM_LAYERS; l++) {
    for (i=1; i<=Net->Layer[l]->Units; i++) {
      for (j=0; j<=Net->Layer[l-1]->Units; j++) {
        Net->Layer[l]->Weight[i][j] = Net->Layer[l]->WeightSave[i][j];
      }
    }
  }
}


/******************************************************************************
                     P R O P A G A T I N G   S I G N A L S
 ******************************************************************************/


void PropagateLayer(NET* Net, LAYER* Lower, LAYER* Upper)
{
  INT  i,j;
  REAL Sum;

  for (i=1; i<=Upper->Units; i++) {
    Sum = 0;
    for (j=0; j<=Lower->Units; j++) {
      Sum += Upper->Weight[i][j] * Lower->Output[j];
    }
    Upper->Output[i] = 1 / (1 + exp(-Net->Gain * Sum));
  }
}


void PropagateNet(NET* Net)
{
  INT l;
   
  for (l=0; l<NUM_LAYERS-1; l++) {
    PropagateLayer(Net, Net->Layer[l], Net->Layer[l+1]);
  }
}


/******************************************************************************
                  B A C K P R O P A G A T I N G   E R R O R S
 ******************************************************************************/


void ComputeOutputError(NET* Net, REAL* Target)
{
  INT  i;
  REAL Out, Err;
   
  Net->Error = 0;
  for (i=1; i<=Net->OutputLayer->Units; i++) {
    Out = Net->OutputLayer->Output[i];
    Err = Target[i-1]-Out;
    Net->OutputLayer->Error[i] = Net->Gain * Out * (1-Out) * Err;
    Net->Error += 0.5 * sqr(Err);
  }
}


void BackpropagateLayer(NET* Net, LAYER* Upper, LAYER* Lower)
{
  INT  i,j;
  REAL Out, Err;
   
  for (i=1; i<=Lower->Units; i++) {
    Out = Lower->Output[i];
    Err = 0;
    for (j=1; j<=Upper->Units; j++) {
      Err += Upper->Weight[j][i] * Upper->Error[j];
    }
    Lower->Error[i] = Net->Gain * Out * (1-Out) * Err;
  }
}


void BackpropagateNet(NET* Net)
{
  INT l;
   
  for (l=NUM_LAYERS-1; l>1; l--) {
    BackpropagateLayer(Net, Net->Layer[l], Net->Layer[l-1]);
  }
}


void AdjustWeights(NET* Net)
{
  INT  l,i,j;
  REAL Out, Err, dWeight;
   
  for (l=1; l<NUM_LAYERS; l++) {
    for (i=1; i<=Net->Layer[l]->Units; i++) {
      for (j=0; j<=Net->Layer[l-1]->Units; j++) {
        Out = Net->Layer[l-1]->Output[j];
        Err = Net->Layer[l]->Error[i];
        dWeight = Net->Layer[l]->dWeight[i][j];
        Net->Layer[l]->Weight[i][j] += Net->Eta * Err * Out + Net->Alpha * dWeight;
        Net->Layer[l]->dWeight[i][j] = Net->Eta * Err * Out;
      }
    }
  }
}


/******************************************************************************
                      S I M U L A T I N G   T H E   N E T
 ******************************************************************************/

/*
x(t) = f( x(t-31), x(t-21), x(t-16), x(t-11), x(t-6), 
       x(t-5), x(t-4), x(t-3), x(t-2), x(t-1) )
*/
void GetInput(REAL Ins[], REAL *now)
{
    Ins[0]=now[-31];
    Ins[1]=now[-21];
    Ins[2]=now[-16];
    Ins[3]=now[-11];
    Ins[4]=now[-6];
    Ins[5]=now[-5];
    Ins[6]=now[-4];
    Ins[7]=now[-3];
    Ins[8]=now[-2];
    Ins[9]=now[-1];

}

void SimulateNet(NET* Net, REAL* Input2, REAL* Output, REAL* Target, BOOL Training)
{
  REAL Input[N];
  GetInput(Input,Input2);

  SetInput(Net, Input);
  PropagateNet(Net);
  GetOutput(Net, Output);
   
  ComputeOutputError(Net, Target);
  if (Training) {
    BackpropagateNet(Net);
    AdjustWeights(Net);
  }
}

void TrainNet(NET* Net, INT Epochs)
{
  INT  Year, n;
  REAL Output[M];

  for (n=0; n<Epochs*TRAIN_YEARS; n++) {
    Year = RandomEqualINT(TRAIN_LWB, TRAIN_UPB);
    SimulateNet(Net, &(Sunspots[Year-N]), Output, &(Sunspots[Year]), TRUE);
  }
}


void TestNet(NET* Net)
{
  INT  Year;
  REAL Output[M],err;

  TrainError = 0;
  for (Year=TRAIN_LWB; Year<=TRAIN_UPB; Year++) {
    SimulateNet(Net, &(Sunspots[Year-N]), Output, &(Sunspots[Year]), FALSE);
/*    TrainError += Net->Error; */
    err = Output[0]-Sunspots[Year];
    TrainError += sqr(err);
  }
  TrainError =   TrainError/(TRAIN_UPB-TRAIN_LWB+1);

  TestError = 0;
  for (Year=TEST_LWB; Year<=TEST_UPB; Year++) {
    SimulateNet(Net, &(Sunspots[Year-N]), Output, &(Sunspots[Year]), FALSE);
/*    TestError += Net->Error; */
    err = Output[0]-Sunspots[Year];
    TestError += sqr(err);
  }
  TestError =   TestError/(TEST_UPB-TEST_LWB+1);

/*
  fprintf(f, "\nNMSE is %0.3f on Training Set and %0.3f on Test Set",
             TrainError / TrainErrorPredictingMean,
             TestError / TestErrorPredictingMean);
*/

  fprintf(f, "\nMSE is %f on Training Set and %f on Test Set",
             TrainError, TestError);
}


void EvaluateNet(NET* Net)
{
  INT  Year;
  REAL Output [M];
  REAL Output_[M];

  fprintf(f, "\n\n\n");
  fprintf(f, "Year    Sunspots    Open-Loop Prediction    Closed-Loop Prediction\n");
  fprintf(f, "\n");
  for (Year=EVAL_LWB; Year<=EVAL_UPB; Year++) {
    SimulateNet(Net, &(Sunspots [Year-N]), Output,  &(Sunspots [Year]), FALSE);
    SimulateNet(Net, &(Sunspots_[Year-N]), Output_, &(Sunspots_[Year]), FALSE);
    Sunspots_[Year] = Output_[0];
    fprintf(f, "%d       %0.3f                   %0.3f                     %0.3f\n",
               FIRST_YEAR + Year,
               Sunspots[Year],
               Output [0],
               Output_[0]);
  }
}


/******************************************************************************
                                    M A I N
 ******************************************************************************/


#define MAXCT 1000
void main()
{
  NET  Net;
  BOOL Stop;
  REAL MinTestError;
  int ct=0;
  
  InitializeRandoms();

  GenerateNetwork(&Net);

  RandomWeights(&Net);

  InitializeApplication(&Net);

  Stop = FALSE;
  MinTestError = MAX_REAL;

  fprintf(f, "\nTrainErrorPredictingMean=%0.3f\n", TrainErrorPredictingMean);
  fprintf(f, "\nTestErrorPredictingMean=%0.3f\n", TestErrorPredictingMean);

/*
  do {
    TrainNet(&Net, 10);
    TestNet(&Net);
    if (TestError < MinTestError) {
      fprintf(f, " - saving Weights ...");
      MinTestError = TestError;
      SaveWeights(&Net);
    }
    else if (TestError > 1.2 * MinTestError) {
      fprintf(f, " - stopping Training and restoring Weights ...");
      Stop = TRUE;
      RestoreWeights(&Net);
    }
  } while (NOT Stop);
*/

  do {
    TrainNet(&Net, 10);
    TestNet(&Net);
    ct++;
  } while (ct<MAXCT);

  fprintf(f, "\nFinal state ...");
  TestNet(&Net);
  EvaluateNet(&Net);
   
  FinalizeApplication(&Net);
}

/*
Simulator Output for the Time-Series Forecasting
Application

NMSE is 0.879 on Training Set and 0.834 on Test Set - saving Weights ...
NMSE is 0.818 on Training Set and 0.783 on Test Set - saving Weights ...
NMSE is 0.749 on Training Set and 0.693 on Test Set - saving Weights ...
NMSE is 0.691 on Training Set and 0.614 on Test Set - saving Weights ...
NMSE is 0.622 on Training Set and 0.555 on Test Set - saving Weights ...
NMSE is 0.569 on Training Set and 0.491 on Test Set - saving Weights ...
NMSE is 0.533 on Training Set and 0.467 on Test Set - saving Weights ...
NMSE is 0.490 on Training Set and 0.416 on Test Set - saving Weights ...
NMSE is 0.470 on Training Set and 0.401 on Test Set - saving Weights ...
NMSE is 0.441 on Training Set and 0.361 on Test Set - saving Weights ...
.
.
.
NMSE is 0.142 on Training Set and 0.143 on Test Set
NMSE is 0.142 on Training Set and 0.146 on Test Set
NMSE is 0.141 on Training Set and 0.143 on Test Set
NMSE is 0.146 on Training Set and 0.141 on Test Set
NMSE is 0.144 on Training Set and 0.141 on Test Set
NMSE is 0.140 on Training Set and 0.142 on Test Set
NMSE is 0.144 on Training Set and 0.148 on Test Set
NMSE is 0.140 on Training Set and 0.139 on Test Set - saving Weights ...
NMSE is 0.140 on Training Set and 0.140 on Test Set
NMSE is 0.141 on Training Set and 0.138 on Test Set - saving Weights ...
.
.
.
NMSE is 0.104 on Training Set and 0.154 on Test Set
NMSE is 0.102 on Training Set and 0.160 on Test Set
NMSE is 0.102 on Training Set and 0.160 on Test Set
NMSE is 0.100 on Training Set and 0.157 on Test Set
NMSE is 0.105 on Training Set and 0.153 on Test Set
NMSE is 0.100 on Training Set and 0.155 on Test Set
NMSE is 0.101 on Training Set and 0.154 on Test Set
NMSE is 0.100 on Training Set and 0.158 on Test Set
NMSE is 0.107 on Training Set and 0.170 on Test Set - stopping Training
                                                      and restoring Weights ...
NMSE is 0.141 on Training Set and 0.138 on Test Set


Year    Sunspots    Open-Loop Prediction    Closed-Loop Prediction

1960       0.572                   0.532                     0.532
1961       0.327                   0.334                     0.301
1962       0.258                   0.158                     0.146
1963       0.217                   0.156                     0.098
1964       0.143                   0.236                     0.149
1965       0.164                   0.230                     0.273
1966       0.298                   0.263                     0.405
1967       0.495                   0.454                     0.552
1968       0.545                   0.615                     0.627
1969       0.544                   0.550                     0.589
1970       0.540                   0.474                     0.464
1971       0.380                   0.455                     0.305
1972       0.390                   0.270                     0.191
1973       0.260                   0.275                     0.139
1974       0.245                   0.211                     0.158
1975       0.165                   0.181                     0.170
1976       0.153                   0.128                     0.175
1977       0.215                   0.151                     0.193
1978       0.489                   0.316                     0.274
1979       0.754                   0.622                     0.373


*/
