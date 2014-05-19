/*
  ポップフィールド型ネットワークを用いた数独の解法
  120443 村上晋太郎
 */


/*******************************************
      数独を解くニューラルネット popfield.cpp
 *******************************************/
#define X             9
#define Y             9
#define Z             9
#define N             (X * Y * Z)


typedef int           BOOL;
typedef char          CHAR;
typedef int           INT;
typedef float         FLOAT;

typedef struct {                     /* A NET:                                */
        INT           Units;         /* - number of units in this net         */
        FLOAT*          Output;        /* - output of ith unit                  */
        INT*          Threshold;     /* - threshold of ith unit               */
} NET;

/*
 * グローバル変数群
 */

/*
 * 関数群
 */
void InitializeRandoms();
void GenerateNetwork(NET* Net);
void InitializeApplication(NET* Net);

void Writeinput();
void Writeoutput();
void SimulateNet(NET* Net, FLOAT* Input);

void FinalizeApplication(NET* Net);


/*******************************************
         結果表示用関数 print.cpp
 *******************************************/

void idle(void);
/* 枠線の描写 */
void frame(void);
void cube(int x, int y, int z);
void display(void);
void resize(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void initVertex(void);
void init(void);

/* グローバル配列out_cubeのリンク */
void link_out_cube(int (*out_cube)[Y][Z]);
