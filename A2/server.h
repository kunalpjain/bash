#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<unistd.h>
#include<string.h>

typedef struct grpMem{              //list of all pids in this group
    int pid;
    struct grpMem *nextMem;			//next member of the list
}grpMem;

typedef struct groupsList{          //list of all groups
    int gpid;
    struct groupsList *next;		// next group
    grpMem *head;					//this gpid members list
}groupsList;


groupsList *joinGroup(int gpid,int pid,groupsList *list);
groupsList *createGroup(int gpid,int pid,groupsList *list);
groupsList *newGroup(int gpid);
grpMem *newMember(int pid);
int checkType(char* text,int *gpid);
groupsList *FindGroup(int gpid,groupsList *list);
bool checkGroupMember(groupsList *list,int pid);
void SendMessageToGroup(int msqid,groupsList *list,my_msgbuf buf);
void SendMessage(int msqid,my_msgbuf buf,groupsList *list);
void listGroups(int msqid,my_msgbuf buf,groupsList *list);
int no_tokens(char *text);
