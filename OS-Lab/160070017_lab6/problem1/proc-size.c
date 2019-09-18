#include "types.h"
#include "stat.h"
#include "user.h"
#include "memlayout.h"
char b[1024];


int getpusz()
{
	/* todo */
	uint i=0;
	int count=0;
	int flags;
	uint pa;
	for(i=0;i<KERNBASE;){
		if(get_va_to_pa(i, &pa, &flags)){
			count = count + 4096;
		}
		i = i + 4096;
	}
	return count;
}
int getpksz()
{
	/* todo */
	uint i=0;
	int count=0;
	int flags;
	uint pa;
	for(i=KERNBASE;i<KERNBASE+PHYSTOP;){
		if(get_va_to_pa(i, &pa, &flags)){
			count = count + 4096;
		}
		i = i + 4096;
	}
	return count;

}

int
main(int argc, char *argv[])
{
	char *buf;

	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());


	buf=sbrk(4096);
	buf[0]='\0';
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());

	
	buf=sbrk(4096*7);
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());

	buf=sbrk(1);
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());

	buf=sbrk(2);
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());


	exit();
}