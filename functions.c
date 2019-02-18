#include "functions.h"

char lookuptable[MAX_SIZE][MAX_SIZE];
void handler(int signo) {
	for(int i=0;i<10;i++) {
		strcpy(lookuptable[i],"ls -l");
	}
	char *completepath = getenv ("PATH");
	int index,nofargs=0;
	bool var=false;
	printf("\nEnter your desired command: ");
	fflush(stdout);
	scanf("%d",&index);
	char *commandstring = lookuptable[index];
	char *firstarg = getcommand(commandstring,&nofargs);
	char *pathofcommand = getfile(completepath,firstarg);
	char **argv = getargv(commandstring,&nofargs,&var);
	if(fork()==0)
		parsecommand(completepath, pathofcommand, argv, nofargs);
	else {
		wait(NULL);
		fflush(stdout);
		fflush(stdin);
	}
}

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
	// write(fileno(stdout),"%s\n",10);
	if(strinarr(argv, "|",nofargs,0) >= 0)
	//	pipesign(completepath,path,argv,nofargs);
		pipecommand(completepath,path,argv,nofargs);
	if(strinarr(argv, "||",nofargs,0) >= 0)
		doublePipe(path,argv,nofargs);
	if(strinarr(argv, "|||",nofargs,0) >= 0)
		TriplePipe(path,argv,nofargs);
	if(strinarr(argv,"<",nofargs,0) >= 0) 
		lessersign(path,argv,nofargs);
	if((nofargs>2) && ((strcmp(argv[nofargs-2], ">") == 0) || (strcmp(argv[nofargs-2], ">>") == 0)))
		greatersign(path,argv,nofargs);
	else
		execv(path,argv);
}

int strinarr(char **argv, char *sym, int nofargs,int start_index) {	// returns index at which str is present in arr, -1 if not found
	int i;
	for(i=start_index;i<nofargs;i++) {
		if(argv[i]!=NULL && strcmp(argv[i],sym) == 0)
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

	int index = strinarr(argv, "<",nofargs,0);
	if(access(argv[index + 1], F_OK) == -1) {
		printf("No such file or directory\n");
		exit(0);
	}


	FILE *fp = fopen(argv[index + 1], "r");
	dup2(fileno(fp), 0);
	int grtsign = strinarr(argv, ">", nofargs,0), appsign=strinarr(argv, ">>", nofargs,0);
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
	int index = strinarr(argv,"||",nofargs,0);
	int index2 = strinarr(argv,",",nofargs,index+1);
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

void pipesign(char *completepath,char *path,char **argv,int nofargs) {

	int countofpipe=0;
	for(int i=0;i<nofargs;i++) {//counting number of pipes
		if(strcmp(argv[i],"|") == 0) {
			countofpipe++;
		}
	}


	int p[countofpipe+1][2];
	for(int i=0;i<=countofpipe;i++)
		pipe(p[i]);
	int index = strinarr(argv,"|",nofargs,0),prev=0;

	for(int i=0;i<=countofpipe;i++) {

		char **newcommand = subarray(argv,prev,index-1);
		
		if(fork()==0) {
			if(i!=0) {
				// printf("changing read pipe for %s\n", newcommand[0]);
				dup2(p[i-1][0],0);
				// printf("changed read\n");
			}
			if(i!=countofpipe) {
				// printf("changing write pipe for %s\n", newcommand[0]);
				dup2(p[i][1],1);
				// printf("changed write\n");
			}
			parsecommand(completepath,getfile(completepath,newcommand[0]),newcommand,index-prev+1);
		}
		else {
			close(p[i-1][0]);
			close(p[i][1]);
			wait(NULL);
			prev = index+1;
			if(i!=countofpipe-1)
				index = strinarr(newcommand,"|",index-prev,0);
			else
				index = nofargs;
		}
	}
	exit(0);

}

void TriplePipe(char *path1,char **argv,int nofargs){
	extern char **environ;
    char *path = getenv ("PATH");
	int MAX_BUF = 10000;
	int fd[2];
	pipe(fd);
	int index = strinarr(argv,"|||",nofargs,0);
	int index2 = strinarr(argv,",",nofargs,index+1);
	int index3 = strinarr(argv,",",nofargs,index2+1);
	char **argv1 = subarray(argv,0,index-1);
	char **argv2 = subarray(argv,index+1,index2-1);
	char **argv3 = subarray(argv,index2+1,index3-1);
	char **argv4 = subarray(argv,index3+1,nofargs-1);
	char *path2 = getfile(path,argv[index+1]);
	char *path3 = getfile(path,argv[index2+1]);
	char *path4 = getfile(path,argv[index3+1]);
	char buf[MAX_BUF];
	int ret = fork();
	if(ret==0){//child
		close(fd[0]);
		dup2(fd[1],1);//write to pipe fd
		execv(path1,argv1);//execute before ||| and store in pipe fd
	}
	else{
		wait(NULL);
		int fd2[2];
		int fd3[2];
		pipe(fd2);
		pipe(fd3);
		int num_b = read(fd[0],buf,MAX_BUF);//copy pipe1 contents to pipe 1,2 and 3
		write(fd2[1],buf,num_b);
		write(fd3[1],buf,num_b);
		write(fd[1],buf,num_b);

		close(fd[1]);				//close all write end pipes
		close(fd2[1]);
		close(fd3[1]);

		int ret2 = fork();
		if(ret2==0){
			dup2(fd[0],0);//read from pipe fd and close all others
			close(fd3[0]);
			close(fd2[0]);
			execv(path2,argv2);//execute 1st command,write to stdout
		}	
		else{
			dup2(fd2[0],0);//read from pipe fd2 and close all others
			close(fd[0]);
			wait(NULL);
			int ret3 = fork();
			if(ret3==0){
				dup2(fd3[0],0);//read from pipe fd3 and close all others
				close(fd[0]);
				close(fd2[0]);
				execv(path3,argv3);//execute 2nd command ,write to stdout
			}
			wait(NULL);
			execv(path4,argv4);//exec 3rd command,write to stdout
		}
	}
}
//new pipe
void pipecommand(char *completepath,char *path,char **argv,int nofargs) {

	int countofpipe=0;
	for(int i=0;i<nofargs;i++) {//counting number of pipes
		if(strcmp(argv[i],"|") == 0) {
			countofpipe++;
		}
	}

		int p[countofpipe][2];
		for(int i=0;i<countofpipe;i++)//create that many pipes
			pipe(p[i]);
		int prev_index=-1;
		int index;

	for(int i=0;i<=countofpipe;i++) {
		if(i!=countofpipe)
			index = strinarr(argv,"|",nofargs,prev_index+1);
		else
			index = nofargs;

		char *path1 = getfile(completepath,argv[prev_index+1]);
		char **argv1 = subarray(argv,prev_index+1,index-1); 
		if(fork()==0) {//child
			if(i!=0) {//read from prev pipe
				dup2(p[i-1][0],0);
				close(p[i-1][1]);
			}
			if(i!=countofpipe) {//write to next pipe
				dup2(p[i][1],1);
			}
			//execv(path1,argv1);
			parsecommand(completepath,path1,argv1,index-prev_index-1);
		}
		else {
			if(i!=0){//close the pipe_w for child to read
				close(p[i-1][0]);
				close(p[i-1][1]);
			}
			wait(NULL);
			prev_index = index;
		}
	}
	exit(0);

}
