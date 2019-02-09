#include<stdio.h>
#include "functions.h"
main(){
char *v[] = {"kunal","jain","is","my","name"};
char **k = subarray(v,1,3);
for(int i=0;i<3;i++)
printf("%s\n",k[i]);
}
