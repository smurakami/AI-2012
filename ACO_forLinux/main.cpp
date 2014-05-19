#include "header.h"
#include <time.h>

int main(int argc, char ** argv){

  srand(time(NULL));
  glutInitWindowSize(800, 800);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  linkGlovalVariable();
  init();
  initACO();

  glutMainLoop();

  
  return 0;
}
