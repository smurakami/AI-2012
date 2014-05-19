/******************************************************************************
                            D E C L A R A T I O N S
 ******************************************************************************/

#include "header.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>


#define FALSE         0
#define TRUE          1
#define NOT           !
#define AND           &&
#define OR            ||

#define LO            0
#define HI            1

#define Sval 0.99
#define BINARY(x)     ((x)>Sval ? HI : LO)
/* 
#define LO            -1
#define HI            +1
#define BIPOLAR(x)    ((x)==FALSE ? LO : HI) 
*/

/* グローバル変数群 */
NET G_Net;//結果表示プログラムとの連携のため、グローバル変数にしてみました。
int G_out_cube[X][Y][Z];//ニューラルネットの出力の値を表示するためのバッファ

/******************************************************************************
        R A N D O M S   D R A W N   F R O M   D I S T R I B U T I O N S
 ******************************************************************************/


void InitializeRandoms()
{
  srand(4711);
}

#define RMAX 32767 /* 2^15-1 */
float Rand01()
{
    long int r;
    
    r=rand();
    return ((double)(r % RMAX)/(RMAX-1));

}

INT RandomEqualINT(INT Low, INT High)
{
  return rand() % (High-Low+1) + Low; 
}      


/******************************************************************************
               A P P L I C A T I O N - S P E C I F I C   C O D E
 ******************************************************************************/


FLOAT                   Input [N];
FILE*                 f;


void InitializeApplication(NET* Net)
{
  INT n,i,j;

  for (i=0; i<Y; i++) {
      for (j=0; j<X; j++) {
/*        Input [i*X+j] = (RandomEqualINT(LO, 100*HI)%2); */
	  Input [i*X+j] = Rand01();
    }
  }
  f = fopen("Queen.txt", "w");
}

void WriteNet(NET* Net)
{
  INT i,j;
   
  for (i=0; i<Y; i++) {
      fprintf(f, "+-+-+-+-+-+-+-+-+\n");
      for (j=0; j<X; j++) {
	  fprintf(f, "|%1c", BINARY(Net->Output[i*X+j]) ? 'Q' : ' ');
      }
      fprintf(f, "|\n");
  }
  fprintf(f, "+-+-+-+-+-+-+-+-+\n");
  fprintf(f, "\n");
}

void Writeinput()
{
  INT i,j;
   
  for (i=0; i<Y; i++) {
      for (j=0; j<X; j++) {
      fprintf(f, "%.2f ", Input[i*X+j]);
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
  Net->Output    = (FLOAT*)  calloc(N, sizeof(FLOAT));
  Net->Threshold = (INT*)  calloc(N, sizeof(INT));

  for (i=0; i<N; i++) {
    Net->Threshold[i] = 0;
  }
}



void SetInput(NET* Net, FLOAT* Input)
{
  INT i;
   
  for (i=0; i<Net->Units; i++) {
    Net->Output[i] = Input[i];
  }
  WriteNet(Net);
}


void GetOutput(NET* Net, FLOAT* Output)
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
#define Aval 100
#define Bval 100
#define Cval 100
#define n_value 9.25

NET* global_net;

FLOAT V(int i,int j)
{
    return (global_net->Output[(i-1)+Y*(j-1)]);
}

void to_dx(int n,int *ip,int *jp)
{
    *ip = n%X+1;
    *jp = n/X+1;

}

FLOAT term11(int i,int j)
{
    int k;
    FLOAT sum;
    
    sum = 0.0;
    for (k=1;k<=Y;k++)
	if (k!=j) sum += V(i,k);
    return sum;
    
}

FLOAT term12(int i, int j)
{
    int k;
    FLOAT sum;

    sum = 0.0;
    for (k=1;k<=X;k++)
	if (k!=i) sum += V(k,j);
    return sum;
    
}

/*sum > 8ならば入力値を上昇させ、
 sum <= 8ならば入力値を減少させる。（すなわち、Uを上昇させる）*/
FLOAT term4()
{
    int k,l;
    FLOAT sum;

    sum = 0.0;
    for (k=1;k<=X;k++)
	for (l=1;l<=Y;l++)
	    sum += V(k,l);
    return sum-n_value;
    
}

FLOAT R(int i, int j)
{
    int k;
    FLOAT sum;

    sum = 0.0;
    if (i>j) {
	for (k=i-j+1;k<=X;k++)
	    if (k!=i) sum += V(k,k-i+j);
    } else {
	for (k=1;k<=X+i-j;k++)
	    if (k!=i) sum += V(k,k-i+j);
    }

    return(sum*Bval);
}

FLOAT S(int i, int j)
{
    int k;
    FLOAT sum;

    sum = 0.0;
    if (i+j>X) {
	for (k=i+j-X;k<=X;k++)
	    if (k!=i) sum += V(k,i+j-k);
    } else {
	for (k=1;k<=i+j-1;k++)
	    if (k!=i) sum += V(k,i+j-k);
    }

    return(sum*Bval);
}

FLOAT Energy()
{
    float E,tm6=0.0;
    FLOAT tm1=0.0,tm2=0.0,tm3=0.0,tm4=0.0,tm5=0.0;
    int i,j,k;
    FLOAT tmp;
    
    for (i=1;i<=X;i++)
	for (j=1;j<=Y;j++)
	    tm1 += term11(i,j)*V(i,j) + term12(i,j)*V(i,j);

    for (i=2;i<=X;i++)
	for (j=1;j<=i-1;j++) {
	    tmp = 0;
	    for (k=i-j+1;k<=X;k++) 
		if (k!=i) tmp += V(k,k-i+j);
	    tm2 += tmp*V(i,j);
	}

    for (i=1;i<=X;i++)
	for (j=i;j<=Y;j++) {
	    tmp = 0;
	    for (k=1;k<=X+i-j;k++) 
		if (k!=i) tmp += V(k,k-i+j);
	    tm2 += tmp*V(i,j);
	}

    for (i=1;i<=X;i++)
	for (j=Y-i+1;j<=Y;j++) {
	    tmp = 0;
	    for (k=i+j-X;k<=X;k++) 
		if (k!=i) tmp += V(k,i+j-k);
	    tm2 += tmp*V(i,j);
	}

    for (i=1;i<=X-1;i++)
	for (j=1;j<=Y-i;j++) {
	    tmp = 0;
	    for (k=1;k<=i+j-1;k++) 
		if (k!=i) tmp += V(k,i+j-k);
	    tm2 += tmp*V(i,j);
	}

    tm6 = term4();
    tm6 = tm6*tm6;

    E=Aval*tm1+Bval*tm2+Bval*tm3+Bval*tm4+Bval*tm5+Cval*tm6;

/*
    fprintf(f, "tm1=%d tm2=%d tm3=%d tm4=%d tm5=%d tm6=%f \n",
	    tm1,tm2,tm3,tm4,tm5,tm6);
*/

    return(E/2);

}

float U(int i, int j)
{
    float sum;

    sum = Aval*(term11(i,j)+term12(i,j)) + R(i,j) 
	+ S(i,j) + Cval*term4();

    return -sum;

}



BOOL PropagateUnit(NET* Net, INT i)
{
  INT  idx,jdx;
  FLOAT  Out,Uvalue;
  BOOL Changed;

  Changed = FALSE;

  to_dx(i,&idx,&jdx);
  Out = Uvalue = U(idx,jdx);

  if (Uvalue>Sval)
      Out = HI;
  else if (Uvalue<1-Sval) Out=LO;
  if (Out != Net->Output[i]) {
      Changed = TRUE;
      Net->Output[i] = Out;
  }
  return Changed;
}


void PropagateNet(NET* Net)
{
  INT Iteration, IterationOfLastChange;


  global_net = Net;

  Iteration = 0;
  IterationOfLastChange = 0;
  do {
      if (Iteration % 100 == 0) {
	  fprintf(f, "E(%d) = %f \n",Iteration,Energy());
 	  WriteNet(Net);
	  fprintf(f, "\n");
      }
      Iteration++;
      if (PropagateUnit(Net, RandomEqualINT(0, Net->Units-1)))
	  IterationOfLastChange = Iteration;
  } while (Iteration-IterationOfLastChange < 10*Net->Units);
  /* ユニット数の10倍のターン変化が無かったら、中止する */
  fprintf(f, "Final E = %f Total Iteration=%d\n",Energy(),Iteration);  
  WriteNet(Net);
  fprintf(f, "\n");
}


/******************************************************************************
                      S I M U L A T I N G   T H E   N E T
 ******************************************************************************/


void SimulateNet(NET* Net, FLOAT* L_Input)
{
  FLOAT Output[N];

  Net = &G_Net;//結果表示用関数との連携のため、グローバル変数を使うようにした。
   
  SetInput(Net, Input);
  PropagateNet(Net);
  GetOutput(Net, Output);
}

/******************************************************************************
                                   M A I N
 ******************************************************************************/

int main(int argc, char *argv[])
{
  /*
    結果表示用のウインドウ等の初期化
  */
  glutInitWindowSize(800, 800);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  init();

  /* ニューラルネットワークの初期化 */

  NET Net;
  INT n;

  InitializeRandoms();
  GenerateNetwork(&Net);
  InitializeApplication(&Net);
  
  Writeinput();
  G_Net = Net;//グローバルなネットに作成したネットをコピー

  //SimulateNetはglutMainLoopにおいて、キーボードの's'が入力されたときに実行される
  //SimulateNet(&Net, Input);
  

  /* 実行 */
  glutMainLoop();
  
  FinalizeApplication(&Net);

  return 0;
}
