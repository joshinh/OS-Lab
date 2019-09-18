#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
	setbuf(stdout, NULL);
	int n = atoi(argv[1]);
	printf("Parent is : %i\n",getpid());
	printf("Number of children: %i\n",n);
	int child_pids[n];
	int stat;
	for(int i=0;i<n;i++){
		int temp = fork();
		float duration;
		duration = 5 + (rand()/(float) RAND_MAX * 5);
		if (temp > 0){
			child_pids[i] = temp;
			printf("Child %i is created\n",temp);
		}
		else{
			sleep(duration);
			exit(0);
		}
	}
	for(int j=0;j<n;j++){
		waitpid(child_pids[n-j-1],&stat,0);
		printf("Child %i exited\n",child_pids[n-j-1]);
	}
	printf("Parent exited\n");
}
