#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "NQueen.h"

/*ななめの関係を精査し、チェス版のi, jが安全な場所かどうか評価*/
int isSafePlace(workingMemory_t * workingMemory, chessBoard_t * chessBoard, int line, int column){
    int i, j;//counter
    int safety = 1;//そこが安全か
    queen_t * threat = NULL;//そこにななめから関与するQueen List
    queen_t * addition;
    // '\'の向きに探査
    i = line;
    j = column;
    while(i > 0 && j > 0){
        i--; j--;
    }

    while(i < N && j < N){
        if(chessBoard->state[i][j] == 1){//クイーンをみつけたら
            //ここは安全じゃない
            safety = 0;
            //脅威リストにクイーンを追加
            addition = allocQueen();
            addition->x = i;
            addition->y = j;
            addition->next = NULL;
            threat = addQueenList(threat, addition);
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
            //ここは安全じゃない
            safety = 0;
            //脅威リストにクイーンを追加
            addition = allocQueen();
            addition->x = i;
            addition->y = j;
            threat = addQueenList(threat, addition);
        }
        i++; j--;
    }    

    //将来nogoodをinにする可能性のあるqueenを保存しておく
    if(safety == 0) workingMemory->buffer = addQueenList(workingMemory->buffer, threat);
    
    return safety;
}

/*Queenをsafeにmalloc*/
queen_t * allocQueen(){
    queen_t * new = (queen_t *)malloc(sizeof(queen_t));
    if(new == NULL) myError("malloc Queen");
    new->next = NULL;
    return new;
}

/*QueenListに、Queen(List)を追加*/
queen_t * addQueenList(queen_t * target, queen_t * addition){
    queen_t *current, *next;

    current = target;

    if(current == NULL) return addition;
    else next = current->next;
    
    while(next != NULL){
        current = next;
        next = current->next;
    }
    current->next = addition;
    return target;
}

/*あるLineをINにできるか調べる。可能であればLineをINにする。不可能であれば,nogoodがINとなる*/
int checkLine(workingMemory_t * workingMemory, chessBoard_t * chessBoard, int currentLine){
    int i;//counter
    int safeColumn;
    int dice;
    int currentColumn;
    char * isSafeColumn = allocCharArray();
    initCharArray(isSafeColumn);
    queen_t * addition = NULL;

    //for(i = 0; i < N; i++) printf(" %d", isSafeColumn[i]);
    //printf("\n");
    
    //そのラインの中で安全なColumnを把握(縦の重なりを排除)
    safeColumn = 0;
    for(i = 0; i < N; i++){
        if(workingMemory->column[i].state == IN){
            isSafeColumn[i] = 0;
        }else{
            isSafeColumn[i] = 1;
            safeColumn++;
        }
    }

    //ななめの重なりをチェック
    for(currentColumn = 0; currentColumn < N; currentColumn++){
        if(isSafeColumn[currentColumn]){
            //ななめの重なりを発見したら
            if(!isSafePlace(workingMemory, chessBoard, currentLine, currentColumn)){
                isSafeColumn[currentColumn] = 0;
                safeColumn--;
            }
        }
    }

//    for(i = 0; i < N; i++) printf(" %d", isSafeColumn[i]);
//    printf("\n");
        
    //printf("line %d: number of safe column: %d\n", currentLine, safeColumn);

    if(safeColumn < 0) myError("error: checklineで安全な列を調べるのに失敗");
    //もしも安全な列が残っていなかったら
    if(safeColumn == 0){
        workingMemory->nogood.state = IN;
        workingMemory->nogood.supporter = workingMemory->buffer;
        workingMemory->buffer = NULL;
        return 0;
    }
    //もしも安全な列が残っていたら、ランダムに列を選択して、そこにQueenを入れる
    else{
        dice = rand() % safeColumn;
        for(currentColumn = 0; currentColumn < N; currentColumn++){
            if(isSafeColumn[currentColumn]){
                if(dice == 0){
                    //Queenの挿入
                    chessBoard->state[currentLine][currentColumn] = 1;
                    workingMemory->line[currentLine].state = IN;
                    workingMemory->column[currentColumn].state = IN;
                    //workingMemoryのセット
                    addition = allocQueen();
                    addition->x = currentLine;
                    addition->y = currentColumn;
                    addition->next = NULL;
                    workingMemory->line[currentLine].supporter = addition;

                    addition = allocQueen();
                    addition->x = currentLine;
                    addition->y = currentColumn;
                    addition->next = NULL;
                    workingMemory->column[currentColumn].supporter = addition;

                    break;
                }else{
                    dice--;
                }
            }
        }
    }

    //過去の遺物
    /* if(isSafePlace(workingMemory, chessBoard, currentLine, currentColumn)){ */
    /*     //そこにQueenをおく */
    /*     chessBoard->state[currentLine][currentColumn] = 1; */
    /*     //そのline, ColumnデータをINに */
    /*     workingMemory->line[currentLine].state = IN; */
    /*     workingMemory->column[currentColumn].state = IN; */
    /*     //そのlineをINにしているデータとして、置いたqueenを追加 */
    /*     addition = allocQueen(); */
    /*     addition->x = currentLine; */
    /*     addition->y = currentColumn; */
    /*     workingMemory->line[currentLine].supporter = addition; */
    /*     break; */
    /* } */
    /* //もしもななめの重なりがあって安全でなかったら */
    /* else{ */
    /*     isSafeColumn[currentColumn] = 0; */
    /*     safeColumn--; */
    /*     //もしももうQueenがおけなくなっていたら */
    /*     if(safeColumn == 0){ */
    /*         //nogood! */
    /*         workingMemory->nogood.state = IN; */
    /*         //nogoodがおこってしまったので、bufferの因子をnogoodのsupporterに代入 */
    /*         workingMemory->nogood.supporter = workingMemory->buffer; */
    /*         workingMemory->buffer = NULL; */
    /*         return 0; */
    /*     } */
    /* } */
    
        

    //if(isSafeColumn != NULL) free(isSafeColumn);
    //nogoodは起こらなかったので、bufferは解放
    //freeQueenList(workingMemory->buffer);

    free(isSafeColumn);
    return 1;
}

/*chessBoardを読み込んでworkingMemoryをセット デバック用の関数*/
workingMemory_t * setWorkingMemoryWithChessBoard(workingMemory_t * workingMemory, chessBoard_t * chessBoard){
    if(workingMemory == NULL || chessBoard == NULL) myError("setWorkingMemory");
    int i, j;
    initWorkingMemory(workingMemory);
    queen_t * addition = NULL;
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            if(chessBoard->state[i][j] == 1){
                workingMemory->line[i].state = IN;
                workingMemory->column[j].state = IN;
                addition = allocQueen();
                addition->x = i;
                addition->y = j;
                workingMemory->line[i].supporter =
                    addQueenList(workingMemory->line[i].supporter, addition);
                workingMemory->column[j].supporter =
                    addQueenList(workingMemory->column[j].supporter, addition);
            }
        }
    }
    
    return 0;
}

/*メイン*/
int runTMS(){
    int i, j, k;//counter
    int INLine;
    //int INColumn;
    /*初期化*/
    workingMemory_t* workingMemory = allocWorkingMemory();
    initWorkingMemory(workingMemory);
    chessBoard_t* chessBoard = allocChessBoard();
    initChessBoard(chessBoard);
    
    srand(time(NULL));

    /*Queenをおける位置を探し、Queenをセット*/
    int dice;//さいころ
    int currentLine;
    //int currentColumn;

    while(workingMemory->goal.state == OUT){
        for(i = 0; i < N; i++){
            if(workingMemory->line[i].state == IN) k++;
        }
        INLine = k;
        if(INLine == N) myError("all line is IN");
        
        if((INLine > 0) &&
            ((rand() % N) == 0)){
           
            dice = rand() % INLine;
            k = 0;
            for(i = 0; i < N; i++){
                if(workingMemory->line[i].state == IN){
                    if(dice == k){
                        /*remove a Queen*/
                        j = 0;
                        while(workingMemory->column[j].state == OUT) j++;
                        chessBoard->state[i][j] = 0;
                        workingMemory->line[i].state = OUT;
                        workingMemory->column[j].state = OUT;
                        free(workingMemory->line[i].supporter);
                        workingMemory->line[i].supporter = NULL;
                        workingMemory->column[j].supporter = NULL;
                        INLine--;
                        if(mode == nomal) printChessBoard(chessBoard);
                        break;
                    }else{
                        k++;
                    }
                }
            }
        }
        /*******************************
        ^無限ループ対策 Queenが1つ以上ある時
        一定の確率で、適当なQueenをぬきとる
        ********************************/
         
        // globalCounter++;
        
        //if(mode == nomal) printChessBoard(chessBoard);
        k = 0;
        //OUTであるlineをランダムに選出
        dice = rand() % (N - INLine);
        k = 0;
        //OUTであるLineのうちdice番目のものをcurrentLineに
        for(i = 0; i < N; i++){
            if(workingMemory->line[i].state == OUT){
                if(k == dice) break;
                else{
                    k++;
                }
            }
        }
        if(i == N) myError("line selection failed");

        currentLine = i;//カウンタ増やす
//        printf("before check\n");
        //そのLineをcheck
        if(checkLine(workingMemory, chessBoard, currentLine)){
//            printf("after check\n");
            //そのlineに無事にqueenをおけたら
            globalCounter++;
            if(mode == nomal) printChessBoard(chessBoard);
            //INであるlineを集計
            for(i = 0; i < N; i++){
                if(workingMemory->line[i].state == OUT) break;
            }
            if(i == N){
                //goalが達成されている
                workingMemory->goal.state = IN;
                for(i = 0; i < N; i++){
                    workingMemory->goal.supporter = addQueenList(workingMemory->goal.supporter, workingMemory->line[i].supporter);
                }
            }
            //goalが達成されていなければ、whileループをまわる
        }else{
            //printf("nogood\n");
            //そのlineに無事にqueenをおけなかったら
            //nogood状態が起こってしまっているので、そのlineにqueenをおくためにnogoodの要因の中からランダムにqueenをとりだして、盤上から取り除く
            //queenをおけたら、nogoodをOUTにする
            int sup;
            queen_t *current;
            queen_t *prev = NULL;
            while(1){
                current = workingMemory->nogood.supporter;
                sup = 0;
                while(current != NULL){
                    sup++;
                    current = current->next;
                }
                dice = rand() % sup;
                current = workingMemory->nogood.supporter;
                while(dice != 0){
                    prev = current;
                    current = current->next;
                    dice--;
                }
                //queenを取り除く
                if(prev != NULL) prev->next = current->next;//currentをぬきとる準備
                if(chessBoard->state[current->x][current->y] == 0){
                    free(current);
                    current = NULL;
                }else{
                    chessBoard->state[current->x][current->y] = 0;
                    workingMemory->line[current->x].state = OUT;
                    workingMemory->column[current->y].state = OUT;
                    freeQueenList(workingMemory->line[current->x].supporter);
                    workingMemory->line[current->x].supporter = NULL;
                    freeQueenList(workingMemory->column[current->y].supporter);
                    workingMemory->column[current->y].supporter = NULL;
                    if(mode == nomal) printChessBoard(chessBoard);
                    globalCounter++;//カウンタ増やす
                    //free(current);
                    current = NULL;
                    if(checkLine(workingMemory, chessBoard, currentLine)){
                        if(mode == nomal) printChessBoard(chessBoard);
                        globalCounter++;//カウンタ増やす
                        break;
                    }
                }
            }
            //Queenをおけたので、nogoodをoutにする
            workingMemory->nogood.state = OUT;
            freeQueenList(workingMemory->nogood.supporter);
            workingMemory->nogood.supporter = NULL;
        }
        //while loopをまわる
    }

    printf("%d: Running TMS finished in %d steps\n", N, globalCounter);
    
    /*あとしまつ*/
    freeChessBoard(chessBoard);
    freeWorkingMemory(workingMemory);
   
    return 0;
}


