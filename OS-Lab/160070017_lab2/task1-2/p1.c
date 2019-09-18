#include <stdio.h>
#include <unistd.h>

int main(){
	pid_t temp = fork();
	if (temp > 0){
		printf("Parent : My process ID is : %i\n",getpid());
		printf("Parent : The child process ID is : %i\n",temp);
	}
	else{
		printf("Child : My process ID is : %i\n",getpid());
		printf("Child : The parent process ID is : %i\n",getppid());
	}
	sleep(10);
}
