#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>

#define MAX_CLIENTS 100
#define MAX_GROUPS 15


void joinGroup(long pid,my_msgbuf buf,long groups[MAX_GROUPS][MAX_GROUPS],long clients[MAX_CLIENTS][MAX_CLIENTS]);
bool checkMem(int pos,long array[MAX_GROUPS][MAX_GROUPS],long key);
void listAllGroups(long pid,int msqid,my_msgbuf buf,long groups[MAX_GROUPS][MAX_GROUPS]);
void listGroup(long pid,int msqid,my_msgbuf buf,long clients[MAX_CLIENTS][MAX_CLIENTS]);
void SendMessage(long pid,int msqid,my_msgbuf buf,long groups[MAX_GROUPS][MAX_GROUPS]);
int getPosGroup(long key,long array[MAX_GROUPS][MAX_GROUPS],int max);
int getPosClient(long key,long array[MAX_CLIENTS][MAX_CLIENTS],int max);
void createGroup(long gpid,long pid,long groups[MAX_GROUPS][MAX_GROUPS],long clients[MAX_CLIENTS][MAX_CLIENTS]);

