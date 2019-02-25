#include "chat.h"
#include "server.h"

int main(void){
	
	groupsList *list = NULL;
	struct my_msgbuf buf;
	int msqid;
	key_t key;

	if((key = ftok(MSGQ_PATH,'A'))==-1){	//generate key
		perror("ftok:");
		exit(1);
	}

	if((msqid = msgget(key,IPC_CREAT | 0644))==-1){ 	//create msgqueue
		perror("msgget:");
		exit(1);
	}
	int gpid;
	printf("Server Ready\n");
	
	while(1){

	//	 while(msgrcv(msqid,&(buf),sizeof(buf),1,0)==0);//get all the messages 

		 if(msgrcv(msqid,&(buf),sizeof(buf),1,0)==-1){//get all the messages 
            perror("msgrcv");
        }
		int msg_type = checkType(buf.mtext,&gpid);//check type of message,creategrp/joingrp/etc
		if(msg_type==1){					
			list = createGroup(gpid,buf.spid,list);
		}
		else if(msg_type==2){
			list = joinGroup(gpid,buf.spid,list);
		}
		else if(msg_type==3){
			listGroups(msqid,buf,list);
		}
		else{
			SendMessage(msqid,buf,list);		//normal group message
		}
			//printf("Message by %ld:%s\n",buf.spid,buf.mtext);

		}
}
	
 
grpMem *newMember(int pid){			//new GrpMem structure
	grpMem *newM = (grpMem*)malloc(sizeof(grpMem));
	newM->pid=pid;
	newM->nextMem=NULL;
	return newM;
}

groupsList *newGroup(int gpid){		//new groupsList structure
		groupsList *newG = (groupsList*)malloc(sizeof(groupsList));
		newG->gpid=gpid;
		newG->head=NULL;
		newG->next= NULL;
		return newG;
}
	

groupsList* createGroup(int gpid,int pid,groupsList *list){	//creating a new group
	groupsList *grp = FindGroup(gpid,list); 	//get group if it already exists
	if(grp!=NULL){
		printf("group already exists\n");
		return list;
	}
	grpMem *newM=newMember(pid);
	if(list==NULL){
		list = newGroup(gpid);
		list->head=newM;
	}
	else{
		groupsList *prevG=list;
		groupsList *nextG=list->next;
		while(nextG!=NULL){						//add to end of the groupsList
			prevG=nextG;
			nextG=nextG->next;
		}
		prevG->next = newGroup(gpid);
		prevG->next->head=newM;
	}
	printf("group %d created\n",gpid);
	return list;
}	
		

void SendMessage(int msqid,my_msgbuf buf,groupsList *list){ 	//normal message sent to group
	groupsList *temp = list;
	while(temp!=NULL){
		if(checkGroupMember(temp,buf.spid) == true){
			SendMessageToGroup(msqid,temp,buf);
		}
		temp=temp->next;
	}
}
	
void SendMessageToGroup(int msqid,groupsList *list,my_msgbuf buf){
	if(list==NULL)
		return;
	grpMem *temp = list->head;
	printf("Sending msg by %ld to group %d\n",buf.spid,list->gpid);
	while(temp!=NULL){
		if(temp->pid==buf.spid){
			temp=temp->nextMem;
			continue;
		}
		kill(temp->pid,SIGUSR1);
		buf.mtype = temp->pid;
		msgsnd(msqid,&buf,sizeof(buf),0);
		temp=temp->nextMem;
	}
}

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

/*
void listGroups(int msqid,my_msgbuf buf,groupsList *list){
	printf("%ld belongs to groups:\n",buf.spid);
	groupsList *temp = list;
	buf.mtype = buf.spid;
	char *str = "";
	char str2[100];
	while(temp!=NULL){
		if(checkGroupMember(temp,buf.spid) == true){
			sprintf(str2, "%ld",buf.spid);
			str = strcat(str,str2);
			str = strcat(str," ");
			//printf("%d\n",temp->gpid);
		}
		temp=temp->next;
	}
	strcpy(buf.mtext,str);
	msgsnd(msqid,&buf,sizeof(buf),0);
		
}
*/

void listGroups(int msqid,my_msgbuf buf,groupsList *list){
	char *str = (char*)malloc(2000);
	strcpy(str,"Avalable groups:\n");
	groupsList *temp = list;
	char str2[2000];
	while(temp!=NULL){
			sprintf(str2,"%d\t",temp->gpid);
			strcat(str,str2);
			temp=temp->next;
	}
	buf.mtype = buf.spid;
	strcpy(buf.mtext,str);
	kill(buf.spid,SIGUSR1);
	msgsnd(msqid,&buf,sizeof(buf),0);
}

groupsList *joinGroup(int gpid,int pid,groupsList *list){
	groupsList *grp = FindGroup(gpid,list);
	if(grp==NULL){
		printf("Group :%d doesnt exist,first create\n",gpid);
		return list;
	}
	if(checkGroupMember(grp,pid) == true){
		printf("Already a member\n");
		return list;
	}
	grpMem * newM = newMember(pid);
	newM->nextMem = grp->head;
	grp->head=newM;
	printf("%d joined group %d\n",pid,gpid);
	return list;
}

bool checkGroupMember(groupsList *list,int pid){
	if(list==NULL)
		return false;
	grpMem *temp = list->head;
	while(temp!=NULL){
		if(temp->pid==pid)
			return true;
		temp=temp->nextMem;
	}
	return false;
}
		
groupsList *FindGroup(int gpid,groupsList *list){ 	//returns groupList pointer if group already exists
	groupsList *temp=list;
	while(temp!=NULL){
		if(temp->gpid==gpid)
			return temp;
		temp=temp->next;
	}
	return NULL;
}


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
