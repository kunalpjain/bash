#include<stdio.h>
#include<string.h>
int main() {
	char a[]="1\n22\n213\n";
	int k=4;
	char s[10];
	while(sscanf(a,"%s",s)) {
		printf("%d\n", atoi(s));
	}
}
