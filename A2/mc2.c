#include "chat.h"
#include "server.h"
#include "client.h"

int main() {
	struct my_msgbuf buf;
	key_t key;
	int p[2];
	char uname[20], charbuf='a';
	pipe(p);
	write(p[1], &charbuf, sizeof(charbuf));
	if((key=ftok(MSGQ_PATH,'A'))==-1){
		perror("ftok:");
		exit(1);
	}
	
	if((msqid = msgget(key,0644 | IPC_CREAT))==-1){
		perror("msgget:");
		exit(1);
	}

	printf("Enter your username: ");
	fflush(stdout);
	scanf("%s", uname);

	buf.mtype=1;
	buf.option=1;
	sprintf(buf.mtext, "User %s is online\n", uname);
	strcpy(buf.uname, uname);
	msgsnd(msqid, &buf, sizeof(buf), 0);

	printf("Welcome %s, retrieving you messages...\n");
	long my_id = ntoid(uname);
	while(msgrcv(msqid, &buf, sizeof(buf), my_id, IPC_NOWAIT)) {
		printf("Msg ->\t%s\n", buf.mtext);
	}
	
	printOptions(1);	

	if(fork()==0) {
		for(;;) {
			msgrcv(msqid, &buf, sizeof(buf), my_id, 0);
			read(p[0], &charbuf, sizeof(charbuf));
			printf("\nMsg -> %s\n", buf.mtext);
			write(p[1], &charbuf, sizeof(charbuf));
		}
	}
	else {
		for(;;) {
			char command[MSG_SIZE], backup[MSG_SIZE];
			gets(command);
			strcpy(backup, command);
			if(strlen(strtok(backup, " ")) > 1) {
				read(p[0], &charbuf, sizeof(charbuf));
				buf.option = 5;										// list my groups
				sprintf(buf.mtext, "User %s is retrieving list of all joined groups\n", uname);
				msgsnd(msqid, &buf, sizeof(buf), 0);
				msgrcv(msqid, &buf, sizeof(buf), my_id, 0);
				printf("You are in the following groups:\n%s", buf.mtext);
				printf("Enter the group number to send to (or empty for all): ");
				fflush(stdout);
				gets(command);
				if(strlen(command) == 0) {
					strcpy(backup, buf.mtext);
					for(;;) {
						
					}
				}
			}
			switch(command[0]) {
				case 'l':
					buf.option = 2;									// list all groups
					sprintf(buf.mtext, "User %s is retrieving list of all groups\n", uname);
					break;
				case 'c':
					buf.option = 3;									// create group
					long gid = atoi(command+2);
					buf.g2id = gid;
					sprintf(buf.mtext, "User %s attempting to create group %ld\n", uname, gid);
			}
		}
	}
	
}

void printOptions(int extra) {
	printf("Options available for you are:\n");
	printf("-> l\t\t\t= list\n");
	printf("-> c <group_number>\t= create\n");
	printf("-> j <group_number>\t= join\n");
	if(extra)
		printf("-> ");
	fflush(stdout);
}
