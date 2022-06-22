#include <stdio.h>
int main(){
    FILE* in;
    FILE* out;
    char data;
    int cnt = 1;
    if ((in = fopen("buff.txt", "r")) != NULL) {
        out = fopen("buff1.txt", "w");
        while (fscanf(in, "%c", &data) != -1) {
            if (data == '1') {
                fprintf(out, "%d", cnt);
                break;
            }
            cnt++;
        }
    }
    else {
        printf("error name");
    }
    fclose(in);
    fclose(out);
}
