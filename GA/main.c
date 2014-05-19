#include "GAI.h"

int main(){
  srand(time(NULL));
//  srand(time(NULL));
  //myAI* AI = initAndEvolveAIWithGA();

  myAI* AI = allocAI();//seed
  initAI(AI);
  addPoint(AI->retina, 3, 0);//網膜の設置
  addPoint(AI->dest, SIZE * 3 / 4, SIZE * 3 / 4);
  cvNamedWindow("output", 1);
  
  initAndEvolveAIWithGA(AI->retina, AI->dest);
  
  //runAI(AI, 5);
  printAIforGA(AI);

  cvWaitKey(0);

  cvDestroyWindow("output");
  freeAI(AI);


  return 0;
}
