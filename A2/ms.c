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

	printf("Server Ready\n");
	
	while(1){
		 if(msgrcv(msqid,&(buf),sizeof(buf),0,0)==-1){//get all the messages 
            perror("msgrcv");
        }
		int msg_type = checkType(buf.mtext);//check type of message,creategrp/joingrp/etc
		if(msg_type==1){					
			list = createGroup(buf,list);
		}
		else if(msg_type==2){
			list = joinGroup(buf,list);
		}
		else{
			SendMessageToGroup(buf,list);		//normal group message
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
		newG->member=NULL;
		newG->next= NULL;
		return newG;
}
	

groupsList* createGroup(my_msgbuf buf,groupsList *list){	//creating a new group
	if(checkGroupExists(buf.gpid,list)==true){
		printf("group already exists\n");
		return list;
	}
	grpMem *newM=newMember(buf.spid);
	if(list==NULL){
		list = newGroup(buf.gpid);
		list->member=newM;
	}
	else{
		groupsList *prevG=list;
		groupsList *nextG=list->next;
		while(nextG!=NULL){						//add to end of the groupsList
			prevG=nextG;
			nextG=nextG->next;
		}
		prevG->next = newGroup(buf.gpid);
		prevG->next->member=newM;
	}
	printf("group %ld created\n",buf.gpid);
	return list;
}	
		
void SendMessageToGroup(my_msgbuf buf,groupsList *list){ 	//normal message sent to group
}
groupsList *joinGroup(my_msgbuf buf,groupsList *list){return list;}



bool checkGroupExists(int gpid,groupsList *list){ 	//returns true if group already exists
	groupsList *temp=list;
	while(temp!=NULL){
		if(temp->gpid==gpid)
			return true;
		temp=temp->next;
	}
	return false;

}


int checkType(char* text){return 1;} 		//returns type of message 1->create,2->join etc
