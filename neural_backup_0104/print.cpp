#include "header.h"
#include <iostream>
#include <cstdlib>
#include <GL/glut.h>
using namespace std;

/*******************************************
           グローバル変数宣言
 *******************************************/

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

GLfloat light0pos[] = { 12.0, 12.0, 12.0, 1.0 };
GLfloat light1pos[] = { 5.0, 3.0, 0.0, 1.0 };

GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 };
GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };

int rotation;//回転角
GLfloat position;//距離
GLint motionSavepoint[2]; /* ドラッグ用以前の座標 */
int headView;//上から見ている状態のときに1

typedef enum{
  left_down,
  right_down,
  none
} MouseButtonState;

MouseButtonState  mouseButtonState;

//法線
GLdouble normal[][3] = {
  { 0.0, 0.0,-1.0 },
  { 1.0, 0.0, 0.0 },
  { 0.0, 0.0, 1.0 },
  {-1.0, 0.0, 0.0 },
  { 0.0,-1.0, 0.0 },
  { 0.0, 1.0, 0.0 }
};

int *** P_out_cube;

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

void cube(int x, int y, int z)
{
  int i, j;

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
    for(i = 0; i < 4; i++){
      glVertex3dv(current_vertex[face[j][i]]);
    }
  }
  glEnd();
}

void display(void)
{
  int i;
  int j;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  /* 視点位置と視線方向 */
  gluLookAt(18.0 + position, 18.0 + position, 18.0 + position, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  /* 光源の位置設定 */
  glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
  glLightfv(GL_LIGHT1, GL_POSITION, light1pos);

  /* モデルビュー変換行列の保存 */
  glPushMatrix();

  /* 図形の回転 */
  glRotated((double)rotation, 0.0, 1.0, 0.0);

  /* 図形の色 (緑)  */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);

  /* 図形の描画 */
  frame();
  /* 図形の色 (赤)  */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
  
  cube(0, 0, 0);
  cube(4, 8, 1);
  cube(5, 6, 3);

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
  gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

  /* モデルビュー変換行列の設定 */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
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
  case 'q':
  case 'Q':
  case '\033':  /* '\033' は ESC の ASCII コード */
    exit(0);
  case 'h':
  case 'H':
    if(headView){
      gluLookAt(18.0 + position, 18.0 + position, 18.0 + position, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
      glutPostRedisplay();
    }else{
      gluLookAt(0, 0, 18.0 + position, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
      glutPostRedisplay();
    }
    break;
  case 's':
  case 'S':
    NET* Net;//ダミー
    FLOAT * L_Input;//ダミー
    SimulateNet(Net, L_Input);
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
