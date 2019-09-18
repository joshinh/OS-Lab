#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
	int pid = fork();
	if(pid > 0){
		wait();
		exit();

	}
	else{
		if (argc<=1){
			printf(1,"Enter program to run\n");
			exit();
		}

		exec(argv[1],argv + 1);
	}
	
}