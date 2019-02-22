#include "chat.h"

int main(void){
	struct my_msgbuf buf;
	int msqid;
	
	key_t key;
	printf("Pid:%d\n",getpid());
	if((key=ftok(MSGQ_PATH,'A'))==-1){
		perror("ftok:");
		exit(1);
	}
	
	if((msqid = msgget(key,0644 | IPC_CREAT))==-1){
		perror("msgget:");
		exit(1);
	}
	while(1){
		printf("You:\n");
		buf.spid = getpid();//registering sender pid
		buf.mtype = 1;//for sending to server
    	fflush(stdin);
    	fflush(stdout);
		read (0, buf.mtext,200);
		if(msgsnd(msqid,&(buf),sizeof(buf),0)==-1){//send message to server
	//		perror("msgsnd");
		}
		if(msgrcv(msqid,&(buf),sizeof(buf),getpid(),IPC_NOWAIT)==-1){//display messages in the group
			perror("msgrcv");
		}
		printf("%ld:%s\n",buf.spid,buf.mtext);
	}
}
