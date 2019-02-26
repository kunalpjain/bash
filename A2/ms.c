#include "chat.h"
#include "server.h"

int main(void){

	struct my_msgbuf buf;
	int msqid;
	key_t key;

	//creating data structures for clients and groups
/*
	long **clients = (long**)malloc(sizeof(long*)*MAX_CLIENTS);
	for(int i=0;i<MAX_CLIENTS;i++)
		clients[i]=(long*)malloc(sizeof(long)*MAX_CLIENTS);

	long **groups = (long**)malloc(sizeof(long*)*MAX_GROUPS);
	for(int i=0;i<MAX_CLIENTS;i++)
		groups[i]=(long*)malloc(sizeof(long)*MAX_GROUPS);
*/
	long clients[MAX_CLIENTS][MAX_CLIENTS];
	long groups[MAX_GROUPS][MAX_GROUPS];

	//initializing clients and groups to 0

	memset(clients,0,sizeof(long)*MAX_CLIENTS*MAX_CLIENTS);
	memset(groups,0,sizeof(long)*MAX_GROUPS*MAX_GROUPS);

	if((key = ftok(MSGQ_PATH,'A'))==-1){	//generate key
		perror("ftok:");
		exit(1);
	}

	if((msqid = msgget(key,IPC_CREAT | 0644))==-1){ 	//create msgqueue
		perror("msgget:");
		exit(1);
	}
	printf("Server Ready\n");
	long pid;
	while(1){

   	 	msgrcv(msqid,&(buf),sizeof(buf),1,0);//get all the messages 
		pid = ntoid(buf.uname);// id of the client
		if(buf.option==1){				// user is online
			 printf("%s",buf.mtext);
		}
		else if(buf.option==2){				//list all groups
			printf("%s",buf.mtext);
			listAllGroups(pid,msqid,buf,groups);
		}
		else if(buf.option==3){				//create group
			pid = ntoid(buf.uname);
			printf("%s",buf.mtext);
			createGroup(buf.gpid,pid,groups,clients);
		}
		else if(buf.option==4){				//join group
			 joinGroup(pid,buf,groups,clients);
			 printf("%s",buf.mtext);
		}
		else if(buf.option==5){				//list specific groups
			printf("option 5\n");
			printf("%s",buf.mtext);
			listGroup(pid,msqid,buf,clients);
		}
		else{
			if(buf.gpid==0){			//send message to all groups
				int cli = getPosClient(pid,clients,MAX_CLIENTS);
				int nos = clients[cli][1];
				for(int i=0;i<nos;i++){
					buf.gpid = clients[cli][i+2];
					SendMessage(pid,msqid,buf,groups);		//normal group message
				}
			}
			else
				SendMessage(pid,msqid,buf,groups);		//normal group message
		}
	}
}
	
void createGroup(long gpid,long pid,long groups[MAX_GROUPS][MAX_GROUPS],long clients[MAX_CLIENTS][MAX_CLIENTS]){
	if(getPosGroup(gpid,groups,MAX_GROUPS) != -1){			//if group already exists,return
			printf("group already exists\n");
			return;
	}
	int i=0;
	while(i<MAX_GROUPS && groups[i][0]!=0) i++;			//adding gpid to clients
	groups[i][0]=gpid;								//adding pid to groups
	groups[i][1]=1;
	groups[i][2]=pid;
	i=0;
	while(i<MAX_CLIENTS && clients[i][0]!=0){			//adding gpid to clients
		if(clients[i][0]==pid){
			clients[i][1]++;
			int pos = clients[i][1]+1;
			clients[i][pos] = gpid;
			return;
		}
		i++;
	}
	clients[i][0] = pid;
	clients[i][1]=1;
	clients[i][2]=gpid;
}	
		
int getPosClient(long key,long array[MAX_CLIENTS][MAX_CLIENTS],int max){//returns pos of key in given array[pos][0]
	int i=0;
	while(i<max && array[i][0]!=0){
		if(array[i][0]==key){
			return i;
		}
		i++;
	}
	return -1;						//-1 if not found
}


int getPosGroup(long key,long array[MAX_GROUPS][MAX_GROUPS],int max){//returns pos of key in given array[pos][0]
	int i=0;
	while(i<max && array[i][0]!=0){
		if(array[i][0]==key){
			return i;
		}
		i++;
	}
	return -1;						//-1 if not found
}

void SendMessage(long pid,int msqid,my_msgbuf buf,long groups[MAX_GROUPS][MAX_GROUPS]){ 	//normal message sent to group
	int grp = getPosGroup(buf.gpid,groups,MAX_GROUPS);			
	if(groups[grp][1]==-1){
		printf("Group not present\n");
		return ;
	}
	int no_members = groups[grp][1];
	printf("Sending msg by %s to group %ld\n",buf.uname,buf.gpid);
	for(int i=0;i<no_members;i++){
		pid = groups[grp][i+2];
		buf.mtype = pid;
		msgsnd(msqid,&buf,sizeof(buf),0);
	}
}
/*
int no_tokens(char *text){
	char *str=(char*)malloc(MSG_SIZE);
	strcpy(str,text);
	char *delim=" ";
	str=strtok(str,delim);
	int count=0;
	while(str!=NULL){
			str=strtok(NULL,delim);
			count++;
	}
	return count;
}

*/

void listGroup(long pid,int msqid,my_msgbuf buf,long clients[MAX_CLIENTS][MAX_CLIENTS]){
	int cli = getPosClient(pid,clients,MAX_CLIENTS);
	int i=0;
	char str[200]="";
	char str2[200];
	int nos = clients[cli][1];
	for(int i=0;i<nos;i++){
		sprintf(str2, "%ld\n",clients[cli][i+2]);
		strcat(str,str2);
	}	
	buf.mtype = pid;
	strcpy(buf.mtext,str);
	printf("sent\n");
	msgsnd(msqid,&buf,sizeof(buf),0);
	printf("sent\n");
}

void listAllGroups(long pid,int msqid,my_msgbuf buf,long groups[MAX_GROUPS][MAX_GROUPS]){
	int i=0;
	char str[200] = "";
	char str2[200];
	while(i<MAX_GROUPS && groups[i][0]!=0){
			sprintf(str2, "%ld\n",groups[i][0]);
			strcat(str,str2);
			i++;
	}
	if(i==0)		//no groups available
		return;
	buf.mtype = pid;
	strcpy(buf.mtext,str);
	msgsnd(msqid,&buf,sizeof(buf),0);
}

bool checkMem(int pos,long array[MAX_GROUPS][MAX_GROUPS],long key){
	int nos = array[pos][1];
	for(int i=0;i<nos;i++){
		if(array[pos][i+2]==key){
			return true;
		}
	}
	return false;
}


void joinGroup(long pid,my_msgbuf buf,long groups[MAX_GROUPS][MAX_GROUPS],long clients[MAX_CLIENTS][MAX_CLIENTS]){
	int grp = getPosGroup(buf.gpid,groups,MAX_GROUPS);
	int cli = getPosClient(pid,clients,MAX_CLIENTS);
	if(checkMem(grp,groups,pid) == true){
		printf("Already a member\n");
		return;
	}
	//add client to group
	groups[grp][1]++;
	int mem = groups[grp][1]+1;
	groups[grp][mem]=pid;

	//add group to client

	clients[cli][1]++;
	mem = clients[cli][1]+1;
	clients[cli][mem]=buf.gpid;
	printf("%s joined group %ld\n",buf.uname,buf.gpid);
}

/*
int checkType(char* text,int *gpid){            //returns type of message 1->create,2->join etc
	int no_token = no_tokens(text);
	char *temp = (char*)malloc(MSG_SIZE);
	if(text[strlen(text)-1] =='\n')
		text[strlen(text)-1]=0;
	strcpy(temp,text);
	const char *delim = " ";
	temp = strtok(temp,delim);
	if(temp==NULL)
		return 4;
	if(no_token == 2 &&strcmp(temp,"create")==0){
		temp=strtok(NULL,delim);
		*gpid = atoi(temp);
		return 1;
	}
	if(no_token==2 && strcmp(temp,"join")==0){
		temp=strtok(NULL,delim);
		*gpid = atoi(temp);
		return 2;
	}
	if(no_token ==1  && strcmp(temp,"list")==0){
		return 3;
	}
	return 4;
}
*/
