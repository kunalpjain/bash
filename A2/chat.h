#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<unistd.h>

#define MSGQ_PATH "chat.h"

struct my_msgbuf{
	long mtype;
	char mtext[200];
	long spid;//sender pid
	long gpid;//group id
};
