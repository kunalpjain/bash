#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>
#include<signal.h>
#include<ctype.h>

#define MSGQ_PATH "chat.h"
#define MSG_SIZE 2000
#define NAME_SIZE 20

typedef struct my_msgbuf{
	long mtype;		//1->normal msg,2->create grp,3->join group
	char mtext[MSG_SIZE];
	char uname[NAME_SIZE];
	int option;
	long gpid;
}my_msgbuf;


