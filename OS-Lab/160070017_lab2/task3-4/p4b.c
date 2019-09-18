#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
	FILE *fp;
	fclose(stdin);
	fp = fopen(argv[1],"r");
	int fno = fileno(fp);
	printf("Parent: File opened: fd=%i.\n",fno);
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