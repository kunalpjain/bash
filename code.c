#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdbool.h>


int MAX_SIZE=100;
bool check(char *filepath);


int main(void){
    extern char **environ;
    char *path = getenv("PATH"), *path2=(char *)malloc(sizeof(char)*MAX_SIZE);
    char delim =':';
    char delim2 = ' ';
    char buf[MAX_SIZE], buf2[MAX_SIZE];
    char *savepath,*savebuf,*savebuf2;


    printf("prompt> ");
    fflush(stdout);


    while(read(0, buf, MAX_SIZE)>0) {

        for(int i=0;i<MAX_SIZE;i++)
            if(buf[i]=='\n')
                buf[i]='\0';
        strcpy(buf2, buf);
        char *firstarg = strtok_r(buf, &delim2, &savebuf);
	if(firstarg==NULL){
       		printf ("prompt> ");
                fflush (stdout);
                continue;
        }

        for(int i=0;i<MAX_SIZE;i++)
            if(buf[i]=='\n')
     	        buf[i]=0;
            char *k = malloc(100);
            char temp[100]="/";
            strcat(temp,firstarg);
            strcpy(path2, path);
            char *p = strtok_r(path2, &delim, &savepath);

            
            while(p!=NULL) {
                strcpy(k,p);
                strcat(k,temp);
                if(check(k)==true)
                    break;;
                p=strtok_r(NULL, &delim ,&savepath);
            }
            

            

            int nofargs=0;
            while(firstarg!=NULL) {
                firstarg = strtok_r(NULL, &delim2 ,&savebuf);
                nofargs++;
            }
            
            
            firstarg = strtok_r(buf2, &delim2 ,&savebuf2);
            bool back_pr = false;
            char **v = (char **) malloc (sizeof(char *) * (nofargs + 1));
            

            for(int i=0;i<nofargs;i++) {
                
                if(strcmp(firstarg,"&")==0 && i==nofargs-1) { // background process 
                    back_pr=true;
                    nofargs--;
                }
            
                v[i] = (char *) malloc (sizeof(char) * MAX_SIZE);
                strcpy(v[i], firstarg);
                firstarg = strtok_r(NULL, &delim2 ,&savebuf2);
            
            }
            

            v[nofargs] = NULL;
            int ret = fork();
            setpgid(ret,ret); //new process group for every command
            if(ret == 0) {
            	if(p==NULL){
               	 	printf("Command %s not found\n",buf);
                	exit(0);
            	}
            
                execv(k,(char **)v);
            

            }
            
            
            else {
            
                signal(SIGTTOU, SIG_IGN);
                int status;
                if(back_pr==false) { // if foreground process
                    int s = tcsetpgrp(0,ret); //give child access to terminal input
                    wait (&status);
                    s = tcsetpgrp(0,getpid()); //take back the terminal input
                }
                printf ("pid=%d,status =%d\n\n", ret, status);
            
            }


        printf("prompt> ");
        fflush(stdout);
    }
}

bool check(char *filepath){
    if(access(filepath,F_OK)==0){
        //  printf("file exists in %s\n",filepath);
        return true;
    }
    return false;
}
