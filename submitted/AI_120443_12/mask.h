/***************************************
 ****制約充足の手段を用いた覆面算の解法******
 ***************************************/

/*
   120443 村上晋太郎
 */

/*
  このプログラムでは、制約充足の手段を用いて、覆面算を解く。
  今回は、三つの構造体を用意した。
  - struct mask
    問題の本体を保管する.
  - struct caractorList

  - struct numberList
  
*/

/************************************
    覆面算の問題本体のセット関連関数群
 ************************************/

/*覆面算の問題の列*/
typedef struct column{
  char c[3];//列 下から順に
  int i[3];//行 下から順に
  int carry;//下からの桁上がりの有無
} column_t;

/*覆面算の問題*/
typedef struct mask{
  column_t * column;
  int size;
  char nogoodSup[3];//nogoodを引き起こしている文字のリスト
} mask_t;

/*size列の覆面算の問題を確保*/
mask_t * initMask(int size);

/*アルファベットの状態を記憶する構造体*/
typedef struct characterList{
  char c;
  int num;
  int usedInUpperSide;//上段で使われているかどうかの判定
  struct characterList * next;
} characterList_t;

/*アルファベットの状態を記憶する構造体を確保*/
characterList_t * mallocCharacterList();
/*覆面算中に現れるアルファベットをリスト構造に格納*/
characterList_t * setCharacterList(mask_t * mask);
/*リストを確認して、その中に該当するアルファベットが存在しなかったら、リストに加える*/
characterList_t * addCharacterList(characterList_t * head, char c);

/*数字の状態を記憶する構造体*/
typedef struct numberList{
  int isUsed[10];
  char c[10];
  int unUsed;
} numberList_t;

/*numberListを確保*/
numberList_t * mallocNumberList();
/*numberListを初期化*/
numberList_t * initNumberList(numberList_t * numberList);
/*numberListをセット*/
numberList_t * setNumberList(characterList_t * characterList);

/*上段で使われている文字を把握*/
characterList_t * setUpperSide(characterList_t * head, mask_t * mask);
/*numberListとcharacterListを参照して、ランダムに空いてる数字をいれる*/
int setNumAtRandom(characterList_t * current, numberList_t * numberList);
/*各文字に割り当てられた数字をもとに、覆面算の本体をセット*/
int setMaskWithNum(characterList_t * characterList, mask_t * mask);

/************************************
        覆面算を解く関数群
 ************************************/
/*覆面算の繰り上がり情報をリセット*/
int initCarry(mask_t * mask);
/*その列で計算が成立しているかcheck 下段に数字が割り当てられていなければ新しい数字を割り当てる
  数字の割当が不可能となったらnogoodを返す*/
int checkColumn(mask_t * mask, characterList_t * characterList, numberList_t * numberList, int column);
/*nogoodが起きたときに、その原因を取り除く*/
int removeNogoodSup(mask_t * mask, characterList_t * characterList, numberList_t * numberList);
/*覆面算を解く*/
int solveMask(mask_t * mask, characterList_t * characterList, numberList_t * numberList);

/******************************
        結果表示用関数群
 ******************************/

void printCharacterList(characterList_t * characterList);
void printMaskWithChar(mask_t * mask);
void printMaskWithNum(mask_t * mask);

void myError(char* errorMessage);
