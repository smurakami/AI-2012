#include "GAI.h"

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
  ^最終的に、ニューロン同士のつながりを有向グラフから無向グラフにすることでこの問題に対処した。

  4.において、個体を選ぶためにルーレット選択を行った。
  ルーレット選択とは、個体iの評価値をf_iとおいたとき、個体iを選ぶ確率をp_iと置いたとき、
  p_i = \frac{f_i}{\sum_{k = 1}^{N}f_k}
  とする方法である。
  f_iは正の数である必要があるため、最小値の絶対値をプラスすることで下駄をはかせる。

  ^ルーレット選択ではきちんとした収束ができなかったので、
  ランキング選択に変更する。
  まず最初にランクつけをし、下から数えた順位の二乗を点数とする（収束を早めるため）
  そして、その点数をもとにルーレット選択を実施する。
  また、上位10%を保存するエリート保存も実施する。

  
*/


/*AIの確保、初期化、進化をさせる関数*/
myAI* initAndEvolveAIWithGA(point * retina, point * dest){
  int gen;
  int i;
  myAI** currentGeneration = (myAI **)calloc(sizeof(myAI *), G_SIZE);
  myAI** nextGeneration = (myAI **)calloc(sizeof(myAI *), G_SIZE);
  myAI** tmp;
  myAI * elete;

  /*1*/
  initGeneration(currentGeneration);
  initGeneration(nextGeneration);

  /*2*/
  setFirstGeneration(currentGeneration, retina, dest);
  setFirstGeneration(nextGeneration, retina, dest);//網膜と受容野の情報の設定

  
  for(gen = 0; gen < G_TIMES; gen++){
    /*3*/
    valueOfAIinGeneration(currentGeneration);
    /*4, 5*/
    selectAI(currentGeneration, nextGeneration);

    /*6*/
    tmp = currentGeneration;
    currentGeneration = nextGeneration;
    nextGeneration = tmp;

    elete = currentGeneration[indexOfEleteAI(currentGeneration)];
    initRetina(elete);
    runAIforGA(elete);
    printf("generation: %d\n", gen);
    printf("value: %d\n", elete->value);
    printAIforGA(elete);
    cvWaitKey(0);

    for(i = 0; i < G_SIZE; i ++){
      initRetina(currentGeneration[i]);
      initRetina(nextGeneration[i]);
    }
  }
  
  elete = currentGeneration[indexOfEleteAI(currentGeneration)];
  runAIforGA(elete);
  printAIforGA(elete);
  cvWaitKey(0);
  
  
  freeGeneration(currentGeneration);
  freeGeneration(nextGeneration);
    
  return NULL;
}

/*最初の世代をランダムにセット*/
int setFirstGeneration(myAI* generation[G_SIZE], point * retina, point * dest){
  int i;//counter
  for(i = 0; i < G_SIZE; i++){
    setAIatRandom(generation[i]);
    generation[i]->retina = retina;
    generation[i]->dest = dest;
  }
  return 0;
}

/*一つの世代に対して評価を実行*/
int valueOfAIinGeneration(myAI * generation[G_SIZE]){
  int i;
  for(i = 0; i < G_SIZE; i++){
    valueOfAI(generation[i]);
  }
  return 0;
}

//現世代から次世代にうつすAIを選び出す
int selectAI(myAI* currentGeneration[G_SIZE], myAI* nextGeneration[G_SIZE]){
  int i, j, k;
  int n;//次世代にコピーされた現世代のAIの個数
  int dice;//乱数につかう
  int f[G_SIZE];//再計算された評価値
  int index[G_SIZE];//ソートのためのインデックス
  int min = currentGeneration[0]->value;
  int sum;//総和の管理に使う
  int valueSum;//分母
  myAI * A1, *A2;//交叉に使う人工頭脳

  /* for(i = 0; i < G_SIZE; i++) */
  /*   if(min > currentGeneration[i]->value) min = currentGeneration[i]->value; */
  /* min--; */
  /* for(i = 0; i < G_SIZE; i++) */
  /*   f[i] = currentGeneration[i]->value - min; */

  /* //ルーレット選択の為にそれぞれのAIの評価値の総和を計算 */
  /* valueSum = 0; */
  /* for(i = 0; i < G_SIZE; i++) valueSum += f[i]; */
  
  /* n = 0; */

  //ランクづけをする
  rankAI(currentGeneration, index);
  for(i = 0; i < G_SIZE; i++){
    printf("%d : %d\n", i, currentGeneration[index[i]]->value);
  }

  for(i = 0; i < G_SIZE; i++){
    f[i] = index[i]*index[i];
  }
  valueSum = G_SIZE*(G_SIZE - 1)*(2*G_SIZE - 1)/6;

  //エリート選出
  for(n = 0; n < G_SIZE / 10; n++){
    copyNeuron(nextGeneration[n], currentGeneration[index[G_SIZE-1 - n]]);
  }
  
  while(n < G_SIZE){
    if(rand() % 1000 < G_MUTE){
            dice = rand() % valueSum;
      sum = 0;
      i = 0;
      while(sum <= dice){
        sum += f[i];
        i++;
      }
      i--;
      mutateAI(currentGeneration[i], nextGeneration[n]);//突然変異
    }
    else{
      //交叉する
      //交叉に使うAIを二つ選択
      dice = rand() % valueSum;
      sum = 0;
      i = 0;
      while(sum <= dice){
        sum += f[i];
        i++;
      }
      i--;
      //printf("A1: i = %d\n", i);
      if(i >= G_SIZE){
        fprintf(stderr, "buffer over flow: i = %d\n", i);
        exit(1);
      }

      A1 = currentGeneration[i];
      
      dice = rand() % valueSum;
      sum = 0;
      i = 0;
      while(sum <= dice){
        sum += f[i];
        i++;
      }
      i--;
      //printf("A2: i = %d\n", i);
      if(i >= G_SIZE){
        fprintf(stderr, "buffer over flow: i = %d\n", i);
        exit(1);
      }
      A2 = currentGeneration[i];
      
      crossAI(A1, A2, nextGeneration[n]);//交叉
    }
    n++;
  }
  
  return 0;
}

//突然変異させる
int mutateAI(myAI * currentAI, myAI * nextAI){
  myAI * mutant = allocAI();
  initAI(mutant);
  setAIatRandom(mutant);//ランダムなAIを生成
  initNeuron(mutant);

  crossAI(currentAI, mutant, nextAI);//ランダムなAIと選んだAIを交叉
  initRetina(nextAI);
  return 0;
}

//一様交叉させる
int crossAI(myAI * A1, myAI * A2, myAI * next){
  int i, j;
  for(i = 0; i < SIZE; i++){
    for(j = 0; j < SIZE; j++){
      if(rand()%2) next->neu[i][j].state = A1->neu[i][j].state;
      else next->neu[i][j].state = A2->neu[i][j].state;
    }
  }
  
  return 0;
}

//一番優秀なAIを見つけ出す
int indexOfEleteAI(myAI** currentGeneration){
  int index;
  int i;
  int max = currentGeneration[0]->value;
  index = 0;
  for(i = 0; i < G_SIZE; i++){
    if(currentGeneration[i]->value > max){
      max = currentGeneration[i]->value;
      index = i;
    }
  }

  return index;
}

int rankAI(myAI** currentGeneration, int * index){
  int i, j;
  int value[G_SIZE];
  int tmp;
  for(i = 0; i < G_SIZE; i++){
    value[i] = currentGeneration[i]->value;
    index[i] = i;
  }
  for(i = G_SIZE; i > 0; i--){
    for(j = 0; j < i - 1; j++){
      if(value[j] > value[j+1]){
        tmp = value[j];
        value[j] = value[j+1];
        value[j+1] = tmp;
        
        tmp = index[j];
        index[j] = index[j+1];
        index[j+1] = tmp;
      }
    }
  }

  return 0;
}
