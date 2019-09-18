#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
	FILE *fp;
	fp = fopen(argv[1],"w");
	int fno = fileno(fp);
	setbuf(stdout, NULL);
	setbuf(fp, NULL);
	printf("Parent: File opened. fd=%i.\n",fno);
	printf("Parent %i: writing to file %i\n", getpid(), fno);
    fprintf(fp, "hello world! i am the parent\n");
    int temp = fork();
	if (temp > 0){
		wait(NULL);
		printf("Parent : The child has terminated.\n");
		fclose(fp);
	}
	else{
		printf("Child %i: writing to file %i\n", getpid(), fno);
		fprintf(fp, "hello world! i am the child\n");
	}

		
}