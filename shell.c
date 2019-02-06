#include "functions.h"

int main (void)
{
	extern char **environ;
	char *path = getenv ("PATH");
	char buf[MAX_SIZE];
	while(1){

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
			       
			
			char *p = getfile(path,command);
			bool back_pr = false;
		   	char **v = getargv(buf,&nofargs,&back_pr);
			int ret = fork ();
			setpgid(ret,ret);//new process group for every command

			
			if (ret == 0){
				if (p == NULL) {
					printf ("Command %s not found\n", buf);
					exit (1);
				}
				 greatersign(p,v,nofargs);
				//parsecommand(p,v,nofargs);
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

