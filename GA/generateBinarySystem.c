#include <stdio.h>
#include <stdlib.h>



/*******************
 "bin_system.hを作成"
 *******************/
int main(){
    int i, j;//counter
    int num;
    char bin[8];
    FILE* fp = fopen("bin_system.h", "w");

    
    for(i = 0; i < 256; i++){
        num = i;
        for(j = 0; j < 8; j++){
            bin[j] = num  % 2;
            num /= 2;
        }
        fprintf(fp, "#define b");
        for(j = 7; j >= 4; j--){
            fprintf(fp, "%d", bin[j]);
        }
        fprintf(fp, "_");
        for(j = 3; j >= 0; j--){
            fprintf(fp, "%d", bin[j]);
        }
        fprintf(fp, " %d\n", i);
    }

    fclose(fp);
    return 0;
}

