#include <stdio.h>
#include <unistd.h>

int main(){
	pid_t temp = fork();
	//Disable stdout buffering
	setbuf(stdout, NULL);
	if (temp > 0){
		printf("Parent : My process ID is: %i\n",getpid());
		printf("Parent : The child process ID is: %i\n",temp);
		sleep(60);
		printf("Parent : The parent has terminated.\n");	
	}
	else{
		printf("Child : My process ID is: %i\n",getpid());
		printf("Child : The parent process ID is: %i\n",getppid());
		getc(stdin);		
	}
	
}