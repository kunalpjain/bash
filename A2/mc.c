#include "chat.h"
#include "server.h"
int msqid;
void handler (int signo){

	struct my_msgbuf buf;
	if(msgrcv(msqid,&(buf),sizeof(buf),getpid(),0)==-1){//display messages in the group
		//perror("msgrcv");
	}
	printf("\n%ld:%s\n",buf.spid,buf.mtext);
	printf("You:");
    fflush(stdout);
}
	
int main(void){
	struct my_msgbuf buf;
	//int msqid;
	
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
	signal(SIGUSR1,handler);
	while(1){
		printf("You:");
		buf.spid = getpid();//registering sender pid
		buf.mtype = 1;//for sending to server
    	fflush(stdin);
    	fflush(stdout);
		memset(buf.mtext, 0,MSG_SIZE);
	//	strcpy(buf.mtext,"");
	//	printf("buf.mtext:%s\n",buf.mtext);
		read (0, buf.mtext,MSG_SIZE);
		if(msgsnd(msqid,&(buf),sizeof(buf),0)==-1){//send message to server
			perror("msgsnd");
		}
/*
		int gpid;
		int type = checkType(buf.mtext,&gpid);
		if(type<=2)
			continue;
*/
	}
}
/*
int checkType(char* text,int *gpid){            //returns type of message 1->create,2->join etc
    char *temp = (char*)malloc(MSG_SIZE);
    strcpy(temp,text);
    const char *delim = " ";
    temp = strtok(temp,delim);
    if(strcmp(temp,"create")==0){
        temp=strtok(NULL,delim);
        *gpid = atoi(temp);
        return 1;
    }
    if(strcmp(temp,"join")==0){
        temp=strtok(NULL,delim);
        *gpid = atoi(temp);
        return 2;
    }
    if(strcmp(temp,"list")==0){
        return 3;
    }
    return 4;
}*/
