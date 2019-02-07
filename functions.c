#include "functions.h"

char *getcommand(char *buf,int *noofargs){//gets the first command and updates no of args
	char *delim = " ";
	char *savebuf;
	char *buf2 = (char*)malloc(MAX_SIZE);
	strcpy(buf2,buf);
	//char *command = strtok_r(buf2,delim,&savebuf);
	int parser=0;
	char *command = nextToken(buf2,&parser);
	char *firstarg=command;
	while (firstarg != NULL){
		*noofargs+=1;
		//firstarg = strtok_r (NULL,delim, &savebuf);
		firstarg = nextToken(buf2,&parser);
	}
	return command;
}






char *getfile(char *path,char *firstarg){//returns file path of command entered by user
	char *delim=":";
	char *savepath;
       
	char *k = (char*)malloc (sizeof(char)*100);//final file path
	
	char temp[100] = "/";
	
	strcat (temp, firstarg);
	char *path2 =(char *) malloc (sizeof (char) * MAX_SIZE);

	strcpy (path2, path);
	char *p = strtok_r (path2, delim, &savepath);//tokenize path2
	int parser=0;
	while (p != NULL){
	    strcpy (k, p);
	    strcat (k, temp);
	    if (check (k) == true)
		break;
	    p = strtok_r (NULL, delim, &savepath);
	}
	if(p!=NULL)
	    return k;
	else
	    return NULL;

}




char **getargv(char *buff,int *nofargs,bool *back_pr){//updates backgprocess and returns vector argv
	char buf[MAX_SIZE];
	strcpy(buf,buff);
	char *savebuf2;
	int parser=0;
	char *firstarg = nextToken(buf,&parser);

	char **v = (char **) malloc (sizeof (char *) * (*nofargs + 1));
	for (int i = 0; i < *nofargs; i++){
		if(strcmp(firstarg,"&")==0 && i==*nofargs-1){// background process 
			*back_pr=true;
			*nofargs-=1;
			break;
		}

		v[i] = (char *) malloc (sizeof (char) * MAX_SIZE);

		strcpy (v[i], firstarg);

		firstarg = nextToken(buf,&parser);

	}

	v[*nofargs] = NULL;
	return v;
}		
	




bool check (char *filepath)
{
	if (access (filepath, F_OK) == 0){
		return true;
	}
	return false;

}





void parsecommand(char *completepath,char *path,char **argv,int nofargs) { //indexes all the symbols
	//if(strinarr(argv, "|",nofargs) >= 0)
	//	pipesign(completepath,path,argv,nofargs);
	if(strinarr(argv, "||",nofargs) >= 0)
		doublePipe(path,argv,nofargs);
	if(strinarr(argv,"<",nofargs) >= 0) 
		lessersign(path,argv,nofargs);
	if((nofargs>2) && ((strcmp(argv[nofargs-2], ">") == 0) || (strcmp(argv[nofargs-2], ">>") == 0)))
		greatersign(path,argv,nofargs);
}

int strinarr(char **argv, char *sym, int nofargs) {	// returns index at which str is present in arr, -1 if not found
	int i;
	for(i=0;i<nofargs;i++) {
		if(strcmp(argv[i],sym) == 0)
			return i;
	}
	return -1;
}



char **subarray(char **argv, int start, int end) {

	char **newpath = (char **) malloc (sizeof(char *) * (end - start + 2));
	for(int i=0;i<=end-start;i++) {
		newpath[i] = (char *) malloc (sizeof(char) * MAX_SIZE);
		strcpy(newpath[i],argv[i+start]);
	}
	newpath[end-start+1]=NULL;
	return newpath;
}



char *nextToken(char *str,int *point){
	int len = strlen(str);
	char *temp = (char*)malloc(sizeof(char)*100);
	int i=*point;
	while(i<strlen(str) && str[i]==' ')
		i++;
	if(i>=strlen(str))
		return NULL;	
	if(str[i]=='>' || str[i]=='<' || str[i]=='|' || str[i]==',' || str[i]=='&'){
		temp[0]=str[i];
		if(str[i]=='>' && str[i+1]=='>'){
			temp[1]=str[i+1];
			temp[2]='\0';
			*point=i+2;
			return temp;
		}
		if(str[i]=='|' && str[i+1]=='|'){
			temp[1]=str[i+1];
			if(str[i+2]=='|'){
				temp[2]='|';
				temp[3]='\0';
				*point=i+3;
				return temp;
			}
			temp[2]='\0';
		*point=i+2;
			return temp;
		}
		else{
			temp[1]='\0';
			*point=i+1;
			return temp;
		}
	}
	int j=0;
	while(1){
		if(str[i]==' ' || str[i]=='>' || str[i]=='<' || str[i]=='|' || str[i]==',' || str[i]=='\0' ||str[i]=='&'){
			temp[j]='\0';
			*point=i;
			return temp;
		}
		temp[j]=str[i];
		i++;
		j++;
	}
	*point=i+1;
	return temp;
}


void lessersign(char *path,char **argv,int nofargs) {

	int index = strinarr(argv, "<",nofargs);
	if(access(argv[index + 1], F_OK) == -1) {
		printf("No such file or directory\n");
		exit(0);
	}


	FILE *fp = fopen(argv[index + 1], "r");
	dup2(fileno(fp), 0);
	int grtsign = strinarr(argv, ">", nofargs), appsign=strinarr(argv, ">>", nofargs);
	printf("File %s remapped from %d to %d\n", argv[index+1], fileno(fp), 0);
	

	if(grtsign == -1 && appsign == -1) {
		for(int i =index;i<nofargs;i++)
			argv[i] = NULL;
		printf("\n");
		execv(path,argv);
	}
	

	FILE *fp2;
	if(grtsign >= 0) {
		fp2 = fopen(argv[grtsign+1], "w");
		printf("File %s remapped from %d to %d\n", argv[grtsign+1], fileno(fp2), 1);
	}
	else {
		fp2 = fopen(argv[appsign+1], "a");
		printf("File %s remapped from %d to %d\n", argv[appsign+1], fileno(fp2), 1);
	}
	dup2(fileno(fp2), 1);
	for(int i=index;i<nofargs;i++)
		argv[i] = NULL;
	printf("\n");
	execv(path,argv);
}


void greatersign(char *path,char **argv,int nofargs){
	
	FILE *fp;
	if(strcmp(argv[nofargs-2],">")==0)
		fp = fopen(argv[nofargs-1],"w");
	else
		fp = fopen(argv[nofargs-1],"a");

	printf("File %s remapped from %d to 1 \n", argv[nofargs-1], fileno(fp));
	for(int i=nofargs-2;i<nofargs;i++)
		argv[i] = NULL;

	dup2(fileno(fp), 1);
	printf("\n");
	execv(path,argv);
}


void doublePipe(char *path1,char **argv,int nofargs){
	extern char **environ;
    char *path = getenv ("PATH");
	int MAX_BUF = 10000;
	int fd[2];
	pipe(fd);
	int index = strinarr(argv,"||",nofargs);
	int index2 = strinarr(argv,",",nofargs);
	char **argv1 = subarray(argv,0,index-1);
	char **argv2 = subarray(argv,index+1,index2-1);
	char **argv3 = subarray(argv,index2+1,nofargs-1);
	char *path2 = getfile(path,argv[index+1]);
	char *path3 = getfile(path,argv[index2+1]);
	char buf[MAX_BUF];
	int ret = fork();
	if(ret==0){//child
		close(fd[0]);
		dup2(fd[1],1);//write to pipe fd
		execv(path1,argv1);
	}
	else{
		wait(NULL);
		int fd2[2];
		pipe(fd2);
		int num_b = read(fd[0],buf,MAX_BUF);//copy pipe1 contents to both pipe1&2
		write(fd2[1],buf,num_b);
		write(fd[1],buf,num_b);
		int ret2 = fork();
		if(ret2==0){
			dup2(fd[0],0);//read from pipe 1 and close all others
			close(fd[1]);
			close(fd2[1]);
			close(fd2[0]);
			execv(path2,argv2);
		}	
		else{
			dup2(fd2[0],0);//read from pipe 2 and close all others
			close(fd[1]);
			close(fd[0]);
			close(fd2[1]);
			wait(NULL);
			execv(path3,argv3);
		}
	}
}

//void pipesign(char *completepath,char *path,char **argv,int nofargs) {
/*
	int countofpipe=0;
	for(int i=0;i<nofargs;i++) {
		if(strcmp(argv[i],"|") == 0) {
			countofpipe++;
		}
	}

	int p[countofpipe][2];
	for(int i=0;i<countofpipe;i++)
		pipe(p[countofpipe]);
	int index = strinarr(argv,"|",nofargs),prev=0;
	for(int i=0;i<countofpipe;i++) {
		while(index >= 0) {
			char **newpath = subarray(argv,prev,index);
			int p[2];
			pipe(p);
			dup2(p[1],1);
			
		}
		if(i!=0)
			dup2(p[0],0);
		if(i!=countofpipe-1)
			dup2(p[1],1);
		char *newcommand = subarray(argv,prev,index);
		if(fork()==0) {
			parsecommand(completepath,getfile(completepath,newcommand[0]),newcommand,index-prev+1);
		}
		else {
			wait(NULL);
			prev = index;
			index = strinarr(newcommand,"|",index-prev+1);
		}
	}
*/
	
