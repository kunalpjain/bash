#include "chat.h"
#include "server.h"
#include "client.h"

int main() {
	struct my_msgbuf buf;
	key_t key;
	int p[2], msqid;
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

	printf("Welcome %s, retrieving you messages...\n", uname);
	long my_id = ntoid(uname);
	strcpy(buf.mtext, "\0");
	while(msgrcv(msqid, &buf, sizeof(buf), my_id, IPC_NOWAIT)!=-1) {
		printf("Msg->\t%s\n", buf.mtext);
	}
	
	printOptions(1);	
	int ret = fork();
	if(ret==0) {
		for(;;) {
			//pause();
			read(p[0], &charbuf, sizeof(charbuf));
			//printf("in child\n");
			if(msgrcv(msqid, &buf, sizeof(buf), my_id, IPC_NOWAIT) != -1)
				printf("\nMsg -> %s\n", buf.mtext);
			write(p[1], &charbuf, sizeof(charbuf));
			//printOptions(1);
		}
	}
	else {
		for(;;) {
			char command[MSG_SIZE], backup[MSG_SIZE];
			scanf(" %[^\n]", command);
			strcpy(backup, command);
			if(strlen(strtok(backup, " ")) > 1) {
				read(p[0], &charbuf, sizeof(charbuf));
				buf.option = 5;										// list my groups
				sprintf(buf.mtext, "User %s is retrieving list of all joined groups\n", uname);
				msgsnd(msqid, &buf, sizeof(buf), 0);
<<<<<<< HEAD
				while(msgrcv(msqid, &buf, sizeof(buf), my_id, 0), buf.option != 5) {			//queues all non 5 msgs
					buf.mtype = my_id;
					msgsnd(msqid, &buf, sizeof(buf), 0);
				}
=======
>>>>>>> 550be853ac688158308bcdc8889c960bb5085c43
				printf("You are in the following groups:\n%s", buf.mtext);
				printf("Enter the group number to send to (or empty for all): ");
				fflush(stdout);
				char groupnumber[NAME_SIZE];
				scanf(" %s", groupnumber);
				if(strlen(groupnumber) == 0)
					buf.gpid = 0;
				else
					buf.gpid = atoi(groupnumber);
				buf.option = 6;										// send message
				buf.mtype =1;
				strcpy(buf.mtext, command);
				msgsnd(msqid, &buf, sizeof(buf), 0);
				write(p[1], &charbuf, sizeof(charbuf));
				continue;
			}
			long gid;
			switch(command[0]) {
				case 'l':
					read(p[0], &charbuf, sizeof(charbuf));
					printf("in L\n");
					buf.option = 2;									// list all groups
					sprintf(buf.mtext, "User %s is retrieving list of all groups\n", uname);
					buf.mtype = 1;
					printf("msg sending...\n");
					msgsnd(msqid, &buf, sizeof(buf), 0);
					printf("msg sent.\n");
					msgrcv(msqid, &buf, sizeof(buf), my_id, 0);
					printf("List of all groups:\n%s", buf.mtext);
					write(p[0], &charbuf, sizeof(charbuf));
					break;
				case 'c':
					buf.option = 3;									// create group
					gid = atoi(command+2);
					buf.gpid = gid;
					sprintf(buf.mtext, "User %s attempting to create group %ld\n", uname, gid);
					break;
				case 'j':
					buf.option = 4;
					gid = atoi(command+2);
					buf.gpid = gid;
					sprintf(buf.mtext, "User %s attempting to join group %ld\n", uname, gid);
					break;
			}
			printf("-> ");
			fflush(stdout);
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
