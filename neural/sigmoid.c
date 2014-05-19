#include <stdio.h>
#include <math.h>

/**************
  シグモイド関数
 **************/
//シグモイド関数の計算の高速化のためのテーブル
double sigmoid_table[1000];

double sigmoid(double x){
  if(x >= 5) return 1;
  if(x < -5) return 0;
  int i = (int)((-x + 5) * 100);
  return sigmoid_table[i];
}
void initSigmoidTable(){
  int i;
  for(i = 0; i < 1000; i++){
    sigmoid_table[i] = 1.0 / (1.0 + exp(0.01 * i - 5));
  }
}

int main(){
  FILE * fp = fopen("sigmoid.dat", "w");
  double x;
  int i;
  initSigmoidTable();
  for(i = 0; i < 1000; i++){
    x = (double)i * 0.01 - 5.0;
    fprintf(fp, "%f %f\n", x, sigmoid(x));
    printf( "%f %f\n", x, sigmoid(x));
  }

  fclose(fp);
  return 0;
}
