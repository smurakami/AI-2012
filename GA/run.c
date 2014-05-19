#include "GAI.h"

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
  point * current = AI->retina;
    
  double startTime = currentTime();
  double t0 = currentTime();
  printAI(AI);

  while((currentTime() - startTime) < runTime){
//    printf("OK\n");
    /*神経細胞間で情報を伝達*/
    communicate1(AI);        
    communicate2(AI);

    //一定時間ごとに網膜からパルスを受け取る
    if((currentTime() - t0) > 1.0/FREQ){
        while(current->next != NULL){
          current = current->next;
          AI->neu[current->x][current->y].state = AI->neu[current->x][current->y].state | b0000_0001;
        }
      t0 = currentTime();
    }

    
    //clearScreen();
    printAI(AI);
    waitAI();//フレームレートの調節
  }

  initNeuron(AI);

  return 0;
}

/*遺伝的アルゴリズムで使う用の人工知能稼働*/
int runAIforGA(myAI* AI){
  int i;//counter
  double t0 = currentTime();
  point * current = AI->retina;
  while(current->next != NULL){
    current = current->next;
    AI->neu[current->x][current->y].state = AI->neu[current->x][current->y].state | b0000_0001;
  }
  for(i = 0; i < SIZE*4; i++){
    /*神経細胞間で情報を伝達*/
    communicate1(AI);
    communicate2(AI);
      
    //clearScreen();
  }

  initNeuron(AI);
        
  return 0;
}

/*人口頭脳のシミュレーションのフレームレート調整*/
int waitAI(){
  double t0, t1;
  int delay = 1000 / FRAME_RATE;//delay[ms]

  cvWaitKey(delay);
  
  return 0;
}
