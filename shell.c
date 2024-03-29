#include "functions.h"

int main (void)
{
	signal(SIGINT,&handler);
	extern char **environ;
	char *path = getenv ("PATH");
	char buf[MAX_SIZE];
	lookuptable = (char **)malloc(sizeof(char *)*MAX_SIZE);
	for(int i=0;i<MAX_SIZE;i++) {
		lookuptable[i] = (char *) malloc (sizeof(char) * 10);
		strcpy(lookuptable[i], "empty");
	}
	for(;;){

		fflush (stdout);
		printf ("prompt> ");
		fflush (stdout);

		if (read (0, buf, MAX_SIZE) > 0){
			for (int i = 0; i < MAX_SIZE; i++)
						if(buf[i] == '\n')
					buf[i] = '\0';
			int nofargs=0;
			char *command = getcommand(buf,&nofargs); //nofargs updated too
			if(command==NULL){//if ENTER
				continue; 
			}
			if(strcmp(command,"exit") == 0) {   //ability to exit from shell
				printf("Exiting prompt\n");
				exit(0);      
			}

			char *p = getfile(path,command);
			bool back_pr = false;
			char **v = getargv(buf,&nofargs,&back_pr);
			if (strcmp(v[0],"sc")==0) {
				customcommands(v,nofargs);
				continue;
			}
			int ret = fork ();
			setpgid(ret,ret);//new process group for every command

			
			if (ret == 0){
				if (p == NULL) {
					printf ("Command %s not found\n", buf);
					exit (0);
				}
				parsecommand(path,p,v,nofargs);
				execv (p, v);
			}
			
			else {
				signal(SIGTTOU, SIG_IGN);
				int status;
				if(back_pr==0){ // if foreground process
					int s = tcsetpgrp(0,ret); //give child access to terminal input
					wait (&status);
					s = tcsetpgrp(0,getpid()); //take back the terminal input
					printf ("\n||PID = %d\n||Status = %d\n\n\n", ret, status);
				}
			}
		}	
	}
}


