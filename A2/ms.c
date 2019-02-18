#include "chat.h"

int main(void){
	struct my_msgbuf buf;
	int msqid;
	key_t key;

	if((key = ftok(MSGQ_PATH,'A'))==-1){
		perror("ftok:");
		exit(1);
	}

	if((msqid = msgget(key,IPC_CREAT | 0644))==-1){
		perror("msgget:");
		exit(1);
	}

	printf("Server Ready\n");
	
	while(1){}
}
	

