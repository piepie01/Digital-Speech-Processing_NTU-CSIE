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
int sequence[2500][MAX_SEQ];
int load_Seq(const char *filename){
    FILE *fp = open_or_die(filename, "r");
    char tmp[SEQ_LEN+1];
    for(int i=0;i<2500;i++){
        fread(tmp, 1, SEQ_LEN+1, fp);
        for(int j=0;j<SEQ_LEN;j++) sequence[i][j] = tmp[j] - 'A';
        sequence[i][SEQ_LEN] = -1;
    }
    for(int i=0;i<5;i++){
        for(int j=0;j<SEQ_LEN;j++) printf("%d%c",sequence[i][j]," \n"[j==SEQ_LEN-1]);
    }
    return 0;
}
int find_max_prop(int line, HMM *hmms, double props[]){
    int adj = 0;
    for(int hmm_i=0;hmm_i<5;hmm_i++){
        double d[SEQ_LEN-adj][STATE_NUM];
        for(int t=0;t<SEQ_LEN-adj;t++){
            for(int sta=0;sta<STATE_NUM;sta++){
                if(t==0) d[t][sta] = (hmms[hmm_i].initial[sta]) * (hmms[hmm_i].observation[sequence[line][t]][sta]);
                else{
                    double p=-1;
                    for(int i=0;i<STATE_NUM;i++){
                        double tmp = d[t-1][i] * (hmms[hmm_i].transition[i][sta]);
                        if(tmp > p) p = tmp;
                    }
                    d[t][sta] = p * (hmms[hmm_i].observation[sequence[line][t]][sta]);
                }
            }
        }

        //for(int x=0;x<SEQ_LEN-adj;x++)
        //   for(int y=0;y<STATE_NUM;y++)
        //        printf("%g%c",d[x][y]," \n"[y==STATE_NUM-1]);
        //printf("\n");
        //exit(0);
        double ans=0;
        for(int sta=0;sta<STATE_NUM;sta++){
            if(d[SEQ_LEN-adj-1][sta] > ans) ans = d[SEQ_LEN-adj-1][sta];
        }
        props[hmm_i] = ans;
    }
    double m=0;
    int ind=0;
    for(int i=0;i<5;i++){
        if(props[i] > m) m = props[i], ind = i;
    }
    return ind;
}
int main(int argc, char *argv[]){

	HMM hmms[5];
	load_models( argv[1], hmms, 5);
	dump_models( hmms, 5);

    
    load_Seq(argv[2]);
    FILE *output = open_or_die(argv[3],"w");
    for(int i=0;i<2500;i++){
        double props[5] = {0};
        int ind = find_max_prop(i,hmms,props);
        //for(int j=0;j<5;j++) printf("%lf%c",props[j]," \n"[j==4]);
        //printf("%d\n",ind);
        fprintf(output, "%s %g\n", hmms[ind].model_name, props[ind]);
    }
    

	return 0;
}
