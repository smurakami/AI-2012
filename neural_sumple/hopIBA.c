/******************************************************************************

                      ==============
        Network:      Hopfield Model
                      ==============

        Application:  Autoassociative Memory
                      Associative Recall of Images

        Author:       Karsten Kutza
        Date:         30.1.96

        Reference:    J.J. Hopfield
                      Neural Networks and Physical Systems
                      with Emergent Collective Computational Abilities
                      Proceedings of the National Academy of Sciences, 79,
                      pp. 2554-2558, 1982

 ******************************************************************************/




/******************************************************************************
                            D E C L A R A T I O N S
 ******************************************************************************/


#include <stdlib.h>
#include <stdio.h>


typedef int           BOOL;
typedef char          CHAR;
typedef int           INT;

#define FALSE         0
#define TRUE          1
#define NOT           !
#define AND           &&
#define OR            ||

#define LO            0
#define HI            1

#define BINARY(x)     ((x)==LO ? FALSE : TRUE)
/* 
#define LO            -1
#define HI            +1
#define BIPOLAR(x)    ((x)==FALSE ? LO : HI) 
*/


typedef struct {                     /* A NET:                                */
        INT           Units;         /* - number of units in this net         */
        INT*          Output;        /* - output of ith unit                  */
        INT*          Threshold;     /* - threshold of ith unit               */
        INT**         Weight;        /* - connection weights to ith unit      */
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


/******************************************************************************
               A P P L I C A T I O N - S P E C I F I C   C O D E
 ******************************************************************************/


#define NUM_DATA      3
#define X             10
#define Y             10

#define N             (X * Y)

CHAR                  Pattern[NUM_DATA][Y][X]  = { { "OOOOOOOOOO",
                                                     "OOOOOOOOOO",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "OOOOOOOOOO",
                                                     "OOOOOOOOOO"  },

                                                   { "OOOOOOOOO ",
                                                     "OOOOOOOOOO",
                                                     "OO      OO",
                                                     "OO     OO ",
                                                     "OOOOOOOO  ",
                                                     "OOOOOOO   ",
                                                     "OO     OO ",
                                                     "OO      OO",
                                                     "OOOO  OOOO",
                                                     "OOOOOOOOO "  },

                                                   { "    OO    ",
                                                     "   OOOO   ",
                                                     "  OO  OO  ",
                                                     "  OO  OO  ",
                                                     "  OOOOOO  ",
                                                     " OOOOOOOO ",
                                                     " OO    OO ",
                                                     " OO    OO ",
                                                     "OO      OO",
                                                     "OO      OO"  } };

CHAR                  Pattern_[NUM_DATA][Y][X] = { { "OOOOOOOOOO",
                                                     "OOOOOOOOOO",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "    OO    ",
                                                     "    OO    " },

                                                   { "          ",
                                                     "    OO    ",
                                                     "   OOOO   ",
                                                     "  OO  OO  ",
                                                     "  OOOOOO  ",
                                                     " OOO  OOO ",
                                                     " OO    OO ",
                                                     " OO    OO ",
                                                     " OO    OO ",
                                                     "          "  },

                                                   { "OOOOOOOOOO",
                                                     "OOOO  OOOO",
                                                     "OOO    OOO",
                                                     "OO  OO  OO",
                                                     "OO      OO",
                                                     "O   OO   O",
                                                     "O  OOOO  O",
                                                     "O  OOOO  O",
                                                     "O  OOOO  O",
                                                     "OOOOOOOOOO" }};



INT                   Input [NUM_DATA][N];
INT                   Input_[NUM_DATA][N];

FILE*                 f;


void InitializeApplication(NET* Net)
{
  INT n,i,j;

  for (n=0; n<NUM_DATA; n++) {
    for (i=0; i<Y; i++) {
      for (j=0; j<X; j++) {
        Input [n][i*X+j] = BINARY(Pattern [n][i][j] == 'O');
        Input_[n][i*X+j] = BINARY(Pattern_[n][i][j] == 'O');
      }
    }
  }
  f = fopen("HOPFIELD.txt", "w");
}


void WriteNet(NET* Net)
{
  INT i,j;

   
  for (i=0; i<Y; i++) {
    for (j=0; j<X; j++) {
      fprintf(f, "%c", BINARY(Net->Output[i*X+j]) ? 'O' : ' ');
    }
    fprintf(f, "\n");
  }
  fprintf(f, "\n");
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
  INT i;

  Net->Units     = N;
  Net->Output    = (INT*)  calloc(N, sizeof(INT));
  Net->Threshold = (INT*)  calloc(N, sizeof(INT));
  Net->Weight    = (INT**) calloc(N, sizeof(INT*));

  for (i=0; i<N; i++) {
    Net->Threshold[i] = 0;
    Net->Weight[i]    = (INT*) calloc(N, sizeof(INT));
  }
}



void CalculateWeights(NET* Net)
{
  INT i,j,n;
  INT Weight;

  for (i=0; i<Net->Units; i++) {
    for (j=0; j<Net->Units; j++) {
      Weight = 0;
      if (i!=j) {
        for (n=0; n<NUM_DATA; n++) {
/*          Weight += Input[n][i] * Input[n][j]; */
	    Weight += (2*Input[n][i]-1) * (2*Input[n][j]-1);
        }
      }
      Net->Weight[i][j] = Weight;
    }
  }

/*
  for (i=0; i<Net->Units; i++) {
      for (j=0; j<Net->Units; j++) {
	fprintf(f,"%3d ", Net->Weight[i][j]);
    }
      fprintf(f, "\n");
  }
*/ 

}


void SetInput(NET* Net, INT* Input)
{
  INT i;
   
  for (i=0; i<Net->Units; i++) {
    Net->Output[i] = Input[i];
  }
  WriteNet(Net);
}


void GetOutput(NET* Net, INT* Output)
{
  INT i;
   
  for (i=0; i<Net->Units; i++) {
    Output[i] = Net->Output[i];
  }
  WriteNet(Net);
}


/******************************************************************************
                     P R O P A G A T I N G   S I G N A L S
 ******************************************************************************/


INT Energy(NET* Net)
{
  INT i,e=0,j;

  for (i=0; i<N; i++) {
      for (j=i+1; j<N; j++)
	  e -= Net->Weight[i][j]*Net->Output[i]*Net->Output[j];
      e -= Net->Weight[i][i]*Net->Output[i];
  }
  return(e);
}

BOOL PropagateUnit(NET* Net, INT i)
{
  INT  j;
  INT  Sum, Out;
  BOOL Changed;

  Changed = FALSE;
  Sum = 0;
  for (j=0; j<Net->Units; j++) {
    Sum += Net->Weight[i][j] * (2*Net->Output[j]-1);
  }

  if (Sum != Net->Threshold[i]) {
      if (Sum < Net->Threshold[i]) Out = LO;
      if (Sum >= Net->Threshold[i]) Out = HI;
      if (Out != Net->Output[i]) {
	  Changed = TRUE;
	  Net->Output[i] = Out;
      }
  }
  return Changed;
}


void PropagateNet(NET* Net)
{
  INT Iteration, IterationOfLastChange;

  Iteration = 0;
  IterationOfLastChange = 0;
  do {
    if (Iteration%100 == 0) {
	fprintf(f, "Energy(%d)=%d\n",Iteration,Energy(Net));
	WriteNet(Net);
    }
    Iteration++;
    if (PropagateUnit(Net, RandomEqualINT(0, Net->Units-1)))
      IterationOfLastChange = Iteration;
  } while (Iteration-IterationOfLastChange < 2*Net->Units);

  fprintf(f, "Final Energy(%d)=%d\n",Iteration,Energy(Net));
  WriteNet(Net);

}


/******************************************************************************
                      S I M U L A T I N G   T H E   N E T
 ******************************************************************************/


void SimulateNet(NET* Net, INT* Input)
{
  INT Output[N];
   
  SetInput(Net, Input);
  PropagateNet(Net);
  GetOutput(Net, Output);
}


/******************************************************************************
                                    M A I N
 ******************************************************************************/


main()
{
  NET Net;
  INT n;

  InitializeRandoms();
  GenerateNetwork(&Net);
  InitializeApplication(&Net);
  CalculateWeights(&Net);
   
  for (n=0; n<NUM_DATA; n++) {
    SimulateNet(&Net, Input[n]);
  }
  for (n=0; n<NUM_DATA; n++) {
    SimulateNet(&Net, Input_[n]);
  }
   
  FinalizeApplication(&Net);
}

/*
Simulator Output for the Autoassociative Memory Application

O O O O O     O O O O O 
 O O O O O     O O O O O
O O O O O     O O O O O 
 O O O O O     O O O O O
O O O O O     O O O O O 
 O O O O O     O O O O O
O O O O O     O O O O O 
 O O O O O     O O O O O
O O O O O     O O O O O 
 O O O O O ->  O O O O O

OO  OO  OO    OO  OO  OO
OO  OO  OO    OO  OO  OO
  OO  OO        OO  OO  
  OO  OO        OO  OO  
OO  OO  OO    OO  OO  OO
OO  OO  OO    OO  OO  OO
  OO  OO        OO  OO  
  OO  OO        OO  OO  
OO  OO  OO    OO  OO  OO
OO  OO  OO -> OO  OO  OO

OOOOO         OOOOO     
OOOOO         OOOOO     
OOOOO         OOOOO     
OOOOO         OOOOO     
OOOOO         OOOOO     
     OOOOO         OOOOO
     OOOOO         OOOOO
     OOOOO         OOOOO
     OOOOO         OOOOO
     OOOOO ->      OOOOO

O  O  O  O    O  O  O  O
 O  O  O       O  O  O  
  O  O  O       O  O  O 
O  O  O  O    O  O  O  O
 O  O  O       O  O  O  
  O  O  O       O  O  O 
O  O  O  O    O  O  O  O
 O  O  O       O  O  O  
  O  O  O       O  O  O 
O  O  O  O -> O  O  O  O

OOOOOOOOOO    OOOOOOOOOO
O        O    O        O
O OOOOOO O    O OOOOOO O
O O    O O    O O    O O
O O OO O O    O O OO O O
O O OO O O    O O OO O O
O O    O O    O O    O O
O OOOOOO O    O OOOOOO O
O        O    O        O
OOOOOOOOOO -> OOOOOOOOOO

              O O O O O 
               O O O O O
              O O O O O 
               O O O O O
              O O O O O 
 O O O O O     O O O O O
O O O O O     O O O O O 
 O O O O O     O O O O O
O O O O O     O O O O O 
 O O O O O ->  O O O O O

OOO O    O    OO  OO  OO
 O  OOO OO    OO  OO  OO
  O O OO O      OO  OO  
 OOO   O        OO  OO  
OO  O  OOO    OO  OO  OO
 O OOO   O    OO  OO  OO
O OO  O  O      OO  OO  
   O OOO        OO  OO  
OO OOO  O     OO  OO  OO
 O  O  OOO -> OO  OO  OO

OOOOO         OOOOO     
O   O OOO     OOOOO     
O   O OOO     OOOOO     
O   O OOO     OOOOO     
OOOOO         OOOOO     
     OOOOO         OOOOO
 OOO O   O         OOOOO
 OOO O   O         OOOOO
 OOO O   O         OOOOO
     OOOOO ->      OOOOO

O  OOOO  O    O  O  O  O
OO  OOOO       O  O  O  
OOO  OOOO       O  O  O 
OOOO  OOOO    O  O  O  O
 OOOO  OOO     O  O  O  
  OOOO  OO      O  O  O 
O  OOOO  O    O  O  O  O
OO  OOOO       O  O  O  
OOO  OOOO       O  O  O 
OOOO  OOOO -> O  O  O  O

OOOOOOOOOO    OOOOOOOOOO
O        O    O        O
O        O    O OOOOOO O
O        O    O O    O O
O   OO   O    O O OO O O
O   OO   O    O O OO O O
O        O    O O    O O
O        O    O OOOOOO O
O        O    O        O
OOOOOOOOOO -> OOOOOOOOOO

*/
