#include "GAI.h"

/*******************
    便利系関数群
 *******************/

/*現在時刻をマイクロ秒の単位の正確さで返す*/
double currentTime(){
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return tp.tv_sec + tp.tv_usec * 1.0E-6;
}

//頭脳上のニューロン同士の距離を出す
int distance(int x1, int y1, int x2, int y2){
  int d;
  int x, y;

  x = x1 - x2;
  if(x < 0) x *= -1;

  y = y1 - y2;
  if(y < 0) y *= -1;

  d = x + y;
  
  return d;
}
