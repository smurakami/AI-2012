#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#define SIZE 1000000
#define TIMES 1000

//現在時間を返す関数
double currentTime(){
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return tp.tv_sec + tp.tv_usec * 1.0E-6;
}

int main(){
    int i, n;//counter
    double sum = 0;
    
    /*1MBの配列を確保する*/
    char* buffer = (char *)malloc(sizeof(char) * SIZE);
    if(buffer == NULL) exit(1);

    for(n = 0; n < TIMES; n++){
        
        double t0 = currentTime();
        //全ての配列に何らかの処理をする
        for(i = 0; i < SIZE; i++){
            buffer[i] = i*4238497 % 100;
        }
        double t1 = currentTime();

        sum += (t1 - t0);
    }

    printf("Time:%f\n", sum / TIMES);

    free(buffer);
    return 0;
}
