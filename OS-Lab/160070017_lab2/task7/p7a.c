#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void recursive_fork(int t){
	//setbuf(stdout, NULL);
	if (t == 0) return;
	else{
		int fork_t = fork();
		if (fork_t > 0) {
			printf("Child %i is created\n",fork_t);
		}
		else{
			recursive_fork(t-1);
			sleep(t);
			printf("Child %i exited\n",getpid());
			exit(0);
		}
	}
}

int main(int argc, char const *argv[])
{
	setbuf(stdout, NULL);
	int n = atoi(argv[1]);
	printf("Parent is : %i\n",getpid());
	printf("Number of children: %i\n",n);
	recursive_fork(n);
	sleep(n+2);
	printf("Parent exited\n");

}

