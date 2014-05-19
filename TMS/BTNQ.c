/*

  単純なバックトラックによるNQueen問題の解法
  
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "NQueen.h"

int runBT(){
    /*初期化*/
    chessBoard_t * chessBoard = allocChessBoard();
    initChessBoard(chessBoard);

    int BTreturn = BT(chessBoard, 0);

    if(BTreturn == 0) printf("%d: BackTrack finished in %d steps\n", N, globalCounter);
    else printf("%d: Back Track failed: steps = %d\n", N, globalCounter);
    
    freeChessBoard(chessBoard);
    return 0;
}

int BT(chessBoard_t* chessBoard, int line){
    int column;
    int BTreturn;
    if(globalCounter > maxBT) return -1;//探索に時間がかかり過ぎなので中止!!
    if(line == N) return 0;
    for(column = 0; column < N; column++){
        if(BTisSafePlace(chessBoard, line, column)){
            chessBoard->state[line][column] = 1;
            if(mode == nomal) printChessBoard(chessBoard);
            globalCounter++;
            BTreturn = BT(chessBoard, line + 1);
            if(BTreturn == 1){
                chessBoard->state[line][column] = 0;
                if(mode == nomal) printChessBoard(chessBoard);
                globalCounter++;
            }
            else if(BTreturn == 0) return 0;
            else if(BTreturn == -1) return -1;
        }
    }
    return 1;
}

int BTisSafePlace(chessBoard_t * chessBoard, int line, int column){
    if(BTrookSafe(chessBoard, line, column) &&
       BTbishopSafe(chessBoard, line, column))
        return 1;
    else
        return 0;
}

int BTrookSafe(chessBoard_t * chessBoard, int line, int column){
    int i, j;
    j = column;
    for(i = 0; i < N; i++){
        if(chessBoard->state[i][j] == 1) return 0;
    }
    i = line;
    for(j = 0; j < N; j++){
        if(chessBoard->state[i][j] == 1) return 0;
    }
    return 1;
}
int BTbishopSafe(chessBoard_t * chessBoard, int line, int column){
    int i, j;//counter
    // '\'の向きに探査
    i = line;
    j = column;
    while(i > 0 && j > 0){
        i--; j--;
    }

    while(i < N && j < N){
        if(chessBoard->state[i][j] == 1){//クイーンをみつけたら
            return 0;
        }
        i++; j++;
    }    

    // '/'の向きに探査
    i = line;
    j = column;
    while(i > 0 && j < N - 1){
        i--; j++;
    }

    while(i < N && j >= 0){
        if(chessBoard->state[i][j]){//クイーンをみつけたら
            return 0;
        }
        i++; j--;
    }    

    return 1;    
}

