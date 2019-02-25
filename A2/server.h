#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<unistd.h>
#include<string.h>

#define MAX_CLIENTS 100
#define MAX_GROUPS 15


void joinGroup(long pid,my_msgbuf buf,long **groups,long **clients);
bool checkMem(int pos,long **array,long key);
void listAllGroups(long pid,int msqid,my_msgbuf buf,long **groups);
void listGroup(long pid,int msqid,my_msgbuf buf,long **clients);
void SendMessage(int msqid,my_msgbuf buf,long **groups);
int getPos(long key,long **array,int max);
void createGroup(long gpid,long pid,long **groups,long **clients);

