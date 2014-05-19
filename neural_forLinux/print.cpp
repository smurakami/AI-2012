#include "header.h"
#include <iostream>
#include <cstdlib>
#include <GL/glut.h>
using namespace std;

/*******************************************
           グローバル変数宣言
 *******************************************/

//空間の管理
GLdouble vertex[10][10][10][3];

// int edge[][2] = {
//   { 0, 1 },
//   { 1, 2 },
//   { 2, 3 },
//   { 3, 0 },
//   { 4, 5 },
//   { 5, 6 },
//   { 6, 7 },
//   { 7, 4 },
//   { 0, 4 },
//   { 1, 5 },
//   { 2, 6 },
//   { 3, 7 }
// };

int face[][4] = {
  { 0, 1, 2, 3 },
  { 1, 5, 6, 2 },
  { 5, 4, 7, 6 },
  { 4, 0, 3, 7 },
  { 4, 5, 1, 0 },
  { 3, 2, 6, 7 }
};

// GLdouble color[][3] = {
//   { 1.0, 0.0, 0.0 },
//   { 0.0, 1.0, 0.0 },
//   { 0.0, 0.0, 1.0 },
//   { 1.0, 1.0, 0.0 },
//   { 1.0, 0.0, 1.0 },
//   { 0.0, 1.0, 1.0 }
// };

//法線
GLdouble normal[][3] = {
  { 0.0, 0.0,-1.0 },
  { 1.0, 0.0, 0.0 },
  { 0.0, 0.0, 1.0 },
  {-1.0, 0.0, 0.0 },
  { 0.0,-1.0, 0.0 },
  { 0.0, 1.0, 0.0 }
};

//数字の画像
int NumberImage[9][7][7] = {
  {
    {0,0,0,0,0,0,0},
    {0,0,1,1,0,0,0},
    {0,0,0,1,0,0,0},
    {0,0,0,1,0,0,0},
    {0,0,0,1,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,1,0,0},
    {0,0,1,1,1,0,0},
    {0,0,1,0,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,1,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,1,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0},
    {0,0,1,0,1,0,0},
    {0,0,1,0,1,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,1,0,0},
    {0,0,0,0,1,0,0},
    {0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,1,0,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,1,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,1,0,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,1,0,1,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,1,0,1,0,0},
    {0,0,0,0,1,0,0},
    {0,0,0,0,1,0,0},
    {0,0,0,0,1,0,0},
    {0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,1,0,1,0,0},
    {0,0,1,1,1,0,0},
    {0,0,1,0,1,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0},
    {0,0,1,1,1,0,0},
    {0,0,1,0,1,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,1,0,0},
    {0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0},
  }
};

//テクスチャを格納する配列
#define TEX_HEIGHT 7
#define TEX_WIDTH  7
static GLubyte image[TEX_HEIGHT][TEX_WIDTH][4];


int (*P_out_cube)[Y][Z];//結果表示用の、ニューラルネットの出力を保管する三次元配列のポインタ

//ライトの位置
GLfloat light0pos[] = { 12.0, 12.0, 12.0, 1.0 };
GLfloat light1pos[] = { 5.0, 3.0, 0.0, 1.0 };

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
int * isFinished;

typedef enum{
  left_down,
  right_down,
  none
} MouseButtonState;

MouseButtonState  mouseButtonState;


/*******************************************
                  関数群
 *******************************************/

void idle(void)
{
    /* 一周回ったら回転角を 0 に戻す */
  if (++rotation >= 360) rotation = 0;
  glutPostRedisplay();
}

/* 枠線の描写 */
void frame(void)
{
  glBegin(GL_LINES);
  for(int i = 0; i <= 9; i++){
    for(int j = 0; j <= 9; j++){
      glVertex3dv(vertex[i][j][0]);
      glVertex3dv(vertex[i][j][9]);
    }
  }

  for(int i = 0; i <= 9; i++){
    for(int j = 0; j <= 9; j++){
      glVertex3dv(vertex[i][0][j]);
      glVertex3dv(vertex[i][9][j]);
    }
  }

  for(int i = 0; i <= 9; i++){
    for(int j = 0; j <= 9; j++){
      glVertex3dv(vertex[0][i][j]);
      glVertex3dv(vertex[9][i][j]);
    }
  }
  
  glEnd();
}

void cube(int xin, int yin, int zin)
{
  int i, j;
  int x, y, z;
  x = yin;
  y = zin;
  z = xin;

  GLdouble current_vertex[8][3];
  for(i = 0; i < 3; i++){
    current_vertex[0][i] = vertex[x][y][z][i];
  }
  for(i = 0; i < 3; i++){
    current_vertex[1][i]= vertex[x+1][y][z][i];
  }
  for(i = 0; i < 3; i++){
    current_vertex[2][i]= vertex[x+1][y+1][z][i];
  }
  for(i = 0; i < 3; i++){
    current_vertex[3][i] = vertex[x][y+1][z][i];
  }
  for(i = 0; i < 3; i++){
    current_vertex[4][i] = vertex[x][y][z+1][i];
  }
  for(i = 0; i < 3; i++){
    current_vertex[5][i] = vertex[x+1][y][z+1][i];
  }
  for(i = 0; i < 3; i++){
    current_vertex[6][i] = vertex[x+1][y+1][z+1][i];
  }
  for(i = 0; i < 3; i++){
    current_vertex[7][i] = vertex[x][y+1][z+1][i];
  }
  
  glBegin(GL_QUADS);
      
  for(j = 0; j < 6; j++){
    glNormal3dv(normal[j]);
    if(j != 5)
    for(i = 0; i < 4; i++){
      glVertex3dv(current_vertex[face[j][i]]);
    }
  }
  glEnd();
}

void setImage(int num){
  int i, j, c;
  for (i=0;i<TEX_HEIGHT;i++) {
    for (j=0;j<TEX_WIDTH;j++) {
      if(NumberImage[num][i][j] == 0){
        image[i][j][0]= image[i][j][1]= image[i][j][2]=255;
      }else{
        image[i][j][0]= image[i][j][1]= image[i][j][2]=0;
      }
      image[i][j][3]=255;
    }
  }
}

void setTexture(int num){
  setImage(num);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_WIDTH, TEX_HEIGHT, 
	       0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

void displayTexPolygon(int i, int j, int k){
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(1.0, 1.0); glVertex3dv(vertex[j+1][k+1][i+1]);
  glTexCoord2f(0.0, 1.0); glVertex3dv(vertex[j  ][k+1][i+1]);
  glTexCoord2f(0.0, 0.0); glVertex3dv(vertex[j  ][k+1][i  ]);
  glTexCoord2f(1.0, 0.0); glVertex3dv(vertex[j+1][k+1][i  ]);
  glEnd();
  glDisable(GL_TEXTURE_2D); 
}


void display(void)
{
  int i;
  int j;
  int k;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  if(headView){
    gluLookAt(0.0 , headPosition, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
  }else if(levelView){
  gluLookAt(27.0 + position, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  }else{
  /* 視点位置と視線方向 */
  gluLookAt(18.0 + position, 18.0 + position, 18.0 + position, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  }

  /* 光源の位置設定 */
  glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
  glLightfv(GL_LIGHT1, GL_POSITION, light1pos);

  /* モデルビュー変換行列の保存 */
  glPushMatrix();

  /* 図形の回転 */
  if(!headView)   glRotated((double)rotation, 0.0, 1.0, 0.0);

  /* 枠線の描画 */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
  frame();

  /* 立方体の描写  */
  if(!selectNum){
    for(i = 0; i < X; i++){
      for(j = 0; j < Y; j++){
        for(k = 0; k < Z; k++){
          glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorArray[k]);
          if(P_out_cube[i][j][k]){
            cube(i, j, k);
            setTexture(k);
            displayTexPolygon(i, j, k);
          }
        }
      }
    }
  }else{
    k = selectNum - 1;
    for(i = 0; i < X; i++){
      for(j = 0; j < Y; j++){
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorArray[k]);
        if(P_out_cube[i][j][k]){
          cube(i, j, k);
          setTexture(k);
          displayTexPolygon(i, j, k);
        }
      }
    }
  }
  
  
  /* モデルビュー変換行列の復帰 */
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
  NET* Net;//ダミー
  FLOAT * L_Input;//ダミー
  switch (key) {
    /* quit */
  case 'q':
  case 'Q':
  case '\033':  /* '\033' は ESC の ASCII コード */
    exit(0);
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
    /* step simurate */
  case 's':
  case 'S':
    SimulateNet(Net, L_Input);
    break;
    /* print input */
  case 'i':
  case 'I':
    Writeinput();
    break;
    /* print output */
  case 'o':
  case 'O':
    Writeoutput();
    break;
    /* continue 50 steps */
  case 'c':
  case 'C':
    while(!(*isFinished)){
      SimulateNet(Net, L_Input);
    }
    break;
  case 'n':
  case 'N':
    if(selectNum < 9) selectNum++;
    else selectNum = 0;
    glutPostRedisplay();
    break;
  default:
    break;
  }
}

void initVertex(void)
{
//  cout << "initVertex... " << endl;
  
  for(int i = 0; i <= 9; i++){
    for(int j = 0; j <= 9; j++){
      for(int k = 0; k <= 9; k++){
        vertex[i][j][k][0] = i - 4.5;
        vertex[i][j][k][1] = j - 4.5;
        vertex[i][j][k][2] = k - 4.5;
        // cout << "v(" << i << ", "
        //      << j << ", "
        //      << k << ") = ("
        //      << vertex[i][j][k][0] << ", "
        //      << vertex[i][j][k][1] << ", "
        //      << vertex[i][j][k][2] << ")" << endl;
      }     
    }
  }
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

  /* 頂点の設置 */
  initVertex();

}

/* グローバル配列out_cubeのリンク */
void link_out_cube(int (*out_cube)[Y][Z]){
  P_out_cube = out_cube;
  
  // cout << "check grobal array linking." << endl;
  // cout << "out_cube(= G_out_cube)" << endl;
  // cout << endl;
  int i, j, k;
  for(i = 0; i < X; i++){
    for(j = 0; j < Y; j++){
      for(k = 0; k < Z; k++){
        if(out_cube[i][j][k]){
//          cout << k+1;
          break;
        }
      }
      // if(k == Z) cout << '*';
      // if(j % 3 == 2) cout << ' ';
    }
//    cout << endl;
//    if(i % 3 == 2) cout << endl;
  }

  // cout << "P_out_cube" << endl;
  // cout << endl;
  for(i = 0; i < X; i++){
    for(j = 0; j < Y; j++){
      for(k = 0; k < Z; k++){
        if(P_out_cube[i][j][k]){
//          cout << k+1;
          break;
        }
      }
      // if(k == Z) cout << '*';
      // if(j % 3 == 2) cout << ' ';
    }
//    cout << endl;
//    if(i % 3 == 2) cout << endl;
  }
  
  
}

void linkFinished(int * p){
  isFinished = p;

}
