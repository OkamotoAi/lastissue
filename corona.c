#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define M 4      // 予測曲線の次数

typedef struct corona
{
    int year;
    int month;
    int day;
    int positive;
} crn;

void printcrn(crn c){
    printf("%d/%d/%d,%d\n",c.year,c.month,c.day,c.positive);
}


int main(){

    /*
    char l[]="22020/12/7,1502\n";
    crn crn1;
    crn1.year  = atoi(strtok(l,",/\n"));
    crn1.month = atoi(strtok(NULL,",/\n"));
    crn1.day   = atoi(strtok(NULL,",/\n"));
    crn1.positive   = atoi(strtok(NULL,",/\n"));
    printcrn(crn1);*/

    crn list[365];
    int num=0;
    FILE *fp;
    char line[256];

    fp = fopen("positive.csv", "r");
    if (fp == NULL)
    { //読み込み失敗でNULLが返却される
        fprintf(stderr, "ファイルがオープンできません\n");
        exit(1);
    }
    while(fgets(line,256,fp) != NULL){
        //printf("%s",line);
        list[num].year  = atoi(strtok(line,",/\n"));
        list[num].month = atoi(strtok(NULL,",/\n"));
        list[num].day   = atoi(strtok(NULL,",/\n"));
        list[num].positive   = atoi(strtok(NULL,",/\n"));
        //printcrn(list[num]);
        num++;
    }

    FILE *gp;
    gp = popen("gnuplot -persist", "w"); // パイプを開き、gnuplotの立ち上げ
    //fprintf(gp, "plot sin(x)\n"); // パイプを通してgnuplotにコマンドを送る
    
    /*
    //y = a0 + a1*x
    double a0 = 0,a1 = 0;
    int i=0;
    double A00=0 ,A01=0, A02=0, A11=0, A12=0;
    for (i=300;i<365;i++) {
        A00+=1;
        A01+=i;
        A02+=list[i].positive;
        A11+=i*i;
        A12+=i*list[i].positive;
    }
    a0 = (A02*A11-A01*A12) / (A00*A11-A01*A01);
    a1 = (A00*A12-A01*A02) / (A00*A11-A01*A01);

    printf("a0=%f a1=%f\n", a0, a1);
    fprintf(gp,"set xrange [0:400]\n");
    fprintf(gp,"set yrange [0:5000]\n");
    //出力
    fprintf(gp,"plot\"-\",x*%f+%f\n\n",a1,a0);
    for (i=0;i<365;i++) {
        fprintf(gp,"%d %d\n",i,list[i].positive);//点の出力
    }
    */





    double s[M*2],t[M],a[M+2][M+2];
    int i=0,j=0,k=0;
    double p,d,px;
        // s[], t[] 計算
    for (i = 0; i < 365; i++) {
        for (j = 0; j <= 2 * M; j++)
            s[j] += pow(i, j);
        for (j = 0; j <= M; j++)
            t[j] += pow(i, j) * list[i].positive;
    }
    for (k = 0; k <= M; k++){
        printf("t%d = %f\n", k, t[k]);
    }

    // a[][] に s[], t[] 代入
    for (i = 0; i <= M; i++) {
        for (j = 0; j <= M; j++)
            a[i][j] = s[i + j];
        a[i][M + 1] = t[i];
    }

    // 掃き出し
    for (k = 0; k <= M; k++) {
        p = a[k][k];
        for (j = k; j <= M + 1; j++)
            a[k][j] /= p;
        for (i = 0; i <= M; i++) {
            if (i != k) {
                d = a[i][k];
                for (j = k; j <= M + 1; j++)
                    a[i][j] -= d * a[k][j];
            }
        }
    }

    // y 値計算＆結果出力
    for (k = 0; k <= M; k++){
        printf("a%d = %f\n", k, a[k][M + 1]);
    }

    fprintf(gp,"set xrange [0:400]\n");
    //fprintf(gp,"set yrange [0:5000]\n");
    fprintf(gp,"plot\"-\",%f+%f*x+%f*x**2+%f*x**3+%f*x**4\n",a[0][M+1],a[1][M+1],a[2][M+1],a[3][M+1],a[4][M+1]);
    for (i=0;i<365;i++) {
        fprintf(gp,"%d %d\n",i,list[i].positive);//点の出力
    }

    
    fprintf(gp, "exit\n"); // gnuplotの終了
    fflush(gp);
    pclose(gp);
    fclose(fp);



}