#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "NQueen.h"

int main(int argc, char** argv){
    if(argc > 3 || argc < 2) myError("usage: NQ TMS/BT/TEST N");
    if(argc == 3) N = atoi(argv[2]);
    else N = 8;

    if(strcmp(argv[1], "TMS") == 0){
        globalCounter = 0;
        while(runTMS()){
            /*解が見つかるまで頑張る*/
        }
        //printf("Finished running TMS in %d cycles\n", globalCounter);
    }else if(strcmp(argv[1], "BT") == 0){
        globalCounter = 0;
        maxBT = 100000000;
        runBT();
        //printf("Finished running BT in %d cycles\n", globalCounter);
    }else if(strcmp(argv[1], "TEST") == 0){
        /* FILE *fp = fopen("TMS.dat", "w"); */
        /* maxBT = 0; */
        /* fprintf(fp, "#N\tcycle\n"); */
        /* for(N = 4; N < 100; N++){ */
        /*     globalCounter = 0; */
        /*     int i; */
        /*     for(i = 0; i < 4; i++){ */
        /*         while(runTMS()) */
        /*         { */
        /*             /\*解がみつかるまで頑張るよ！*\/ */
        /*         } */
        /*     } */
        /*     globalCounter /= i; */
        /*     if(globalCounter > maxBT) maxBT = globalCounter; */
        /*     fprintf(fp, "%d\t%d\n", N, globalCounter); */
        /* } */
        /* fclose(fp); */

        FILE * fpBT = fopen("BT.dat", "w");
        fprintf(fpBT, "#N\tcycle\n");
        maxBT = 10000;
        for(N = 4 ; N < 100; N++){
            globalCounter = 0;
            runBT();
            fprintf(fpBT, "%d\t%d\n", N, globalCounter);
        }
        fclose(fpBT);
    }else{
    myError("usage: NQ {TMS/BT/TEST} (N)");
    }

    return 0;
}
