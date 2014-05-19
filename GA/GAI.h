#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cv.h>
#include <highgui.h>
#include "bin_system.h" //二進数表現を簡単にするためのマクロ
#define SIZE  40       //人口頭脳の一辺の長さ
#define FRAME_RATE 20    //人工頭脳稼働のフレームレート
#define FREQ 1          //視神経からのパルス発生の頻度

/*遺伝的アルゴリズム関連の数値*/
#define G_SIZE 50       //遺伝的アルゴリズムで利用する、一世代あたりの個体数
#define G_TIMES 100     //遺伝的アルゴリズムの世代数
#define G_MUTE 100      //突然変異の起こる確率[10分の1%]

/*neuronのstate*/
#define RIGHT   b0001_0000//右向きシナプス
#define UP      b0010_0000//上向きシナプス
#define LEFT    b0100_0000//左向きシナプス
#define DOWN    b1000_0000//下向きシナプス

#define ACT     b0000_0001//興奮している
#define NEIB    b0000_0010//繋がった、隣接している細胞が興奮している
#define RETINA  b0000_1000//網膜からたどりつける

/********************************
          人工頭脳の設計
 ********************************/ 

/*
  以下のような構造体で人工頭脳を表現する。
  人工頭脳は、人工頭脳myAI, 人工神経細胞neuronの二層構造にする。

  人工頭脳AIは、平面上に人工神経細胞が正方形にSIZE*SIZE個集まったものである。
  
  神経細胞では、八桁の二進数により、神経細胞の状態を示す。
  神経細胞の状態を表す変数を、neuron.stateとする.
  便宜のため、コメント中ではstateのx桁目をstate-xと記載する

  このプログラムのなかでは、二進数の表記法を以下のようにする。
  bxxxx_xxxx
  ex.)b0010_0001 = 65
  これは、"bin_system.h"の中で定義されている。
  
  stateが神経細胞の状態を表す規則は以下のようにする。
  state:    表す情報     
  b0000_000x      そのニューロンが興奮しているか 
  b0000_00x0      隣に興奮しているニューロンがあって、その細胞からシナプスがのびているか.[近所の情報]
  b0000_0x00      (空き)
  b0000_x000      今回のみ使う場所。その神経細胞に網膜からたどりつく事ができるか。
  b000x_0000      右向きにシナプスがのびているか 
  b00x0_0000      上向きにシナプスがのびているか 
  b0x00_0000      左向きにシナプスがのびているか 
  bx000_0000      下向きにシナプスがのびているか

  例えば、右にシナプスがのびているかは、下のようなビット演算でチェックすることが出来る
  state & b0001_0000
  また、右にシナプスをのばしたいときは
  state = state | b0001_0000
  という演算で実現することができる。

  さらに演算を分かりやすくするためにマクロで
  state & RIGHT
  state = state | RIGHT
  をいうような表記を採用することとする
*/
//頭脳上の座標を表す構造体.リストとしても使える
typedef struct _point{
  int x;
  int y;
  struct _point * next;
} point;

/*神経細胞*/
typedef struct{
  unsigned char state;
  unsigned char from;
} neuron;

/*人口頭脳を表す構造体*/
typedef struct {
  neuron neu[SIZE][SIZE];
  int value;//その頭脳の評価値
  IplImage * output;//その頭脳の画像表現
  point * retina;
  point * dest;
} myAI;

/*人工頭脳の確保*/
myAI* allocAI();
/*人工頭脳の初期化*/
myAI* initAI(myAI* AI);
/*興奮状態を初期化*/
int initNeuron(myAI * AI);
/*人工頭脳の解放*/
int freeAI(myAI* AI);
int copyNeuron(myAI* dest, myAI* orig);

/********************************
        人工頭脳を進化させる
********************************/ 

/*
  alloc, initされたAIは、シナプス結合がない。すなわち、個々の神経細胞が独立した状態にある。
  このAIの中の神経細胞を、シナプス結合をつかってつなぎ、神経細胞の回路をつくることで、
  はじめてAIは頭脳のシミュレートを行うことができる。
  
  今回は、人工頭脳の端に網膜(座標[3][0])からの信号を一定時間ごとに流し込み、それを人工頭脳の中央にある
  視覚受容野(座標[SIZE/2][SIZE/2])に伝達するとういう脳の働きをシミュレートする。
  
  そのために、網膜と視覚受容野を繋ぐような神経回路を作成したい。

  今回は、人工頭脳のシミュレーションなので、今後のもっと複雑なシミュレーションの練習をかねて、
  人工頭脳の進化を遺伝的アルゴリズムで実装してみようと思う。
  （遺伝的アルゴリズムを使うのはこれが初めてです）
  
  遺伝的アルゴリズムの手順を以下に示す(Wikipediaより)
  
  1.あらかじめG_SIZE個のAI個体が入る配列を二つ確保する。これを、現世代、次世代と呼ぶ
  2.現世代にG_SIZE個の個体をランダムに作成する
  3.評価関数により、現世代の各個体の適応度を計算する
  4.ある確率で次の三つのいずれかの動作を行い、その結果を次世代に保存する。
  - 個体を二つ選択して交差を行う
  - 個体を一つ選択して突然変異を行う
  - 個体を一つ選択してそのままコピーする
  5.次世代の個体数がN個になるまで上記の動作を繰り返す
  6.次世代の個体数がN個になったら次世代の内容を現世代に移す
  7. 3.移行の操作を最大世代数G_TIMES回まで繰り返し、最終的に現世代の中で最も適応度の高いものを解とする

  1.の操作はinitGeneration関数で実装する
  2.の操作はsetFirstGenetarion関数で実装する
  ただしここで、お互いにシナプスをのばしあっているような神経細胞の対が存在すると、
  そこが常に興奮してしまうような部位が生まれてしまう。
  そうなるとあまり面白くないので、そういう場合のみ起きないように除外することにする。
  
*/

/*AIの確保、初期化、進化をさせる関数*/
myAI* initAndEvolveAIWithGA(point * retina, point * dest);//GA.c
/*AIの評価をする関数*/
int valueOfAI(myAI* AI);//value.c
/*一つの世代に対して評価を実行*/
int valueOfAIinGeneration(myAI * generation[G_SIZE]);
/*遺伝的アルゴリズムで使う用の人工知能稼働*/
int runAIforGA(myAI* AI);//run.c


/*最初の世代を初期化する*/
int initGeneration(myAI* generation[G_SIZE]);
/*世代に割り当てたメモリを解放*/
int freeGeneration(myAI* generation[G_SIZE]);
/*最初の世代をランダムにセット*/
int setFirstGeneration(myAI* generation[G_SIZE], point * retina, point * dest);
/*AIのシナプスをランダムにセット*/
int setAIatRandom(myAI* AI);//init.c
//突然変異させる
int mutateAI(myAI * currentAI, myAI * nextAI);//GA.c
//現世代から次世代にうつすAIを選び出す
int selectAI(myAI* currentGeneration[G_SIZE], myAI* nextGeneration[G_SIZE]);
//一様交叉させる
int crossAI(myAI * A1, myAI * A2, myAI * next);
//ランク付けをする
int rankAI(myAI** currentGeneration, int * index);


/********************************
         人工頭脳を稼働させる
********************************/ 

/*神経細胞間の情報の伝達*/
/*
  神経細胞間の情報の伝達は、前半部communicate1と、後半部communicate2で分けて行う。
  communicate1ではAIの中の全神経細胞のうち興奮している(state-1 == 1)ものを調べ、
  その神経細胞と隣接していて、シナプスでつながっている細胞を探し、その細胞のstate-2を1にする。
  communicate2では,再びAIの中の全神経細胞を調べ、state-2が1である場合、state-1 == 1(興奮)
  とする。
*/
int communicate1(myAI* AI);//AI.c
int communicate2(myAI* AI);//AI.c

/*人工頭脳稼働*/
/*
  今回のレポートでは、人工頭脳の端(AI->neu[3][0])に網膜があり、
  そこから断続的にパルスが発生していて、その信号を中央の視覚受容野(AI->neu[size/2][size/2])
  まで伝搬させるという状況をシミュレートする。
  網膜でのパルス発生の頻度はFREQとしてマクロ定義してある。
*/

int runAI(myAI* AI, double runTime);//run.c

/*************************************
       結果表示、その他のための関数群
*************************************/ 

/*画面を一掃する*/
int clearScreen();//print.c
/*現在時刻をマイクロ秒の単位の正確さで返す*/
double currentTime();//util.c
/*人工頭脳のシミュレーションのフレームレート調整*/
int waitAI();//run.c
/*人工頭脳のマップの表示
  その神経が興奮していたら'*'
  そうでなければ'-'をプリント
*/
int printAI(myAI* AI);//print.c
int printAIforGA(myAI * AI);//print.c

/*************************************
        init.c
*************************************/ 
point * allocPoint();
int initPoint(point * p);
point * getPoint(int x, int y);
//頭脳上のニューロン同士の距離を出す
int distance(int x1, int y1, int x2, int y2);
