/*********************************

     TMSによるN QUEEN問題の解法

 *********************************

NQUEEN問題とは・・・
N x Nマスのチェス版上にN個のQUEENを、お互いに
とりあうことのないように並べる問題。

まず、この問題で扱うWorking Memory(WM)をどのように表現するかを考える。
WM上のデータの起動は、その指示仮説を伴って表されるべきである。
すると、こんな感じで表現するのが妥当である。

 */

/*盤のサイズ*/
int N;
/*何回かかったかカウント*/
int globalCounter;
/*TESTで、バックトラックの上限回数をきめる*/
int maxBT;

//盤上のQueen
typedef struct queen{
    int x;
    int y;
    struct queen * next;
} queen_t;

typedef enum{
    OUT,
    IN
} dataState;

//VM上のデータ
typedef struct data{
    dataState state;//inかoutか
    queen_t* supporter;//そのdataをinにしているQueen
} data_t;

//VM本体
typedef struct workingMemory{
    data_t * line;
    data_t * column;
    data_t goal;
    data_t nogood;

    queen_t * buffer;//保管場所
    
} workingMemory_t;

//チェス板 何かに使うかも    
typedef struct chessBoard{
    char** state;//そこにQueenがおいてあるか
} chessBoard_t;

/*utility*/
void myError(char* errorMassage);
double currentTime();

/*初期化*/
workingMemory_t * allocWorkingMemory();
workingMemory_t * initWorkingMemory(workingMemory_t * workingMemory);
chessBoard_t * allocChessBoard();
chessBoard_t * initChessBoard(chessBoard_t * chessBoard);
char * allocCharArray();
char * initCharArray(char * array);
/*あとしまつ*/
void freeWorkingMemory(workingMemory_t* workingMemory);
void freeChessBoard(chessBoard_t * chessBoard);
void freeQueenList(queen_t * list);

/*chessBoardを読み込んでworkingMemoryをセット デバック用の関数*/
workingMemory_t * setWorkingMemoryWithChessBoard(workingMemory_t * workingMemory, chessBoard_t * chessBoard);

/*チェス板のi, jが安全な場所かどうか評価*/
int isSafePlace(workingMemory_t * workingMemory, chessBoard_t * chessBoard, int line, int column);

/*QueenListに、Queen(List)を追加*/
queen_t * addQueenList(queen_t * target, queen_t * addition);

/*Queenをsafeにmalloc*/
queen_t * allocQueen();

/*あるLineをINにできるか調べる。可能であればLineをINにする。不可能であれば,nogoodがINとなる*/
int checkLine(workingMemory_t * workingMemory, chessBoard_t * chessBoard, int currentLine);

/*結果の表示*/
int printChessBoard(chessBoard_t * chessBoard);
int printChessBoardForDebug(workingMemory_t * workingMemory, chessBoard_t * chessBoard);

/*元main*/
int runTMS();

/****************************

 比較用のバックトラックによる探索

****************************/

int runBT();
int BT(chessBoard_t* chessBoard, int line);
int BTisSafePlace(chessBoard_t * chessBoard, int line, int column);
int BTrookSafe(chessBoard_t * chessBoard, int line, int column);
int BTbishopSafe(chessBoard_t * chessBoard, int line, int column);
