all:
	ipcrm --all=msg
	gcc -g msgq_server.c chat.c -o s
	gcc -g msgq_client.c chat.c -o c

clean:
	rm s c
	
