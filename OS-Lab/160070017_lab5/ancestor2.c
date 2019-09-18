#include "types.h"
#include "stat.h"
#include "user.h"

//Creation of child and grandchild with parent exiting first, then child and then grandchild


int main() 
{
	int ret = fork();
	if (ret == 0) 
	{
		sleep(100);

		int ret2 = fork();
		if(ret2 == 0){
			sleep(400);
			printf(1, "\ngrandchild: pid %d\n", getpid());
			printf(1, "grandchild: parent pid %d\n", getppid());
			exit();
		}
		else{
			sleep(200);
			printf(1, "\nchild: pid %d\n", getpid());
			printf(1, "child: parent pid %d\n", getppid());
			exit();	
		}

		
	}
	else 
	{
		sleep(100);

		printf(1, "\nparent: pid %d\n", getpid());
		printf(1, "parent: parent pid %d \n", getppid());
		printf(1, "parent: child pid %d\n", ret);

		

	}
	
	exit();
}