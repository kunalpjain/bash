#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdbool.h>



int MAX_SIZE = 100;

bool check (char *filepath);
char *getfile(char *buf,char *path,char *firstarg);
char **getargs(char buf[],int nofargs,bool *back_pr);


int main (void)
{

	extern char **environ;

	char *path = getenv ("PATH");
	
	char delim = ':';

	char delim2 = ' ';
char buf[MAX_SIZE], buf2[MAX_SIZE],buf3[MAX_SIZE];
	

	
	
	printf ("prompt> ");
	
	fflush (stdout);
	while (read (0, buf, MAX_SIZE) > 0){

        
		for (int i = 0; i < MAX_SIZE; i++)
            if(buf[i] == '\n')
                buf[i] = '\0';
		
        char  *savebuf, *savebuf2;
        strcpy(buf2,buf);
        strcpy(buf3,buf);
        char *firstarg = strtok_r (buf3, &delim2, &savebuf);
        
        if(firstarg==NULL){
             printf ("prompt> ");
             fflush (stdout);
            continue; 
        }
       
        char *p = getfile(buf,path,firstarg);//PROBLEM IN THIS FUNCTION*************************8
 
		int nofargs = 0;
		while (firstarg != NULL){
			nofargs++;
			firstarg = strtok_r (NULL, &delim2, &savebuf);
		}
		
		// firstarg = strtok_r (buf2, &delim2, &savebuf2);
		bool back_pr = false;
		
        char **v = getargs(buf2,nofargs,&back_pr);
        printf("back_pr:%d\n",back_pr );
		
		
		int ret = fork ();
		setpgid(ret,ret);//new process group for every command
		if (ret == 0){
			if (p == NULL){
				printf ("Command %s not found\n", buf);
				exit (1);
			}
			execv (p, v);
		}
		else{
			signal(SIGTTOU, SIG_IGN);
			int status;
			if(back_pr==0){// if foreground process
				int s = tcsetpgrp(0,ret);//give child access to terminal input
				wait (&status);
				s = tcsetpgrp(0,getpid());//take back the terminal input
			}
            //if(p!="\n")
			printf ("pid=%d,status =%d\n\n", ret, status);
		}
		
		printf ("prompt> ");
		fflush (stdout);
		
	}
		
}

char *getfile(char *buf,char *path,char *firstarg){//returns file path of command entered by user
        char delim=':';
        char delim2=' ';
        char *savepath;
       
        char *k = (char*)malloc (sizeof(char)*100);//final file path
        
        char temp[100] = "/";
        
        strcat (temp, firstarg);
        char *path2 =(char *) malloc (sizeof (char) * MAX_SIZE);

        strcpy (path2, path);
        //printf("path2:%s\n",path2);

        /*
        PROBLEM aAFTER THIS***************************
        */
        char *p = strtok_r (path2, &delim, &savepath);//tokenize path2
        
        while (p != NULL){
        
            printf("p:%s\n",p );// problem here not giving correct result on multiple runs
            
            strcpy (k, p);
            
            strcat (k, temp);
            printf("        path,k:%s\n",k );
            if (check (k) == true)
            
                break;
            
            p = strtok_r (NULL, &delim, &savepath);
            
        }
        if(p!=NULL)
            return k;
        else
            return NULL;

}

char **getargs(char *buf,int nofargs,bool *back_pr){//updates if background process and returns vector args
    char delim2 = ' ';
    char *savebuf2;
    char *firstarg = strtok_r (buf, &delim2, &savebuf2);
    
    char **v = (char **) malloc (sizeof (char *) * (nofargs + 1));
        for (int i = 0; i < nofargs; i++){
            if(strcmp(firstarg,"&")==0 && i==nofargs-1){// background process 
                //printf("here\n");
                *back_pr=true;
                nofargs--;
            }
            // printf("firstarg:%s\n",firstarg);
            v[i] = (char *) malloc (sizeof (char) * MAX_SIZE);
            
            strcpy (v[i], firstarg);
            
            firstarg = strtok_r (NULL, &delim2, &savebuf2);
        
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
