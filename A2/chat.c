#include "chat.h"

long ntoid(char *name) {
	long id = 0;
	for(int i=0;i<strlen(name);i++) {
		if(isupper(name[0]) == 1)
			id += (long) (name[0] - 'A') + 26;
		else
			id += (long) (name[0] - 'a');
	}
	return id;
}
