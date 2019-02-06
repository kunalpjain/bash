#include "functions.h"

char *getcommand(char *buf,int *noofargs){//gets the first command and updates no of args
	char *delim = " ";
	char *savebuf;
	char *buf2 = (char*)malloc(MAX_SIZE);
	strcpy(buf2,buf);
	char *command = strtok_r(buf2,delim,&savebuf);
	char *firstarg=command;
	while (firstarg != NULL){
		*noofargs+=1;
		firstarg = strtok_r (NULL,delim, &savebuf);
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

char **getargv(char *buff,int nofargs,bool *back_pr){//updates backgprocess and returns vector argv
	char buf[MAX_SIZE];
	strcpy(buf,buff);
    char *delim2 = " ";
    char *savebuf2;
    char *firstarg = strtok_r (buf, delim2, &savebuf2);
    
    char **v = (char **) malloc (sizeof (char *) * (nofargs + 1));
        for (int i = 0; i < nofargs; i++){
            if(strcmp(firstarg,"&")==0 && i==nofargs-1){// background process 
                *back_pr=true;
                nofargs--;
            }
            v[i] = (char *) malloc (sizeof (char) * MAX_SIZE);
            
            strcpy (v[i], firstarg);
            
            firstarg = strtok_r (NULL, delim2, &savebuf2);
        
        }
        
    v[nofargs] = NULL;
    return v;
}		
	
bool check (char *filepath)
{
	if (access (filepath, F_OK) == 0){
      		return true;
    	}
	return false;

}

void parsecommand(char *path,char **argv,int nofargs) { //indexes all the symbols
	/*if(strinarr(argv,"<",nofargs) == 1)
		lessersign(path,argv,nofargs);
	if((strcmp(argv[nofargs-2], ">") == 0) || (strcmp(argv[nofargs-2], ">>") == 0))
		greatersign(path,argv,nofargs);*/
	printf("fun %d\n", strinarr(argv,"<",nofargs));
}


int strinarr(char **argv, char *sym, int nofargs) {
	int i;
	for(i=0;i<nofargs;i++) {
		if(strcmp(argv[i],sym) == 0)
			return 1;
	}
	return 0;
}


void lessersign(char *path,char **argv,int nofargs) {

}


void greatersign(char *path,char **argv,int nofargs){
	int i=0;
	while(i<nofargs && (strcmp(argv[i],">")!=0 && strcmp(argv[i],">>") !=0)){
		i++;
	}
	if(i!=nofargs-2){
		return;
	}
	char buf[10000];
	int PIPE_BUFFER=10000;

	char **argv1=(char**)malloc(sizeof(char*)*(nofargs-1));
	char **argv2=(char**)malloc(sizeof(char*)*(2));
	int j;
	for(j=0;j<nofargs-2;j++){
		argv1[j]=(char*)malloc(sizeof(char)*MAX_SIZE);
        strcpy (argv1[j], argv[j]);
	}
	argv1[j]=NULL;

		argv2[0]=(char*)malloc(sizeof(char)*MAX_SIZE);
        strcpy (argv2[0], argv[nofargs-1]);
		argv2[1]=NULL;

	int fd[2];
	pipe(fd);
	int ret = fork();
	if(ret==0){//child writes output to pipe
		close(fd[0]);//close read end
		close(1);//close stdout
		dup2(fd[1],1);//output to pipe
		execv(path,argv1);
	}
	else{//parent reads input from pipe
		close(0);//close stdin
		close(fd[1]);//close pipe_write
		dup2(fd[0],0);//input from pipe
		wait(NULL);
		read(0,buf,PIPE_BUFFER);
		FILE *fp;
		if(strcmp(argv[i],">")==0)
			fp = fopen(argv2[0],"w");
		else
			fp = fopen(argv2[0],"a");
		fprintf(fp,"%s\n",buf);
		exit(0);
	}
}