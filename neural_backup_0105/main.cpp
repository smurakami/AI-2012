#include "header.h"
#include <iostream>
#include <cstdlib>
#include <GL/glut.h>

FLOAT                   Input [N];

int main(int argc, char *argv[])
{
  /*
    結果表示用のウインドウ等の初期化
  */
  glutInitWindowSize(800, 800);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  init();

  /* ニューラルネットワークの初期化 */

  NET Net;
  INT n;

  InitializeRandoms();
  GenerateNetwork(&Net);
  InitializeApplication(&Net);
  
  Writeinput();
  SimulateNet(&Net, Input);
   
  FinalizeApplication(&Net);

  /* 実行 */
  glutMainLoop();
  return 0;
}
