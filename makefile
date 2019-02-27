all:	bash

bash: functions.o shell.o functions.h
	gcc -g functions.o shell.o 

functions.o: functions.c functions.h
	gcc -g -c functions.c

shell.o:	shell.c functions.h
	gcc -g -c shell.c

clean:
	rm *.o a.out

