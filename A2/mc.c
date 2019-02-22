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
		buf.gpid = 1;//for now assuming all group 1,later we will extend
		buf.mtype = 1;//for sending to server
    	fflush(stdin);
    	fflush(stdout);
		scanf("%s",buf.mtext);
		if(msgsnd(msqid,&(buf),sizeof(buf),0)==-1){//send message to server
			perror("msgsnd");
		}
//		if(msgrcv(msqid,&(buf),sizeof(buf),getpid(),IPC_NOWAIT)==-1){//display messages in the group
//			perror("msgrcv");
//		}
//		printf("%ld:%s\n",buf.spid,buf.mtext);
	}
}
