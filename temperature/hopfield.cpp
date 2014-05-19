/******************************************************************************
                            D E C L A R A T I O N S
******************************************************************************/

#include "header.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
using namespace std;

int t_counter;
int t_array[2000];

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
/*
 * 今回のニューラルネットでは、出力を変えたくないニューロンが存在する。
 * それは、問題として与えられた、空欄でない数字の入った欄に対応するニューロンである。
 * たとえば、(x, y)に数字が入っていれば、(x, y, 1) ~ (x, y, 9)は固定される。
 * そのような情報を管理するために、以下のblank_index[N], blank_index_num
 * を利用する。
 * blank_index[N]は、空欄であるニューロンのindexを保存する。
 * blank_index_numは、空欄であるニューロンの数をほぞんする。
 * これらは、ニューラルネットワークを稼働させるときに、出力を計算するニューロンを
 * ランダム抽出する際に使用する。
 */
int blank_index[N];
int blank_index_num;

/**************
  シグモイド関数
 **************/
//シグモイド関数の計算の高速化のためのテーブル
double sigmoid_table[1000];
double sigmoid(double x){
  if(x >= 5) return 1;
  if(x < -5) return 0;
  int i = (int)((x + 5) * 100);
  return sigmoid_table[i];
}
void initSigmoidTable(){
  for(int i = 0; i < 1000; i++){
    sigmoid_table[i] = 1.0 / (1.0 + exp(-(0.01 * i - 5)));
  }
}

/***************
   定数
 ***************/
double delta_t = 0.1;
double temperature = 0.4;

/******************************************************************************
        R A N D O M S   D R A W N   F R O M   D I S T R I B U T I O N S
******************************************************************************/


void InitializeRandoms()
{
  srand(time(NULL));
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

//問題中の空欄に相当するユニットをランダムに抽出
INT RandomBlankUnit(){
  return blank_index[rand() % blank_index_num];
}


/******************************************************************************
               A P P L I C A T I O N - S P E C I F I C   C O D E
******************************************************************************/

/* N = X * Y * Z */
FLOAT                   Input [N];
FILE*                 f;


void InitializeApplication(NET* Net)
{
  INT n,i,j,k;

  for (i=0; i<X; i++) {
    for (j=0; j<Y; j++) {
      for (k=0; k<Z; k++) {
/*        Input [i*X+j] = (RandomEqualINT(LO, 100*HI)%2); */
        if(G_out_cube[i][j][k]) break;
      }
      if(k == Z){
        for(int l = 0; l < Z; l++){
          Input[i*Y*Z+j*Z+l] = Rand01();
          blank_index[blank_index_num] = i*Y*Z+j*Z+l;
          blank_index_num++;
        }
      }else{
        for(int l = 0; l < Z; l++){
          if(k == l) Input[i*Y*Z+j*Z+l] = 1;
          else Input[i*Y*Z+j*Z+l] = 0;
        }
      }
    }
  }
  f = stdout;
}

void WriteNet(NET* Net)
{
  glutPostRedisplay();
  glFlush();
}

void Writeinput()
{
  INT i,j,k;

  for(k = 0; k < Z; k++){
    fprintf(f, "*** k = %d ***\n", k);
    for (i=0; i<X; i++) {
      for (j=0; j<Y; j++) {
        fprintf(f, "%.2f ", Input[i*Y*Z+j*Z+k]);
      }  
      fprintf(f, "\n");
    }
    fprintf(f, "\n");
  }
}

void Writeoutput()
{
  INT i,j,k;

  fprintf(f, "Writeoutput\n");
  for(k = 0; k < Z; k++){
    fprintf(f, "*** k = %d ***\n", k);
    for (i=0; i<X; i++) {
      for (j=0; j<Y; j++) {
        fprintf(f, "%.2f ", G_Net.Output[i*Y*Z+j*Z+k]);
      }  
      fprintf(f, "\n");
    }
    fprintf(f, "\n");
  }
}


void FinalizeApplication(NET* Net)
{
  char name[1000];
  sprintf(name, "temperature_%.2f.dat", temperature);
  FILE * fpt = fopen(name, "w");
  for(int i = 0; i < 2000; i++){
    fprintf(fpt, "%f %f\n", (i - 1000) / 100.0, (double)t_array[i] / t_counter);
  }
  fclose(fpt);

  sprintf(name, "sigmoid_%.2f.dat", temperature);
  FILE * fps = fopen(name, "w");
  for(int i = 0; i < 2000; i++){
    fprintf(fps, "%f %f\n", (i - 1000) / 100.0, sigmoid((i - 1000) / 100.0));
  }
  fclose(fps);

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
  Net->U         = (FLOAT*)  calloc(N, sizeof(FLOAT));
  Net->Threshold = (INT*)  calloc(N, sizeof(INT));
  Net->isFinished = 0;

  for (i=0; i<N; i++) {
    Net->Threshold[i] = 0;
  }
}



void SetInput(NET* Net, FLOAT* Input)
{
  INT i;
   
  for (i=0; i<Net->Units; i++) {
    Net->U[i] = Net->Output[i] = Input[i];
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

void ReadProblem(char * input_file){
  ifstream input_stream;
  if(input_file){
    input_stream.open(input_file);
  }else{
    input_stream.open("input");
  }
  int i, j, k;//counter
  char c;
  int n;
  i = j = 0;

  cout << "begin problem..." << endl << endl;
  while(i < X){
    j = 0;
    while(j < Y){
      input_stream >> c;
      if((c > '0')&&(c <= '9')){
        n = c - '0';
        for(k = 0; k < Z; k++){
          if(k == n-1) G_out_cube[i][j][k] = 1;
        }
        j++;
      }else if((c == '0') || (c == '*')){
        j++;
      }
    }
    i++;
  }

  //check
  for(i = 0; i < X; i++){
    for(j = 0; j < Y; j++){
      for(k = 0; k < Z; k++){
        if(G_out_cube[i][j][k]){
          cout << k+1;
          break;
        }
      }
      if(k == Z) cout << '*';
      if(j % 3 == 2) cout << ' ';
    }
    cout << endl;
    if(i % 3 == 2) cout << endl;
  }


}

void PrintUsage(){
  cout << "**** USAGE ****" << endl;
  cout << endl;
  cout << "S : ステップ実行(10000ステップ)" << endl;
  cout << "C : Continue(問題が解けるまで実行)" << endl;
  cout << "H : Head View(上から見る)" << endl;
  cout << "L : Level View(横から見る)" << endl;
  cout << "N : Number View(一つの数字に着目して見る)" << endl;
  cout << "Click and Drug : 回転、縮小、拡大" << endl;
  cout << "Q : quit" << endl;
  cout << endl;
  cout << "***************" << endl;
  cout << endl;

}



/******************************************************************************
                     P R O P A G A T I N G   S I G N A L S
******************************************************************************/
#define Aval 100
#define Bval 100
#define Cval 100
#define n_value 9.25

NET* global_net;

//手動でデバック済み
FLOAT V(int i,int j, int k)
{
  return (global_net->Output[(i-1)*Y*Z+(j-1)*Z+(k-1)]);
}

//手動でデバック済み
void to_dx(int n,int *ip,int *jp, int *kp)
{
  *ip = n/(Y*Z)+1;
  *jp = (n%(Y*Z))/Z+1;
  *kp = n%Z + 1;

}
//横のユニットの出力の総和 (x方向)
FLOAT term11(int i,int j, int k)
{
  int l;
  FLOAT sum;
    
  sum = 0.0;
  for (l=1; l<=X; l++)
    sum += V(l,j,k);
  return sum - 1.0;
    
}

//縦のユニットの出力の総和 (y方向)
FLOAT term12(int i, int j, int k)
{
  int l;
  FLOAT sum;
    
  sum = 0.0;
  for (l=1; l<=X; l++)
    sum += V(i,l,k);
  return sum - 1.0;
    
}

//高さ方向のユニットの出力の総和 (z方向)
FLOAT term13(int i, int j, int k)
{
  int l;
  FLOAT sum;
    
  sum = 0.0;
  for (l=1; l<=X; l++)
    sum += V(i,j,l);
  return sum - 1.0;
    
}

//一つの数字は一つの区画(9x9マス)にひとつしか入らないというルールに対応する。
//そのマスが所属する区画の、他のユニットの出力の総和を返す。
FLOAT term2(int i, int j, int k)
{
  FLOAT sum;
  int l, m;

  sum = 0.0;
  for(l = i - (i - 1) % 3; l < i - (i - 1) % 3 + 3; l++){
    for(m = i - (i - 1) % 3; m < i - (i - 1) % 3 + 3; m++){
      sum += V(l, m, k);
    }
  }
  
  return sum - 1.0;
}

/*各数字を9回ずつ出現するように働きかけるネットワーク
  sum > 9ならば入力値を上昇させ、
  sum <= 9ならば入力値を減少させる。（すなわち、Uを上昇させる）
  n_value = 9.25である。
*/
/* 改良の余地あり。各々のユニットは自分自身と通信は出来ないはずだから、if(i != k || j != l)の条件を付加するべき */
FLOAT term4(int i, int j, int k)
{
  int l, m, n;
  FLOAT sum;
  
  sum = 0.0;
  for (l=1;l<=X;l++){
    for (m=1;m<=Y;m++){
      sum += V(l, m, k);
    }
  }
  return sum-n_value;
    
}

/*
  今回は使わない斜め方向のかさなり
 */
// FLOAT R(int i, int j)
// {
//   int k;
//   FLOAT sum;

//   sum = 0.0;
//   if (i>j) {
//     for (k=i-j+1;k<=X;k++)
// 	    if (k!=i) sum += V(k,k-i+j);
//   } else {
//     for (k=1;k<=X+i-j;k++)
// 	    if (k!=i) sum += V(k,k-i+j);
//   }

//   return(sum*Bval);
// }

// FLOAT S(int i, int j)
// {
//   int k;
//   FLOAT sum;

//   sum = 0.0;
//   if (i+j>X) {
//     for (k=i+j-X;k<=X;k++)
// 	    if (k!=i) sum += V(k,i+j-k);
//   } else {
//     for (k=1;k<=i+j-1;k++)
// 	    if (k!=i) sum += V(k,i+j-k);
//   }

//   return(sum*Bval);
// }



FLOAT Energy()
{
  float E,tm6=0.0;
  FLOAT tm1=0.0,tm2=0.0,tm3=0.0,tm4=0.0,tm5=0.0;
  int i,j,k, l;
  FLOAT tmp;
    
  for (j=1;j<=Y;j++)
    for (k=1;k<=Z;k++)
      tm1 += term11(1,j,k)*term11(1,j,k);

  for (i=1;i<=X;i++)
    for (k=1;k<=Z;k++)
      tm1 += term12(i,1,k)*term12(i,1,k);
  
  for (i=1;i<=X;i++)
    for (j=1;j<=Y;j++)
        tm1 += term13(i,j,1)*term13(i,j,1);

  for (i=1;i<=X/3;i++){
    for (j=1;j<=Y/3;j++) {
      for (k=1;k<=Z;k++) {
        tm2 += term2(i*3,j*3,k)*term2(i*3,j*3,k);
      }
    }
  }

  // for (i=1;i<=X;i++){
  //   for (j=1;j<=Y;j++) {
  //     for (j=1;j<=Y;j++) {
  //       tm6 += term4(i,j,k) * V(i,j,k);
  //     }
  //   }
  // }
  
  
  E=Aval*tm1+Bval*tm2+Bval*tm3+Bval*tm4+Bval*tm5+Cval*tm6;

/*
  fprintf(f, "tm1=%d tm2=%d tm3=%d tm4=%d tm5=%d tm6=%f \n",
  tm1,tm2,tm3,tm4,tm5,tm6);
*/

  return(E/2);

}

float U(int i, int j, int k)
{
  float sum;

  sum = -term11(i, j, k) - term12(i, j, k) - term13(i, j, k) - term2(i, j, k);
  G_Net.U[(i-1)*Y*Z+(j-1)*Y+k-1] += sum*delta_t;
  return G_Net.U[(i-1)*Y*Z+(j-1)*Y+k-1];

}



BOOL PropagateUnit(NET* Net, INT i)
{
  INT  idx,jdx,kdx;
  FLOAT  Out,Uvalue;
  BOOL Changed;
  int index;
  
  Changed = FALSE;
  
  to_dx(i,&idx,&jdx,&kdx);
  Uvalue = U(idx,jdx,kdx);

  if((-10.0 < Uvalue) || (Uvalue < 10.0)){
    index = (int)(Uvalue + 10 * 100);
    t_array[index]++;
  }
  t_counter++;
  
  Out = sigmoid(Uvalue / temperature);
  
  if (Out != Net->Output[i]) {
    Changed = TRUE;
    G_out_cube[idx-1][jdx-1][kdx-1] = BINARY(Out);
    Net->Output[i] = Out;
  }
  return Changed;
}


void PropagateNet(NET* Net)
{
  static  INT Iteration, IterationOfLastChange;


  global_net = Net;

  if(Iteration == 0){ fprintf(f, "E(%d) = %f \n",Iteration,Energy());
  fprintf(f, "\n");
  }
  
  do {
    Iteration++;
        if (Iteration % 100000 == 0) {
      break;
    }

    if (PropagateUnit(Net, RandomBlankUnit()))//第二引数はもともとRandomEqualINT(0, Net->Units-1)であった。
      IterationOfLastChange = Iteration;
  } while (Iteration-IterationOfLastChange < 10*Net->Units);
  fprintf(f, "E(%f) = %f \n", Iteration*delta_t, Energy());
  fprintf(f, "temperature(%f) = %f \n", Iteration*delta_t, temperature);
  fprintf(f, "\n");
  FinalizeApplication(&G_Net);
  exit(0);
}


/******************************************************************************
                      S I M U L A T I N G   T H E   N E T
******************************************************************************/


void SimulateNet(NET* Net, FLOAT* L_Input)
{
  FLOAT Output[N];

  Net = &G_Net;//結果表示用関数との連携のため、グローバル変数を使うようにした。
   
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
  char * input_file;
  if(argc > 1)  input_file = argv[1];
  else input_file = NULL;
  ReadProblem(input_file);
  PrintUsage();
  link_out_cube(G_out_cube);
  linkFinished(&G_Net.isFinished);

  initSigmoidTable();
  
  InitializeRandoms();
  GenerateNetwork(&G_Net);
  InitializeApplication(&G_Net);
  //Writeinput();
  SetInput(&G_Net, Input);
  

  //SimulateNetはglutMainLoop()において、キーボードの's'が入力されたときに実行される
  //SimulateNet(&Net, Input);
  

  /* 実行 */
  glutMainLoop();
  
  FinalizeApplication(&Net);

  return 0;
}
