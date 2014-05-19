#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "mask.h"

void myError(char* errorMessage){
  perror(errorMessage);
  exit(1);
}

/************************************
    覆面算の問題本体のセット関連関数群
 ************************************/

/*覆面算の本体を確保して初期化*/
mask_t * initMask(int size){
  int i, j;//counter
  mask_t * mask = (mask_t *)malloc(sizeof(mask_t));
  if(mask == NULL) {
    perror("malloc mask");
    exit(1);
  }
  mask->column = (column_t *)malloc(sizeof(column_t) * size);
  for(i = 0; i < size; i++){
    for(j = 0; j < 3; j++){
      mask->column[i].c[j] = 0;
      mask->column[i].i[j] = -1;
    }
    mask->column[i].carry = 0;
  }
  mask->size = size;
  for(i = 0; i < 3; i++){
    mask->nogoodSup[i] = 0;
  }

  return mask;
}


/*アルファベットの状態を記憶する構造体を確保*/
characterList_t * mallocCharacterList(){
  characterList_t * characterList = (characterList_t *)malloc(sizeof(characterList_t));
  if(characterList == NULL){
    perror("malloc characterList");
    exit(1);
  }
  characterList->c = 0;//未定義の印
  characterList->num = -1;//未定義の印
  characterList->usedInUpperSide = 0;
  characterList->next = NULL;
  return characterList;
}

/*リストを確認して、その中に該当するアルファベットが存在しなかったら、リストに加える*/
characterList_t * addCharacterList(characterList_t * head, char c){
  int setted;
  characterList_t * current = head;
  setted = 0;
  if(!isalpha(c)) return head;
  while(current->next != NULL){
    current = current->next;
    if(current->c == c){
      setted = 1;
      break;
    }
  }
  if(setted == 0){
    current->next = mallocCharacterList();
    current->next->c = c;
  }
  return head;
}

/*覆面算中に現れるアルファベットをリスト構造に格納*/
characterList_t * setCharacterList(mask_t * mask){
  int i, j;
  characterList_t * head = mallocCharacterList();
  head->c = -1;//先頭の印
  for(i = 2; i >=0; i--){
    for(j = 0; j < mask->size; j++){
      addCharacterList(head, mask->column[j].c[i]);
    }
  }

  return head;
}

/*上段で使われている文字を把握*/
characterList_t * setUpperSide(characterList_t * head, mask_t * mask){
  characterList_t * current;
  int i, j;
  for(i = 2; i > 0; i--){
    for(j = 0; j < mask->size; j++){
      current = head;
      while(current->next != NULL){
        current = current->next;
        if(current->c == mask->column[j].c[i]){
          current->usedInUpperSide = 1;
          break;
        }
      }
    }
  }
  return head;
}
/*numberListを確保*/
numberList_t * mallocNumberList(){
  numberList_t * numberList = (numberList_t *)malloc(sizeof(numberList_t));
  if(numberList == NULL){
    perror("malloc numberList");
    exit(1);
  }
  return numberList;
}
/*numberListを初期化*/
numberList_t * initNumberList(numberList_t * numberList){
  int i;
  for(i = 0; i < 10; i++){
    numberList->c[i] = 0;
    numberList->isUsed[i] = 0;
  }
  numberList->unUsed = 10;
  return numberList;
}
/*numberListをセット*/
numberList_t * setNumberList(characterList_t * head){
  characterList_t * current = head;
  numberList_t * numberList = mallocNumberList();
  initNumberList(numberList);
  /*while(current->next != NULL){
    current = current->next;
    if((current->num == -1) && (current->usedInUpperSide == 1)){
      setNumAtRandom(current, numberList);
    }
    }*/
  return numberList;
}

/*numberListとcharacterListを参照して、ランダムに空いてる数字をいれる*/
int setNumAtRandom(characterList_t * current, numberList_t * numberList){
  int i, k;
  int dice;
  dice = rand()%numberList->unUsed;
  k = 0;
  for(i = 0; i < 10; i++){
    if(numberList->isUsed[i] == 0){
      if(k == dice){
        current->num = i;
        numberList->isUsed[i] = 1;
        numberList->c[i] = current->c;
        numberList->unUsed--;
        break;
      }
      k++;
    }
  }
  if(current->num == -1) myError("failed setting num at random");

  return 0;
}

/*各文字に割り当てられた数字をもとに、覆面算の本体の数字をセット*/
int setMaskWithNum(characterList_t * characterList, mask_t * mask){
  int i, j;
  characterList_t * current;
  for(i = 2; i >= 0; i--){
    for(j = mask->size - 1; j >= 0; j--){
      if(!isalpha(mask->column[j].c[i])) continue;
      current = characterList;
      while(current->next != NULL){
        current = current->next;
        if(current->c == mask->column[j].c[i]){
          mask->column[j].i[i] = current->num;
          break;
        }
      }
    }
  }
  
  return 0;
}

/************************************
        覆面算を解く関数群
 ************************************/
/*覆面算の繰り上がり情報をリセット*/
int initCarry(mask_t * mask){
  int i;
  for(i = 0; i < mask->size; i++) mask->column[i].carry = 0;
  return 0;
}

/*characterListから、あるアルファべットを探す*/
characterList_t * searchChar(characterList_t * head, char c){
  characterList_t * current = head;
  while(current->next != NULL){
    current = current->next;
    if(current->c == c){
      return current;
    }
  }
  return NULL;
}

/*ある列の上段の、数字が不定なアルファベットに数字をランダムにセット*/
/* '*'の場合は、不定のままにしておく */
int setColumnAtRandom(mask_t * mask, characterList_t * characterList, numberList_t * numberList, int column){
  column_t * currentColumn = mask->column;
  int i;
  characterList_t * search;
  for(i = 2; i > 0; i--){
    if(currentColumn[column].i[i] == -1){
      if(currentColumn[column].c[i] == '*') continue;
      search = searchChar(characterList, currentColumn[column].c[i]);
      if(search == NULL) continue;
      setNumAtRandom(search, numberList);
      currentColumn[column].i[i] = search->num;
    }
  }
  return 0;
} 

/*ある特定の文字に特定の数字を割当*/
int setNumToChar(characterList_t * characterList, numberList_t * numberList, char c, int num){
  characterList_t * search = searchChar(characterList, c);
  search->num = num;
  numberList->isUsed[num] = 1;
  numberList->c[num] = c;
  numberList->unUsed--;

  return 0;
}

/*ある特定の文字から数字を除外*/
int removeNumFromChar(characterList_t * characterList, numberList_t * numberList, char c){
  characterList_t * search = searchChar(characterList, c);
  if(search == NULL) myError("failed searching for remove");
  int num  = search->num;
  if(num == -1) return 1;//いちおう異常終了
  numberList->isUsed[num] = 0;
  numberList->c[num] = 0;
  numberList->unUsed++;
  search->num = -1;

  return 0;
}

/*その列で計算が成立しているかcheck 下段に数字が割り当てられていなければ新しい数字を割り当てる
  数字の割当が不可能となったらnogoodを返す*/
/*まずは、上段に'*'が無い前提で書く。*/
int checkColumn(mask_t * mask, characterList_t * characterList, numberList_t * numberList, int column){
  int i;
  column_t currentColumn = mask->column[column];
  /*上段が両方とも埋まっている場合*/
  //上段の和を計算  
  int sum;
  if(column < mask->size-1){//最上位でない場合
    sum = currentColumn.i[2] + currentColumn.i[1] + currentColumn.carry;
  }
  else{//最上位の場合
    sum = currentColumn.carry;
  }
  //下段が不定の場合
  if((currentColumn.i[0] == -1) ||(currentColumn.c[0] == '*')){
    //下段に数字を割当てられるか検証
    if(numberList->isUsed[sum%10] == 1){//割当に失敗
      mask->nogoodSup[0] = currentColumn.c[2];
      mask->nogoodSup[1] = currentColumn.c[1];
      return -1;//return nogood;
    }
    //割当に成功したら
    setNumToChar(characterList, numberList, currentColumn.c[0], sum%10);
    currentColumn.i[0] = sum % 10;
    if(column < mask->size) mask->column[column+1].carry = sum / 10;//桁上がり
    return 1;//成功
  }else{//下段が確定している場合
    if(currentColumn.c[0] == sum%10){
      if(column < mask->size) mask->column[column+1].carry = sum / 10;//桁上がり
      return 1;//成功
    }else{
      for(i = 0; i < 3; i++){
        mask->nogoodSup[i] = currentColumn.c[i];//その列すべて、悪い要因になる
      }
      return -1;//return nogood
    }
  }
  myError("check column failed");//ここに処理がくる事はない
  return 0;
}

/*nogoodが起きたときに、その原因を取り除く*/
int removeNogoodSup(mask_t * mask, characterList_t * characterList, numberList_t * numberList){
  int i, j;
  char c;
  for(i = 0; i < 3; i++){
    if(isalpha(c = mask->nogoodSup[i])){
      removeNumFromChar(characterList, numberList, c);
      setMaskWithNum(characterList, mask);
    }
  }
  return 0;//正常終了
}

/*覆面算を解く*/
int solveMask(mask_t * mask, characterList_t * characterList, numberList_t * numberList){
  int i;
  int check;
  while(1){
    initCarry(mask);
    for(i = 0; i < mask->size; i++){
      setColumnAtRandom(mask, characterList, numberList, i);
      check = checkColumn(mask, characterList, numberList, i);
      setMaskWithNum(characterList, mask);
      //printMaskWithChar(mask);
      printMaskWithNum(mask);
      if(check == -1){//nogoodが起こったら
        while(check == -1){//列の割当に失敗しているあいだ
          removeNogoodSup(mask, characterList, numberList);
          setMaskWithNum(characterList, mask);        
          //printMaskWithChar(mask);
          printMaskWithNum(mask);
          
          setColumnAtRandom(mask, characterList, numberList, i);
          setMaskWithNum(characterList, mask);
          //printMaskWithChar(mask);
          printMaskWithNum(mask);
          
          check = checkColumn(mask, characterList, numberList, i);
          setMaskWithNum(characterList, mask);
          //printMaskWithChar(mask);
          printMaskWithNum(mask);
        }
        break;
      }
    }
    if(i == mask->size) break;//計算完了
    
  }
  return 0;
}

/******************************
        結果表示用関数群
 ******************************/

void printCharacterList(characterList_t * characterList){
    characterList_t * current = characterList;
  while(current->next != NULL){
    current = current->next;
    printf("%c : %d\n", current->c, current->num);
  }
}

void printNumberList(numberList_t * numberList){
  int i;
  for(i = 0; i < 10; i++){
    printf("%d : isUsed = %d, c = %c\n", i, numberList->isUsed[i], numberList->c[i]);  
  }
  printf("unUsed num : %d\n", numberList->unUsed);
}

void printMaskWithChar(mask_t * mask){
  int i, j;
  for(i = 2; i > 0; i--){
    for(j = mask->size - 1; j >= 0; j--){
      if((isalnum(mask->column[j].c[i])) || (mask->column[j].c[i] == '*')){
        putchar(mask->column[j].c[i]);     
      }else{
        if((i == 1) && (j == mask->size - 1)) putchar('+');
        else putchar(' ');
      }
    }
    putchar('\n');
  }
  for(j = 0; j < mask->size; j++) putchar('-');
  putchar('\n');
  for(j = mask->size - 1; j >= 0; j--){
    if((isalnum(mask->column[j].c[i])) || (mask->column[j].c[i] == '*')){
      putchar(mask->column[j].c[i]);     
    }else{
      if((i == 1) && (j == mask->size - 1)) putchar('+');
      else putchar(' ');
    }
  }
  putchar('\n');
  putchar('\n');
  
}
void printMaskWithNum(mask_t * mask){
  int i, j;
  for(i = 2; i > 0; i--){
    for(j = mask->size - 1; j >= 0; j--){
      if((isalnum(mask->column[j].c[i])) || (mask->column[j].c[i] == '*')){
        if(mask->column[j].i[i] == -1) putchar('?');
        else printf("%d", mask->column[j].i[i]);
      }else{
        if((i == 1) && (j == mask->size - 1)) putchar('+');
        else putchar(' ');
      }
    }
    putchar('\n');
  }
  for(j = 0; j < mask->size; j++) putchar('-');
  putchar('\n');
  for(j = mask->size - 1; j >= 0; j--){
    if((isalnum(mask->column[j].c[i])) || (mask->column[j].c[i] == '*')){
      if(mask->column[j].i[i] == -1) putchar('?');
      else printf("%d", mask->column[j].i[i]);
    }else{
      if((i == 1) && (j == mask->size - 1)) putchar('+');
      else putchar(' ');
    }
  }
  putchar('\n');
  putchar('\n');
  
}
int main(){
  srand(time(NULL));
  mask_t * mask = initMask(5);
  mask->column[4].c[2] = 0;
  mask->column[3].c[2] = 'S';
  mask->column[2].c[2] = 'E';
  mask->column[1].c[2] = 'N';
  mask->column[0].c[2] = 'D';

  mask->column[4].c[1] = 0;
  mask->column[3].c[1] = 'M';
  mask->column[2].c[1] = 'O';
  mask->column[1].c[1] = 'R';
  mask->column[0].c[1] = 'E';

  mask->column[4].c[0] = 'M';
  mask->column[3].c[0] = 'O';
  mask->column[2].c[0] = 'N';
  mask->column[1].c[0] = 'E';
  mask->column[0].c[0] = 'Y';
  
  characterList_t * characterList = setCharacterList(mask);
  setUpperSide(characterList, mask);
  numberList_t * numberList = mallocNumberList();
  initNumberList(numberList);
  setMaskWithNum(characterList, mask);

  solveMask(mask, characterList, numberList);
  
  //試しに一行ランダムにセットして見る。
  /* setColumnAtRandom(mask, characterList, numberList, 0); */
  /* setMaskWithNum(characterList, mask); */
  /* int check; */
  /* check = checkColumn(mask, characterList, numberList, 0); */
  /* setMaskWithNum(characterList, mask); */
  /* while(check == -1){ */
  /*   printMaskWithNum(mask); */
    
    
  /*   removeNogoodSup(mask, characterList, numberList); */
  /*   setMaskWithNum(characterList, mask); */

  /*   printMaskWithNum(mask); */
    
  /*   setColumnAtRandom(mask, characterList, numberList, 0); */
  /*   setMaskWithNum(characterList, mask); */
    
  /*   check = checkColumn(mask, characterList, numberList, 0); */
  /*   setMaskWithNum(characterList, mask); */
  /* } */
  
  /* printCharacterList(characterList); */
  /* printNumberList(numberList); */
  /* putchar('\n'); */
  /* printMaskWithChar(mask); */
  /* putchar('\n'); */
  /* printMaskWithNum(mask); */
  
  return 0;
}
