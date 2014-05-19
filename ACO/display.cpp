#include "header.h"
#define X N
#define Y N
#define Z N

using namespace std;

/*******************************************
           グローバル変数宣言
 *******************************************/

//空間の管理
GLdouble vertex[X + 1][Y + 1][Z + 1][3];
GLdouble vertex_colony[3];

//ライトの位置
GLfloat light0pos[] = { 12.0, 12.0, 12.0, 1.0 };
GLfloat light1pos[] = { -12.0, 12.0, 12.0, 1.0 };
GLfloat light2pos[] = { 12.0, -12.0, 12.0, 1.0 };
GLfloat light3pos[] = { 12.0, 12.0, -12.0, 1.0 };
GLfloat light4pos[] = { -12.0, 12.0, -12.0, 1.0 };

//色
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat red[] = { 1.0, 0.2, 0.2, 1.0 };
GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };
GLfloat colorArray[][4] = {
  {0.5, 0.0 ,1.0 ,1.0},//紫
  {0.0, 0.0 ,1.0 ,1.0},//青
  {0.0, 1.0 ,1.0 ,1.0},//シアン
  {0.0, 1.0 ,0.0 ,1.0},//緑
  {1.0, 1.0 ,0.0 ,1.0},//黄色
  {1.0, 0.5 ,0.0 ,1.0},//だいだい
  {0.7, 0.3 ,0.3 ,1.0},//茶
  {1.5, 0.0 ,0.0 ,1.0},//赤
  {1.5, 0.0 ,0.7 ,1.0}//ピンク
};

//状態処理
int rotation;//回転角
GLfloat position;//距離
const GLfloat headPosition = 600;
GLint motionSavepoint[2]; /* ドラッグ用以前の座標 */
int headView;//上から見ている状態のときに1
int levelView;//水平視点から見ている時に1
int selectNum;//特定の番号のみに注目して描写
//window sizeを保管しておく
int window_size_x = 800;
int window_size_y = 800;
//終了したか

typedef enum{
  left_down,
  right_down,
  none
} MouseButtonState;

MouseButtonState  mouseButtonState;

//ACOとの連携
double (*d_tau)[N*N][N*N];
double *d_tau0;
int * d_isFinished;
int * d_parameterAdjustmentMode;

/*******************************************
                  関数群
 *******************************************/

/* 枠線の描写 */
void frame(void)
{
  glBegin(GL_LINES);
  for(int i = 0; i <= X; i++){
    for(int j = 0; j <= Y; j++){
      glVertex3dv(vertex[i][j][0]);
      glVertex3dv(vertex[i][j][X]);
    }
  }

  for(int i = 0; i <= Y; i++){
    for(int j = 0; j <= Z; j++){
      glVertex3dv(vertex[i][0][j]);
      glVertex3dv(vertex[i][Y][j]);
    }
  }

  for(int i = 0; i <= Z; i++){
    for(int j = 0; j <= X; j++){
      glVertex3dv(vertex[0][i][j]);
      glVertex3dv(vertex[Z][i][j]);
    }
  }
  
  glEnd();
}

void nodes(){
  glBegin(GL_POINTS);

  for(int i = 0; i < X; i++){
    for(int j = 0; j < Y; j++){
      for(int k = 0; k < Z; k++){
        glVertex3dv(vertex[i][j][k]);
      }
    }
  }
     
  glEnd();

}

void brokenLine(int x1, int y1, int z1, int x2, int y2, int z2, double density){
  double dx, dy, dz;
  double x21 = x2 - x1;
  double y21 = y2 - y1;
  double z21 = z2 - z1;
  double len = sqrt(x21*x21 + y21*y21 + z21*z21);

  dx = x21 / len / density;
  dy = y21 / len / density;
  dz = z21 / len / density;
  
  glBegin(GL_POINTS);

  for(int i = 0;
      i < density * len;
      i++){
  
    glVertex3d(
      vertex[x1][y1][z1][0] + dx*i,
      vertex[x1][y1][z1][1] + dy*i,
      vertex[x1][y1][z1][2] + dz*i
      );

  }
  glEnd();

}

void brokenLineFromColony(int x2, int y2, int z2, double density){
  double dx, dy, dz;
  int x1 = -1;
  int y1 = 4;
  int z1 = 4;
  double x21 = x2 - x1;
  double y21 = y2 - y1;
  double z21 = z2 - z1;
  double len = sqrt(x21*x21 + y21*y21 + z21*z21);

  dx = x21 / len / density;
  dy = y21 / len / density;
  dz = z21 / len / density;
  
  glBegin(GL_POINTS);

  for(int i = 0;
      i < density * len;
      i++){
  
    glVertex3d(
      vertex_colony[0] + dx*i,
      vertex_colony[1] + dy*i,
      vertex_colony[2] + dz*i
      );

  }
  glEnd();

}

void line(int x1, int y1, int z1, int x2, int y2, int z2){
  glBegin(GL_LINES);
  
  glVertex3dv(vertex[x1][y1][z1]);
  glVertex3dv(vertex[x2][y2][z2]);
  
  glEnd();

}

void edge(int k, int i, int j){
  brokenLine(k, i / N, i % N, k+1, j / N, j % N, d_tau[k][i][j]);

}

void allEdges(){
  for(int k = 0; k < N - 1; k++){
    for(int i = 0; i < N * N; i++){
      for(int j = 0; j < N * N; j++){
        edge(k, i, j);
      }
    }
  }
  for(int i = 0; i < N*N; i++){
    brokenLineFromColony(0, i / N, i % N, d_tau0[i]);    
  }
}



void setLookAt(){
  if(headView){
    gluLookAt(0.0 , headPosition, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
  }else if(levelView){
  gluLookAt(27.0 + position, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  }else{
  /* 視点位置と視線方向 */
  gluLookAt(18.0 + position, 18.0 + position, 18.0 + position, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  }
}

void setLight(){
  /* 光源の位置設定 */
  glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
  glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
  glLightfv(GL_LIGHT2, GL_POSITION, light2pos);
  glLightfv(GL_LIGHT3, GL_POSITION, light3pos);
  glLightfv(GL_LIGHT4, GL_POSITION, light4pos);

}

void setRotate(){
/* 図形の回転 */
  if(!headView)   glRotated((double)rotation, 0.0, 1.0, 0.0);
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  setLookAt();
  setLight();
  setRotate();
  glPushMatrix();
  /* 図形の描画 */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
  nodes();
  allEdges();

  /*図形の描写以上*/
  
  
  glPopMatrix();
  glutSwapBuffers();

}

  
void resize(int w, int h)
{
  glViewport(0, 0, w, h);

  /* 透視変換行列の設定 */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if((!headView)){
    gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
  }else{
    gluPerspective(1.0, (double)w / (double)h, headPosition - 10, headPosition + 10);
  }
  /* モデルビュー変換行列の設定 */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  window_size_x = w;
  window_size_y = h;
}

void mouse(int button, int state, int x, int y)
{
  switch (button) {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN) {
      mouseButtonState = left_down;
      motionSavepoint[0] = x;
      motionSavepoint[1] = y;
    }
    else {
      mouseButtonState = none;
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if (state == GLUT_DOWN) {
    }
    break;
  default:
    break;
  }
}

void motion(int x, int y)
{
  
  //回転
  rotation += x - motionSavepoint[0];
  //距離移動
  position += (y - motionSavepoint[1]) * 0.1;
  
  glutPostRedisplay();

  /* 現在の座標を保存 */
  motionSavepoint[0] = x;
  motionSavepoint[1] = y;
 
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    /* quit */
  case 'q':
  case 'Q':
  case '\033':  /* '\033' は ESC の ASCII コード */
    exit(0);
  case 'i':
  case 'I':
    initACO();
    display();
    break;
    /* simurate */
  case 's':
  case 'S':
    simulateACO();
    display();
    break;
  case 'c':
  case 'C':
    *d_isFinished = 0;
    while(*d_isFinished == 0){
      simulateACO();
    }
    display();
    break;
  // case 'd'://for debugg
  //   static int counter;
  //   for(int i = 0; i < N*N; i++){
  //     d_tau0[i] = 0;
  //   }
  //   d_tau0[counter] = 100.0;
  //   counter++;
  //   if(counter == N*N) counter = 0;
  //   display();
  //   break;
    /* head view mode */    
  case 'h':
  case 'H':
    if(headView){
      headView = 0;
    }
    else{
      headView = 1;
      levelView = 0;
    }
    resize(window_size_x, window_size_y);
    glutPostRedisplay();
    break;
    /* level 水平視点 */
  case 'l':
  case 'L':
    if(levelView) levelView = 0;
    else{
      levelView = 1;
      headView = 0;
    }
    resize(window_size_x, window_size_y);
    glutPostRedisplay();

    break;
  case 'n':
  case 'N':
    if(selectNum < 9) selectNum++;
    else selectNum = 0;
    glutPostRedisplay();
    break;
  case 'P':
  case 'p':
    //定数の最適化モード
    parameterAdjustment();
    break;
  default:
    break;
  }
}

void initVertex(void)
{
//  cout << "initVertex... " << endl;
  
  for(int i = 0; i <= X; i++){
    for(int j = 0; j <= Y; j++){
      for(int k = 0; k <= Z; k++){
        vertex[i][j][k][0] = i - 4.5;
        vertex[i][j][k][1] = j - 4.5;
        vertex[i][j][k][2] = k - 4.5;
      }     
    }
  }

  vertex_colony[0] = - 5.5;
  vertex_colony[1] = - 0.5;
  vertex_colony[2] = - 0.5;
}

void init(void)
{
  /* 背景色の設定 */
  glClearColor(0.0, 0.0, 0.0, 1.0);

  /* 描写の前後関係を有効に */
  glEnable(GL_DEPTH_TEST);

  /* 裏面を描写しない */
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  /* ライトの設置 */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);
  glEnable(GL_LIGHT3);
  glEnable(GL_LIGHT4);

  /* 頂点の設置 */
  initVertex();

}

void linkTau(double (* p_tau)[N*N][N*N], double * p_tau0){
  d_tau = p_tau;
  d_tau0 = p_tau0;

}
void linkSwarmSize(){}

void linkIsFinished(int * p_isFinished){
  d_isFinished = p_isFinished;
  
}

void linkParameterAdjustmentMode(int * p_parameterAdjustmentMode){
  d_parameterAdjustmentMode = p_parameterAdjustmentMode;
}
