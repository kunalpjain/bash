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
