#include "types.h"
#include "stat.h"
#include "user.h"

char buf[1];

void
head(int fd, int m)
{
  int n;
  int t=0;
  while((n = read(fd, buf, sizeof(buf))) > 0) {
      if(t<m){
        if (write(1, buf, n) != n) {
          printf(1, "head: write error\n");
          exit();
        }
      }
      if (buf[0] == '\n') t=t+1;    
          
  }
  if(n < 0){
    printf(1, "head: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 2){
    head(0,1);
    exit();
  }

  int n = atoi(argv[1]);
  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf(1, "head: cannot open %s\n", argv[i]);
      exit();
    }
    printf(1,"--%s--\n",argv[i]);
    head(fd,n);
    close(fd);
  }
  exit();
}
