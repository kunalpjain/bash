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
			listGroups(buf.spid,list);
		}
		else{
			SendMessage(msqid,buf,list);		//normal group message
		}
			printf("Message by %ld:%s\n",buf.spid,buf.mtext);

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
	while(temp!=NULL){
		buf.mtype = temp->pid;
		msgsnd(msqid,&buf,sizeof(buf),0);
		temp=temp->nextMem;
	}
}

void listGroups(int pid,groupsList *list){
	printf("%d belongs to groups:\n",pid);
	groupsList *temp = list;
	while(temp!=NULL){
		if(checkGroupMember(temp,pid) == true){
			printf("%d\n",temp->gpid);
		}
		temp=temp->next;
	}
		
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
	char *temp = (char*)malloc(200);
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
}
