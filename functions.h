#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdbool.h>
#include<signal.h>

#define MAX_SIZE 100
char **lookuptable;

void handler(int signo);
bool check (char *filepath);
void createEmptyFile(char *filename);
void replaceInFile(char *filename, int index, char **argv, int nofargs);
char *indexInFile(FILE *fp, int index);
char *getfile(char *path,char *firstarg);
char **getargv(char *buff,int *nofargs,bool *back_pr);
char *getcommand(char *buf,int *noofargs);
void parsecommand(char *completepath,char *p,char **argv,int nofargs);
int strinarr(char **argv, char *sym, int nofargs,int start_index);
char **subarray(char **argv, int start, int end);
void lessersign(char *p,char **argv,int nofargs);
void greatersign(char *p,char **argv,int nofargs);
void pipesign(char *completepath,char *path,char **argv,int nofargs);
void pipecommand(char *completepath,char *path,char **argv,int nofargs);
void lessersign(char *p,char **argv,int nofargs);
char *nextToken(char *str,int *point);
void doublePipe(char *path1,char **argv,int nofargs);
void TriplePipe(char *path1,char **argv,int nofargs);
void parsecommand2(char *completepath,char *p,char **argv,int nofargs);
void customcommands(char **argv,int nofargs);
void deleteLineFromFile(char *filename, int index);
void insertLineIntoFile(char *filename, int index, char *text);
char *getLineFromFile(char *filename, int index);

