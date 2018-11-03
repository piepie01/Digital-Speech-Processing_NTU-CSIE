#include "hmm.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef SEQ_LEN
#	define SEQ_LEN 	50
#endif

#ifndef STATE_NUM
#	define STATE_NUM 	6
#endif

#ifndef OBSER_NUM
#	define OBSER_NUM 	6
#endif
int sequence[10000][MAX_SEQ];
int load_Seq(const char *filename){
    FILE *fp = open_or_die(filename, "r");
    char tmp[SEQ_LEN+1];
    for(int i=0;i<10000;i++){
        fread(tmp, 1, SEQ_LEN+1, fp);
        for(int j=0;j<SEQ_LEN;j++) sequence[i][j] = tmp[j] - 'A';
        sequence[i][SEQ_LEN] = -1;
    }
    for(int i=0;i<5;i++){
        for(int j=0;j<SEQ_LEN;j++) printf("%d%c",sequence[i][j]," \n"[j==SEQ_LEN-1]);
    }
    return 0;
}
void print_arr(double arr[][6],int a,int b){
    for(int i=0;i<a;i++){
        printf("i = %d :",i);
        for(int j=0;j<b;j++) printf("%g%c",arr[i][j]," \n"[j==b-1]);
    }
    return;
}
int train(HMM *hmm, int epochs){
    for(int epoch=0;epoch<epochs;epoch++){
        printf("=====epoch %d=====\n",epoch+1);
        double new_initial[STATE_NUM];
        double new_transition[STATE_NUM][STATE_NUM];
        double new_observation[OBSER_NUM][STATE_NUM];
        memset(new_initial, 0, sizeof(new_initial));
        memset(new_transition, 0, sizeof(new_transition[0][0])*STATE_NUM*STATE_NUM);
        memset(new_observation, 0, sizeof(new_observation[0][0])*OBSER_NUM*STATE_NUM);
        for(int seq=0;seq<10000;seq++){
            double a[SEQ_LEN][OBSER_NUM], b[SEQ_LEN][OBSER_NUM], g[SEQ_LEN][OBSER_NUM], e[SEQ_LEN][OBSER_NUM][OBSER_NUM];

            //forward
            for(int t=0;t<SEQ_LEN;t++){
                for(int obs=0;obs<OBSER_NUM;obs++){
                    if(t==0) a[t][obs] = hmm->initial[obs] * hmm->observation[sequence[seq][t]][obs];
                    else{
                        double p=0;
                        for(int i=0;i<OBSER_NUM;i++) p += a[t-1][i] * hmm->transition[i][obs];
                        a[t][obs] = p * hmm->observation[sequence[seq][t]][obs];
                    }
                }
            }
            //backward
            for(int t=SEQ_LEN-1;t>=0;t--){
                for(int obs=0;obs<OBSER_NUM;obs++){
                    if(t==SEQ_LEN-1) b[t][obs] = 1;
                    else{
                        double p=0;
                        for(int i=0;i<OBSER_NUM;i++) p += hmm->transition[obs][i] * hmm->observation[sequence[seq][t+1]][i] * b[t+1][i];
                        b[t][obs] = p;
                    }
                }
            }
            //counting gamma
            for(int t=0;t<SEQ_LEN;t++){
                double p=0;
                for(int obs=0;obs<OBSER_NUM;obs++) p += a[t][obs] * b[t][obs];
                for(int obs=0;obs<OBSER_NUM;obs++) g[t][obs] = a[t][obs] * b[t][obs] / p;
            }
            //print_arr(g, SEQ_LEN, OBSER_NUM);
            //exit(0);
            //counting epsilon
            for(int t=0;t<SEQ_LEN-1;t++){
                double p=0;
                for(int i=0;i<OBSER_NUM;i++)
                    for(int j=0;j<OBSER_NUM;j++) p += a[t][i] * hmm->transition[i][j] * hmm->observation[sequence[seq][t+1]][j] * b[t+1][j];
                for(int i=0;i<OBSER_NUM;i++)
                    for(int j=0;j<OBSER_NUM;j++) e[t][i][j] = a[t][i] * hmm->transition[i][j] * hmm->observation[sequence[seq][t+1]][j] * b[t+1][j] / p;
            }
            //recording update value
            for(int i=0;i<STATE_NUM;i++) new_initial[i] += g[0][i];
            for(int i=0;i<STATE_NUM;i++){
                double p=0;
                for(int j=0;j<SEQ_LEN-1;j++) p += g[j][i];
                for(int j=0;j<STATE_NUM;j++){
                    double p_0=0;
                    for(int t=0;t<SEQ_LEN-1;t++) p_0 += e[t][i][j];
                    new_transition[i][j] += p_0 / p;
                }
            }
            for(int i=0;i<STATE_NUM;i++){
                double p_arr[OBSER_NUM]={0}, p=0;
                for(int j=0;j<SEQ_LEN;j++) p_arr[sequence[seq][j]] += g[j][i], p += g[j][i];
                for(int j=0;j<OBSER_NUM;j++) new_observation[j][i] += p_arr[j] / p;
            }

        }
        //for(int i=0;i<STATE_NUM;i++) printf("%lf%c",new_initial[i]," \n"[i==STATE_NUM-1]);
        //for(int i=0;i<STATE_NUM;i++)
        //    for(int j=0;j<STATE_NUM;j++) printf("%lf%c",new_transition[i][j]," \n"[j==STATE_NUM-1]);
        //for(int i=0;i<OBSER_NUM;i++)
        //    for(int j=0;j<STATE_NUM;j++) printf("%lf%c",new_observation[i][j]," \n"[j==STATE_NUM-1]);
        for(int i=0;i<STATE_NUM;i++) hmm->initial[i] = new_initial[i]/10000;
        for(int i=0;i<STATE_NUM;i++)
            for(int j=0;j<STATE_NUM;j++) hmm->transition[i][j] = new_transition[i][j]/10000;
        for(int i=0;i<OBSER_NUM;i++)
            for(int j=0;j<STATE_NUM;j++) hmm->observation[i][j] = new_observation[i][j]/10000;
	    dumpHMM( stderr, hmm );


    }
    return 0;
}
int main(int argc, char *argv[]){
/*
	HMM hmms[5];
	load_models( "modellist.txt", hmms, 5);
	dump_models( hmms, 5);
*/
	HMM hmm_initial;
	loadHMM( &hmm_initial, argv[2] );
	dumpHMM( stderr, &hmm_initial );
    
    load_Seq(argv[3]);
    int epochs = atoi(argv[1]);
    train(&hmm_initial, epochs);
//	printf("%f\n", log(1.5) ); // make sure the math library is included
    
    FILE *output = open_or_die(argv[4],"w");
    dumpHMM(output, &hmm_initial);
    

	return 0;
}
