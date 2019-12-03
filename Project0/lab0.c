
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>

void catchSegFault(){
  fprintf(stderr, "Error: Segmentation Fault\n");
  exit(4);
}

int main(int argc, char* argv[]){
  int inf = 0, outf = 0, segfaultf = 0, catchf = 0;
  char* fin;
  char* fout;
  
  struct option flagOptions[] =
    {
     {"input", 1, &inf, 1},
     {"output", 1, &outf, 1},
     {"segfault", 0, &segfaultf, 1},
     {"catch", 0, &catchf, 1},
     {0, 0, 0, 0}
    };

  int x;
  int indf;
  for(;;){
    x = getopt_long(argc, argv, "", flagOptions, &indf);
    if(x == -1)
      break;
    if(x != 0){
      fprintf(stderr, "Error: invalid argument(s)\n");
      exit(1);
    }
    if(indf == 0)
      fin = optarg;
    if(indf == 1)
      fout = optarg;
  }
  
  if(catchf){
    signal(SIGSEGV, catchSegFault);
  }

  if(segfaultf){
    char* p = NULL;
    *p = 'a';
  }

  if(inf){
    int infd = open(fin, O_RDONLY);
    if(infd >= 0){
      close(0);
      dup(infd);
      close(infd);
    }
    else{
      fprintf(stderr, "Error: %s. File %s could not be opened.\n", strerror(errno), fin);
      exit(2);
    }
  }

  if(outf){
    int outfd = creat(fout, 0666);
    if(outfd >= 0){
      close(1);
      dup(outfd);
      close(outfd);
    }
    else{
      fprintf(stderr, "Error: %s. File %s could not be created.\n", strerror(errno), fout);
      exit(3);
    }
  }

  char buf;
  while(read(0, &buf, sizeof(char)) > 0){
    write(1, &buf, sizeof(char));
  }
  
  exit(0);
  
}




