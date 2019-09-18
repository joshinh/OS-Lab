#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char const *argv[])
{
	setbuf(stdout, NULL);
	int n = atoi(argv[1]);
	printf("Parent is : %i\n",getpid());
	printf("Number of children: %i\n",n);
	for(int i=0;i<n;i++){
		int temp = fork();
		float duration;
		duration = 5 + (rand()/(float) RAND_MAX * 5);
		if (temp > 0){
			printf("Child %i is created\n",temp);
		}
		else{
			sleep(duration);
			printf("Child %i exited\n",getpid());
			exit(0);
		}
	}
	sleep(12);
	printf("Parent exited\n");
}
