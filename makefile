all:	bash	

bash: functions.o shell.o
	gcc -g functions.o shell.o 

functions.o: functions.c
	gcc -g -c functions.c

shell.o:	shell.c
	gcc -g -c shell.c

