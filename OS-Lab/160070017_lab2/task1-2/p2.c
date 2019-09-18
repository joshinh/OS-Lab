#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	int temp = fork();
	if (temp > 0){
		wait(NULL);
		printf("Parent : My process ID is : %i\n",getpid());
		printf("Parent : The child process ID is : %i\n",temp);
		printf("Parent : The child with process ID %i has terminated.\n",temp);
	}
	else{
		printf("Child : My process ID is : %i\n",getpid());
		printf("Child : The parent process ID is : %i\n",getppid());
	}
}
