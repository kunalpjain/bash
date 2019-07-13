# bash
This is a bash created using system calls (pipes, exec, etc) which can replace the traditional linux shell.Also, it has a feature to create shortcuts to commands.
	 	 	
Network Programming
Assignment #1

Design Decisions


Date Submitted:					 Vedant Patwary (2016A7PS0031P)
February 28, 2019					 Kunal Jain (2016A7PS0022P)




Part 1

Deliverables included: shell.c, makefile, functions.c, functions.h


How to run:
			$ make
			$ ./a.out
			$ prompt> 		Now you can start using the shell
			$ prompt> exit	This command exits the prompt


Constraints:
	1. Maximum size of input can be 100.
	2. You can store maximum 100 shortcut commands (index 1 to 100) of length 		10 each.


Working of the code:
	Once the prompt starts, it will work like a normal shell. Type ‘exit’ to return to
	your normal terminal. While writing commands, you can use as many
	whitespaces as you want. Writing an ‘&’ at the end of the command will send
	it to background. To store a command as shortcut to an index, use
	‘sc -i index command` and to delete it use `sc -d index [command]`.
	To execute your shortcut command, press Ctrl + C to invoke the shortcut menu.
	Then either enter the shortcut index that you want to execute or ‘0’ for printing
	all available shortcuts (that you added earlier).

	


Implementation:
	
 	The code is implemented as a 	parent-child relationship. The parent takes input from stdin and 	then after finding the path to it and finding the number of 	arguments, it passes it to the child.
 	
		
 		Parent – has the job of 		taking input, parsing it (removing spaces, etc), calculating number 		of arguments, handling ‘sc’ commands and creating background 		processes.
 		
		Child – does the actual 		execution of the commands (except ‘sc’).
 	
	
	Getfile() – This functions 	finds the command you entered in the paths mentioned in PATH 	enviroment variable.
 	
	Parsecommand() - This 	function is backbone of the code, this code redirects all commands 	to their respective functions. 	
 	
	Pipecommand() - This function 	uses n-1 pipes (if there are n ‘|’s) and joins them linearly so 	that multiple pipes are supported. Then it sends the command in 	between pipes to parsecommand, which handles the rest.
 	
	Lookuptable – This is a 2D 	char array used to store the shortcut commands. When there is 	nothing stored then the row has “empty”. ‘sc -i <ind> 	<cmd>’ command replaces “empty” with “cmd” and ‘sc 	-d <ind> <cmd>’ does the vice versa. 	
 	
	A handler for receving SIGINT 	was made. It had the job of executing shortcut commands and printing 	if necessary.



