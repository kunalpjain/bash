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




char **getargv(char *buff,int *nofargs,bool *back_pr){//updates backgprocess and returns vector argv
	char buf[MAX_SIZE];
	strcpy(buf,buff);
	char *delim2 = " ";
	char *savebuf2;
	char *firstarg = strtok_r (buf, delim2, &savebuf2);

	char **v = (char **) malloc (sizeof (char *) * (*nofargs + 1));
	for (int i = 0; i < *nofargs; i++){
		if(strcmp(firstarg,"&")==0 && i==*nofargs-1){// background process 
			*back_pr=true;
			*nofargs-=1;
			break;
		}

		v[i] = (char *) malloc (sizeof (char) * MAX_SIZE);

		strcpy (v[i], firstarg);

		firstarg = strtok_r (NULL, delim2, &savebuf2);

	}

	v[*nofargs] = (char *) malloc (sizeof (char) * MAX_SIZE);
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





void parsecommand(char *path,char **argv,int nofargs) { //indexes all the symbols
	if(strinarr(argv, "|",nofargs) >= 0)
		pipesign(path,argv,nofargs);
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

	char **newpath = (char **) malloc (sizeof(char *) * (end - start + 1));
	for(int i=0;i<=end-start;i++) {
		newpath[i] = (char *) malloc (sizeof(char) * MAX_SIZE);
		newpath[i] = argv[i+start];
	}
	return newpath;
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
		for(int i=index;i<nofargs;i++)
			argv[i] = NULL;
		printf("\n");
		execv(path,argv);
	}
	

	FILE *fp2;
	if(grtsign >= 0)
		fp2 = fopen(argv[grtsign+1], "w");
	else
		fp2 = fopen(argv[appsign+1], "a");
	printf("File %s remapped from %d to %d\n", argv[nofargs-1], fileno(fp2), 1);
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

	for(int i=nofargs-2;i<nofargs;i++)
		argv[i] = NULL;

	printf("File %s remapped from %d to %d\n", argv[nofargs-1], fileno(fp), 1);
	dup2(fileno(fp), 1);
	printf("\n");
	execv(path,argv);
}


void pipesign(char *path,char **argv,int nofargs) {

	int countofpipe=0;
	for(int i=0;i<nofargs;i++) {
		if(strcmp(argv[i],"|") == 0) {
			countofpipe++;
		}
	}

	int p[countofpipe][2];
	int index = strinarr(argv,"|",nofargs),prev=0;
	for(int i=0;i<countofpipe;i++) {
		/*while(index >= 0) {
			char **newpath = subarray(argv,prev,index);
			int p[2];
			pipe(p);
			dup2(p[1],1);
			
		}*/
		if(i!=0)
			dup2(p[0],0);
		if(i!=countofpipe-1)
			dup2(p[1],1);
		if(fork()==0) {
			parsecommand()
		}
	}

}