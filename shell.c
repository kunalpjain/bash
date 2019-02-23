#include "functions.h"

int main (void)
{
/*
	struct sigaction act;
	memset (&act, '\0', sizeof(act));
	//act.sa_sigaction = &handler;
	act.sa_handler = &handler;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &act, NULL);
*/
	remove("lookup.txt");
	signal(SIGINT,&handler);
	extern char **environ;
	char *path = getenv ("PATH");
	char buf[MAX_SIZE];
	while(1){

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
			int ret = fork ();
			setpgid(ret,ret);//new process group for every command

			
			if (ret == 0){

				if (p == NULL && strcmp(v[0],"sc")==0) {
					printf("args:%d\n",nofargs);
					customcommands(v,nofargs);
					//printf ("Command %s not found\n", buf);
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
					printf ("pid=%d,status =%d\n\n", ret, status);
				}
			}
		}	
	}
}

