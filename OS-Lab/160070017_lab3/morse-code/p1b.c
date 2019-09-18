#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int count = 0;

void sighandler(){
	pid_t t;
	t = wait(NULL);
	printf("Child %i is exited\n",t);
	count = count + 1;
	return;
}

int main(int argc, char const *argv[])
{
	int n;
	
	n = atoi(argv[1]);
	signal(SIGCHLD, sighandler);
	for(int i=0;i<n;i++){
		int temp = fork();
		float duration;
		duration = 5 + (rand()/(float) RAND_MAX * 5);
		//duration = rand() % 10;
		if (temp > 0){
			printf("Child %i is created (sleeps for %f seconds)\n",temp,duration);
		}
		else{
			sleep(duration);
			exit(0);
		}
	}
	while(1){
		if (count == 5) break;
	}

}