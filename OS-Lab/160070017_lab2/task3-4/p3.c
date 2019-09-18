#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	int temp = fork();
	if (temp > 0){
		wait(NULL);
		printf("Parent : The child has terminated.\n");
	}
	else{
        char* cmd = "./mycat";
		char *arv[] = {NULL};
		execv(cmd,arv);
	}
}