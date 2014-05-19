#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "NQueen.h"



double currentTime(){
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return tp.tv_sec + tp.tv_usec * 1.0E-6;
}

void myError(char* errorMassage){
    perror(errorMassage);
    exit(1);
}

workingMemory_t * allocWorkingMemory(){
    workingMemory_t * workingMemory = (workingMemory_t *)malloc(sizeof(workingMemory_t));
    if (workingMemory == NULL) myError("malloc VM");

    workingMemory->line = (data_t *)malloc(sizeof(data_t) * N);
    if( workingMemory->line == NULL) myError("malloc line");
    workingMemory->column = (data_t *)malloc(sizeof(data_t) * N);
    if( workingMemory->column == NULL) myError("malloc column");

    return workingMemory;
}

workingMemory_t* initWorkingMemory(workingMemory_t* workingMemory){
    if(workingMemory == NULL){
        myError("workingMemory == NULL");
    }
    
    int i;
    
    for(i = 0; i < N; i++){
        workingMemory->line[i].state = OUT;
        workingMemory->line[i].supporter = NULL;
        workingMemory->column[i].state = OUT;
        workingMemory->column[i].supporter = NULL;
    }
    
    workingMemory->goal.state = OUT;
    workingMemory->goal.supporter = NULL;
    workingMemory->nogood.state = OUT;
    workingMemory->nogood.supporter = NULL;

    workingMemory->buffer = NULL;
    
    return workingMemory;
}

chessBoard_t * allocChessBoard(){
    chessBoard_t * chessBoard = (chessBoard_t*)malloc(sizeof(chessBoard_t));
    if(chessBoard == NULL) myError("malloc chessBoard");

    chessBoard->state = (char **)malloc(sizeof(char *) * N);
    if(chessBoard->state == NULL) myError("malloc chessBoard");
    int i;
    for(i = 0; i < N; i++){
        chessBoard->state[i] = (char *)malloc(sizeof(char) * N);
        if(chessBoard->state[i] == NULL) myError("malloc chessBoard");
    }
    
    return chessBoard;
}

chessBoard_t* initChessBoard(chessBoard_t * chessBoard){
    if(chessBoard == NULL) myError("chessBoard == NULL"); 

    int i, j;
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            chessBoard->state[i][j] = 0;
        }
    }

    return chessBoard;
}

char * allocCharArray(){
    char * array = (char *)malloc(sizeof(char) * N);
    if(array == NULL) myError("malloc array");

    return array;
}
char * initCharArray(char * array){
    if(array == NULL) myError("Bad char array access");
    int i;
    for(i = 0; i < N; i++) array[i] = 0;

    return array;
}


/*あとしまつ*/
void freeWorkingMemory(workingMemory_t* workingMemory){
    //int i;
    queen_t *current, *next;
/*    for(i = 0; i < N; i++){
        current = workingMemory->line[i].supporter;
        while(current != NULL){
            next = current->next;
            free(current);
            current = next;
        }
    }

    for(i = 0; i < N; i++){
        current = workingMemory->column[i].supporter;
        while(current != NULL){
            next = current->next;
            free(current);
            current = next;
        }
    }
*/
    free(workingMemory->line);
    free(workingMemory->column);
    
    current = workingMemory->goal.supporter;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }

    current = workingMemory->nogood.supporter;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }

    freeQueenList(workingMemory->buffer);
    
    free(workingMemory);
}

void freeQueenList(queen_t * list){
    queen_t * current, * next;
    current = list;
    while(current != NULL){;
        next = current->next;
        free(current);
        current = next;
    }
}

void freeChessBoard(chessBoard_t * chessBoard){
    int i;
    for(i = 0; i < N; i++){
        free(chessBoard->state[i]);
    }
    free(chessBoard->state);
    free(chessBoard);

    
}

/*結果の表示*/
int printChessBoard(chessBoard_t * chessBoard){
    int i, j;

    //printf("\033[2J");//画面をクリア
    printf("\n");
    printf("******************\n");
    printf("*PRINT CHESSBOARD*\n");
    printf("******************\n");
    printf("\n");

    printf("  ");
    for(j = 0; j < N; j++) printf("%2d", j);
    printf("\n");
    
    for(i = 0; i < N; i++){
        printf("%2d", i);
        for(j = 0; j < N; j++){
            if(chessBoard->state[i][j] == 1) printf(" Q");
            else                       printf(" -");
        }
        printf("\n");
    }

    return 0;
}

int printChessBoardForDebug(workingMemory_t * workingMemory, chessBoard_t * chessBoard){
    int i, j;
    
    printf("\n");
    printf("  ");
    for(j = 0; j < N; j++) printf("%2d", j);
    printf("\n");
    for(i = 0; i < N; i++){
        printf("%2d", i);
        for(j = 0; j < N; j++){
            if(chessBoard->state[i][j] == 1){
                printf(" Q");
            }else if(isSafePlace(workingMemory, chessBoard, i, j) == 0 ||
                workingMemory->line[i].state == IN ||
                workingMemory->column[j].state == IN){
                printf(" *");
            }else{
                printf(" -");
            }
        }
        printf("    %d\n", workingMemory->line[i].state);
    }


    return 0;
}

/*デバック用に作成した関数*/
int testMain(){
    int i, j;//counter
    
    N = 8;
    globalCounter = 0;
    workingMemory_t* workingMemory = allocWorkingMemory();
    initWorkingMemory(workingMemory);
    chessBoard_t* chessBoard = allocChessBoard();
    initChessBoard(chessBoard);
    srand(time(NULL));
    /*ひながた
    char test0[8][8] =
        {{0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}};
    */

    char test1[8][8] =
        {{0, 0, 0, 0, 0, 0, 0, 1},
         {0, 0, 0, 0, 1, 0, 0, 0},
         {0, 0, 1, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 1, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 1, 0, 0, 0}};

    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            chessBoard->state[i][j] = test1[i][j];
        }
    }

    printChessBoard(chessBoard);

    
    setWorkingMemoryWithChessBoard(workingMemory, chessBoard);
/*    for(i = 0; i < N; i++)
        printf("workingMemory->line[i].state = %d\n", workingMemory->line[i].state);

    for(j = 0; j < N; j++)
        printf("workjngMemory->column[j].state = %d\n", workingMemory->column[j].state);
*/
    printChessBoardForDebug(workingMemory, chessBoard);
    printf("*0*\n");

    printf("3:%d\n", checkLine(workingMemory, chessBoard, 3));
    printChessBoardForDebug(workingMemory, chessBoard);

    printf("*1*\n");

    printf("6:%d\n", checkLine(workingMemory, chessBoard, 6));
    printChessBoardForDebug(workingMemory, chessBoard);

    printf("*2*\n");

    printf("5:%d\n", checkLine(workingMemory, chessBoard, 5));
    printChessBoardForDebug(workingMemory, chessBoard);

    printf("*3*\n");

    printf("%d\n", 567 % 1);

    runTMS();
    printf("Finished Running TMS in %d cycle\n", globalCounter);
    
    return 0;
}
