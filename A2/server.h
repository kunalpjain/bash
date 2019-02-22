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
    grpMem *member;					//this gpid members list
}groupsList;


void SendMessageToGroup(my_msgbuf buf,groupsList *list);
groupsList *joinGroup(my_msgbuf,groupsList *list);
groupsList *createGroup(my_msgbuf buf,groupsList *list);
groupsList *newGroup(int gpid);
grpMem *newMember(int pid);
bool checkGroupExists(int gpid,groupsList *list);
int checkType(char* text);
