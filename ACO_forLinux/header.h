/************************************
 ************************************
 *                                  *
 *  N - Q U E E N   W I T H   A C O *
 *                                  *
 ************************************
 ************************************/

/************************************
          I N C L U D E S   
 ************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

/************************************
         P A R A M E T E R S
 ************************************/

#define N 8 //number of Queens
#define DEFAULT_SWARM_SIZE 30 //default swarm size

/************************************
   D I S P L A Y   F U N C T I O N S  
 ************************************/

void display(void);
void keyboard(unsigned char key, int x, int y);
void resize(int, int);
void mouse(int, int , int, int);
void motion(int, int);
void init(void);
void initVertex(void);
void linkTau(double (* p_tau)[N*N][N*N], double * tau_0);
void linkSwarmSize();
void linkParameterAdjustmentMode(int *);

/************************************
      A C O    F U N C T I O N S 
 ************************************/

/*----     INITIALIZATION     ----*/
void initTau();
void initAnts();
void linkGlovalVariable();
void initACO();
void linkIsFinished(int * p_isFinished);

/*----       SIMULATION       ----*/
int simulateACO();
void parameterAdjustment();
