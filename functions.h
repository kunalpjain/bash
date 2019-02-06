#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdbool.h>

#define MAX_SIZE 100

bool check (char *filepath);
char *getfile(char *path,char *firstarg);
char **getargv(char *buff,int *nofargs,bool *back_pr);
char *getcommand(char *buf,int *noofargs);
void greatersign(char *p,char **argv,int nofargs);
