#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sighandler1(){
	printf("Interrupt SIGINT received.\n");
	printf("Do you really want to quit (y|n)?\n");
	char temp[100];
	scanf("%s",temp);
	if (temp[0] == 'n') return;
	else exit(0);
}

void sighandler2(){
	printf("Interrupt SIGTERM received.\n");
	printf("Do you really want to quit (y|n)?\n");
	char temp[100];
	scanf("%s",temp);
	if (temp[0] == 'n') return;
	else exit(0);
}


int main(){
	while(1){
		signal(SIGINT, sighandler1);
		signal(SIGTERM, sighandler2);
	}
	
}