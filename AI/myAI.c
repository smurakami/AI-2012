#include "myAI.h"

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

  
*/

/*人工頭脳の確保*/
myAI* allocAI(){
    myAI* AI = (myAI*)malloc(sizeof(myAI));
    if(AI == NULL){perror("allocAI"); exit(1);}
    return AI;
}
/*人工頭脳の初期化*/
myAI* initAI(myAI* AI){
    int i, j;//counter
    for(i = 0; i < SIZE; i++){
        for(j = 0; j < SIZE; j++){
            AI->neu[i][j].state = 0;
        }
    }
    return AI;
}
/*人工頭脳の解放*/
int freeAI(myAI* AI){
    free(AI);
    return 0;
}


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

/*世代を確保、初期化する*/
int initGeneration(myAI* generation[G_SIZE]){
    int i;//counter
    for(i = 0; i < G_SIZE; i++){
        generation[i] = allocAI();
        initAI(generation[i]);
    }
    return 0;
}
/*世代に割り当てたメモリを解放*/
int freeGeneration(myAI* generation[G_SIZE]){
    int i;//counter
    for(i = 0; i < G_SIZE; i++){
        freeAI(generation[i]);
    }
    return 0;
}

/*最初の世代をランダムにセット*/
int setFirstGeneration(myAI* generation[G_SIZE]){
    int i;//counter
    for(i = 0; i < G_SIZE; i++){
        setAIatRandom(generation[i]);
    }
    return 0;
}

/*AIのシナプスをランダムにセット*/
int setAIatRandom(myAI* AI){
    int i, j;
    for(i = 0; i < SIZE; i++)
        for(j = 0; j < SIZE; j++){
            if(rand()%2){//50%の確率で右向きにシナプスをのばす
                if(!((j < SIZE-1) && 
                     (AI->neu[i][j+1].state & LEFT)
                       )){//左からシナプスがのびていない場合に限って
                    AI->neu[i][j].state = AI->neu[i][j].state | RIGHT;//右向きにシナプスをのばす
                }   
            }
            //以下同様
            if(rand()%2){//50%の確率で上向きにシナプスをのばす
                if(!((i > 1) &&
                     (AI->neu[i-1][j].state & DOWN)
                       )) AI->neu[i][j].state = AI->neu[i][j].state | UP; 
            }
            if(rand()%2){//50%の確率で左向きにシナプスをのばす
                if(!((j > 1) &&
                     (AI->neu[i][j-1].state & RIGHT)
                       )) AI->neu[i][j].state = AI->neu[i][j].state | LEFT;
            }
            if(rand()%2){//50%の確率で下向きにシナプスをのばす
                if(!((i < SIZE - 1) &&
                     (AI->neu[i+1][j].state & UP)
                       )) AI->neu[i][j].state = AI->neu[i][j].state | DOWN; 
            }
        }  
    return 0;
}

/*AIの評価をする関数*/
int valueOfAI(myAI* AI){
    
    
    
    return 0;
}

/*遺伝的アルゴリズムで使う用の人工知能稼働*/
int runAIforGA(myAI* AI){
    int i//counter
    
    for(i = 0; i < SIZE*SIZE; i++){
        /*神経細胞間で情報を伝達*/
        communicate1(AI);
        communicate2(AI);
        
        /*一定時間ごとに網膜からパルスを受け取る*/
        if((currentTime() - t0) > 1.0/FREQ){
            AI->neu[3][0].state = AI->neu[3][0].state | b0000_0001;
            t0 = currentTime();
        }
        
        clearScreen();
        printAI(AI);
        waitAI();//フレームレートの調節
    }
        
    return 0;
}

/*AIの確保、初期化、進化をさせる関数*/
myAI* initAndEvolveAIWithGA(){
    int gen;
    myAI* currentGeneration[G_SIZE];
    myAI* nextGeneration[G_SIZE];

    /*1*/
    initGeneration(currentGeneration);
    initGeneration(nextGeneration);

    /*2*/
    setFirstGeneration(currentGeneration);
    
    for(gen = 0; gen < G_TIMES; gen++){


    }
    
    freeGeneration(currentGeneration);
    freeGeneration(nextGeneration);
    
    return NULL;
}

/********************************
         人工頭脳を稼働させる
 ********************************/ 

/*人工頭脳稼働*/
/*
  今回のレポートでは、人工頭脳の端(AI->neu[3][0])に網膜があり、
  そこから断続的にパルスが発生していて、その信号を中央の視覚受容野(AI->neu[size/2][size/2])
  まで伝搬させるという状況をシミュレートする。
  網膜でのパルス発生の頻度はFREQとしてマクロ定義してある。
*/
int runAI(myAI* AI, double runTime){
    //printf("hi\n");
    double startTime = currentTime();
    double t0 = currentTime();
    printAI(AI);

    while((currentTime() - startTime) < runTime){
        /*神経細胞間で情報を伝達*/
        communicate1(AI);        
        communicate2(AI);

        /*一定時間ごとに網膜からパルスを受け取る*/
        if((currentTime() - t0) > 1.0/FREQ){
            AI->neu[3][0].state = AI->neu[3][0].state | b0000_0001;
            t0 = currentTime();
        }

        clearScreen();
        printAI(AI);
        waitAI();//フレームレートの調節
    }

    return 0;
}

/*神経細胞間の情報の伝達*/
/*
  神経細胞間の情報の伝達は、前半部communicate1と、後半部communicate2で分けて行う。
  communicate1ではAIの中の全神経細胞のうち興奮している(state-1 == 1)ものを調べ、
  その神経細胞と隣接していて、シナプスでつながっている細胞を探し、その細胞のstate-2を1にする。
  communicate2では,再びAIの中の全神経細胞を調べ、state-2が1である場合、state-1 == 1(興奮)
  とする。
 */

int communicate1(myAI* AI){
    int i, j;//counter
    for(i = 0; i < SIZE; i++){
        for(j = 0; j < SIZE; j++){
            if(AI->neu[i][j].state & b0000_0001){//もしも興奮していたら
                if(AI->neu[i][j].state & RIGHT){//rightにシナプスがのびていたら
                    if(j < SIZE - 1) AI->neu[i][j+1].state = AI->neu[i][j+1].state | NEIB; 
                }
                if(AI->neu[i][j].state & UP){//upにシナプスがのびていたら
                    if(i > 1) AI->neu[i-1][j].state = AI->neu[i-1][j].state | NEIB; 
                }
                if(AI->neu[i][j].state & LEFT){//leftにシナプスがのびていたら
                    if(j > 1) AI->neu[i][j-1].state = AI->neu[i][j-1].state | NEIB; 
                }
                if(AI->neu[i][j].state & DOWN){//downにシナプスがのびていたら
                    if(i < SIZE - 1) AI->neu[i+1][j].state = AI->neu[i+1][j].state | NEIB; 
                }
            }
        }
    }
    return 0;
}

int communicate2(myAI* AI){
    int i, j;//counter
    for(i = 0; i < SIZE; i++){
        for(j = 0; j < SIZE; j++){
            if(AI->neu[i][j].state & b0000_0010){//興奮しているご近所さんとつながってたら
                AI->neu[i][j].state = AI->neu[i][j].state | b0000_0001;//興奮させる
                AI->neu[i][j].state = AI->neu[i][j].state | b0000_1000;//網膜からたどりつけることをメモ
                AI->neu[i][j].state = AI->neu[i][j].state & b1111_1101;//近所の情報をリセット
            }else{
                AI->neu[i][j].state = AI->neu[i][j].state & b1111_1110;//興奮状態を解除
            }
        }
    }
    return 0;
}


/*************************************
        結果表示、その他のための関数群
 ********************+++++************/ 

/*画面を一掃する*/
int clearScreen(){
    printf("\033[2J");
    return 0;
}

/*現在時刻をマイクロ秒の単位の正確さで返す*/
double currentTime(){
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return tp.tv_sec + tp.tv_usec * 1.0E-6;
}

/*人口頭脳のシミュレーションのフレームレート調整*/
int waitAI(){
    double t0, t1;
    double deray = 1.0 / FRAME_RATE;

    t0 = currentTime();
    while(1){
        t1 = currentTime();
        if(deray < (t1 - t0)) break;
    }
    return 0;
}

/*人工頭脳のマップの表示
 その神経が興奮していたら'*'
 そうでなければ'-'をプリント
*/
int printAI(myAI* AI){
    int i, j;//counter
    for(i = 0; i < SIZE - 3; i++) putchar('*');
    printf(" myAI ");
    for(i = 0; i < SIZE - 3; i++) putchar('*');

    printf("\n");
    printf("\n");

    for(i = 0; i < SIZE; i++){
        for(j = 0; j < SIZE; j++){
            if(AI->neu[i][j].state & b0000_0001){//もしも興奮していたら
                printf(" *");
                    }else{
                printf("  ");
            }
        }
            printf("\n");
    }
    return 0;
}

int main(){
    srand(time(NULL));

    //myAI* AI = initAndEvolveAIWithGA();

    myAI* AI = allocAI();
    initAI(AI);
    setAIatRandom(AI);
    
    runAI(AI, 5.0);

    freeAI(AI);


    return 0;
}

