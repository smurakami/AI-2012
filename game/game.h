/*---------------------------
        定数定義
  ---------------------------*/
#define SIZE 8//ボードサイズ
#define DEPTH 5//探索深さ

int valueOfPiece;//駒の基本ポイント
int valueOfProtecter;//駒が守られている時の加算ポイント
int valueOfAffecter;//駒が脅威にさらされている時の加算ポイント マイナス
int valueOfAdvance;//駒が一マス進むごとに加算
int valueOfKing;//駒が向こう岸に到達した場合加算

/*プレイヤーの駒関連*/
typedef enum{
  NONE,//どちらでもない
  A,//プレイヤーA 普通の駒
  AK,//プレイヤーA キング
  B,//プレイヤーB 普通の駒
  BK//プレイヤーB キング
} pKind_t;

/*モード関連*/
typedef enum{
  H2H,//人vs人
  H2C,//人vsCOM
  C2C//COMvsCOM
} runMode_t;
/*次の手にどのようにつながっているか*/
typedef enum{
  cont,//通常
  pass,//パス。使わないかも
  finish//終局
} runState_t;

typedef enum{
  R,//右
  L,//左
  other
} direction_t;

/*---------------------------
        構造体定義
  ---------------------------*/
//座標をしめす
typedef struct point{
  int x;
  int y;
}* point_t;

typedef struct piece{
  int enable;//有効か無効か 0:無効 1:動ける 2:取れる駒がある
  struct point p;//その駒の座標を示す
  int left;//左にうごけるか 0:動けない 1:動ける 2:取れる駒がある
  int right;//右に動けるか 0:動けない 1:動ける 2:取れる駒がある
  struct state * leftSt;//その駒を左に動かした場合のSt
  struct state * rightSt;//その駒を右に動かした場合のSt
}* piece_t;

//チェッカーボード本体
typedef struct state{
  pKind_t bd[SIZE][SIZE];//board
  piece_t piece[(SIZE*3)/2];//次に動かせる駒を保存する。
  piece_t attackingPiece;//攻撃中の駒。二度目の攻撃が起こったときに使用
  //ターンごとのパラメータ
  pKind_t player;//今誰が操作しているか
  int activePiece;//動かせる駒の数
  int attack;//取れる駒があるか あれば1, 二度目以降の攻撃で2
  piece_t pieceToMove;//動かそうとする駒
  //minmax関連
  int value;//その盤の状態の評価値
  int valueIndex;//その評価値を与える駒のインデックス
  direction_t valueDirection;
  //ゲーム全体のパラメータ
  runMode_t mode;//人が戦っているか、コンピュータがたたかっているか
  pKind_t winner;//最終的な勝者
}* state_t;

/*---------------------------
      初期化関連関数 init.c
  ---------------------------*/

state_t allocState();
int initState(state_t st);
int allocPieceList(state_t st);
int initPieceList(state_t st);
int setBoard(state_t st);
point_t allocPoint();
piece_t allocPiece();
int copyPiece(piece_t p1, piece_t p0);
int copyState(state_t s1, state_t s0);

/*---------------------------
      ゲーム進行関連 run.c
  ---------------------------*/
//ゲームの進行
pKind_t runGameH2H(state_t st);//人vs人
pKind_t runGameH2C(state_t st);//人vsCOM
pKind_t runGameC2C(state_t st);//COMvsCOM
//ターンの進行
runState_t Hturn(state_t st, pKind_t player);//人のターン
runState_t Cturn(state_t st, pKind_t player);//COMのターン

/*---------------------------
    ゲーム板のチェック check.c
  ---------------------------*/
//動かせる駒を調べる
int checkBoard(state_t st);
//その駒を動かせるか、動かせるならその方向を調べる。
int checkPiece(state_t st, struct point p);
//攻撃できる駒をしらべる
int checkBoardToAttack(state_t st);
//その駒を動かせるか、動かせるならその方向を調べる
int checkPieceToAttack(state_t st, struct point p);
//その座標がチェス板の上か
int inTheBoard(int x, int y);

/*---------------------------
     事後処理関連 finish.c
  ---------------------------*/

int freeState(state_t st);
/*---------------------------
      結果表示関連 print.c
  ---------------------------*/
int printBoard(state_t st);

/*---------------------------
     駒を動かす move.c
  ---------------------------*/
void stringToInt(point_t p);
void choosePoint(point_t p);
int choosePiece(state_t st, point_t p);

int movePieace(state_t st, point_t p);
int move(state_t st, point_t p, direction_t d);
int moveForMinMax(state_t st, point_t p, direction_t d);

/*---------------------------
  盤面の評価値の算出 estimate.c
  ---------------------------*/
int estimate(state_t st);
int estimatePiece(state_t st, int x, int y);

/*---------------------------
  minmax法の実行 minmax.c
  ---------------------------*/
int minmax(state_t st, int depth);

/*---------------------------
      その他便利関数 util.c
  ---------------------------*/

void myError(char * s);
