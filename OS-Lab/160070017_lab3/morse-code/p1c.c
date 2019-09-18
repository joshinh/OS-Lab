#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


void sighandler(){
	pid_t t;
	t = wait(NULL);
	printf("Parent %i: Child %i is exited\n",getpid(),t);
	return;
}

int main(){
	signal(SIGCHLD, sighandler);
	int temp1;
	int sleep1 = 10, sleep2 = 5;
	temp1 = fork();
	if (temp1 > 0){
		printf("Child %i is created by parent %i (sleeps for %i seconds)\n",temp1,getpid(),sleep1);
	}
	else{
		int temp2 = fork();
		if (temp2 > 0){
			printf("Child %i is created by parent %i (sleeps for %i seconds)\n",temp2,getpid(), sleep2);
			sleep(10);
			exit(0);
		}
		else{
			sleep(5);
			exit(0);
		}
	}
	sleep(13);
}