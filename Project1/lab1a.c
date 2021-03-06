
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>

struct termios original;
int to_child[2];
int to_parent[2];
int pid;

void restoreTerm(){
  tcsetattr(0, TCSANOW, &original);
}

void exitShell(){
  int status;
  waitpid(pid, &status, 0);
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", (status & 0x7f), ((status >> 8) & 0xff));
}

void sigpipeHandler(){
  close(to_parent[0]);
  close(to_child[1]);
  exitShell();
  restoreTerm();
  exit(0);
}

int main(int argc, char* argv[]){
  //set terminal settings
  struct termios custom;
  tcgetattr(0, &original);
  tcgetattr(0, &custom);
  custom.c_iflag = ISTRIP;
  custom.c_oflag = 0;
  custom.c_lflag = 0;
  tcsetattr(0, TCSANOW, &custom);

  
  //shell option
  int shell_f = 0;
  char* filename;
  struct option options[] = { {"shell", 1, NULL, 's'},
			      {0, 0, 0, 0} };
  int val;
  while((val = getopt_long(argc, argv, "", options, NULL)) != -1){
    switch(val){
    case 's':
      shell_f = 1;
      filename = optarg;
      break;
    default:
      fprintf(stderr, "Error: Unknown flag.\nUsage: --shell\n");
      restoreTerm();
      exit(1);
    }
  }

  
  if(shell_f){
    if(pipe(to_child) == -1){
      fprintf(stderr, "Error: %s\n", strerror(errno));
      restoreTerm();
      exit(1);
    }
    if(pipe(to_parent) == -1){
      fprintf(stderr, "Error: %s\n", strerror(errno));
      restoreTerm();
      exit(1);
    }

    signal(SIGPIPE, sigpipeHandler);
    pid = fork();
    
    if(pid < 0){
      fprintf(stderr, "Error: %s\n", strerror(errno));
      restoreTerm();
      exit(1);
    }
    else if(pid == 0){ //set up child process

      close(to_parent[0]);
      close(to_child[1]);

      close(0);
      dup(to_child[0]);
      close(1);
      dup(to_parent[1]);
      close(2);
      dup(to_parent[1]);
      
      close(to_parent[1]);
      close(to_child[0]);
      
      if(execlp(filename, filename, NULL) == -1){
	fprintf(stderr, "Error: %s\nUnable to exec Program:%s.\n", strerror(errno), filename);
	restoreTerm();
	exit(1);
      }
    }
    else{
      close(to_child[0]);
      close(to_parent[1]);
      
      struct pollfd pollfds[] = { {0, (POLLIN + POLLHUP + POLLERR), 0},
				  {to_parent[0], (POLLIN + POLLHUP + POLLERR), 0}};

      int done = 0;
      while(!done){
	if(poll(pollfds, 2, 0) < 0){
	  fprintf(stderr, "Error: %s\n", strerror(errno));
	  restoreTerm();
	  exit(1);
	}
	//handle input from keyboard
	if(pollfds[0].revents & POLLIN){
	  char buff[256];
	  int bytesRead = read(0, buff, sizeof(char) * 256);
	  if(bytesRead < 0){
	    fprintf(stderr, "Error: %s\n", strerror(errno));
	    restoreTerm();
	    exit(1);
	  }
	  else{
	    for(int i = 0; i < bytesRead; i++){
	      if(buff[i] == 0x03){
		char* killCommand = "^C";
		write(1, killCommand, 2);
	        kill(pid, SIGINT);
	      }
	      else if(buff[i] == 0x04){
		char* exitcode = "^D";
		write(1, exitcode, 2);
		done = 1;
		close(to_child[1]);
		
	      }
	      else if(buff[i] == '\r' || buff[i] == '\n'){
		char* newreturn = "\r\n";
		write(1, newreturn, 2);
		char* newline = "\n";
		write(to_child[1], newline, 1);
	      }
	      else{
		write(1, &buff[i], 1);
		write(to_child[1], &buff[i], 1);
	      }
	      
	    }
	  }
	}
	//handle input from program
	if(pollfds[1].revents & POLLIN){
	  char buff[256];
	  int bytesRead = read(to_parent[0], buff, sizeof(char) * 256);
	  if(bytesRead < 0){
	    fprintf(stderr, "Error: %s\n", strerror(errno));
            restoreTerm();
            exit(1);
	  }
	  else{
	    for(int i = 0; i < bytesRead; i++){
	      if(buff[i] == '\n'){
		char* newreturn = "\r\n";
		write(1, newreturn, 2);
	      }
	      else{
		write(1, &buff[i], 1);
	      }
	    }
	  }
	}
	if(pollfds[1].revents & (POLLHUP | POLLERR)){
	  close(to_child[1]);
	  exitShell();
	  restoreTerm();
	  exit(0);
	}
      }
      close(to_child[1]);
      exitShell();
      restoreTerm();
      exit(0);
    }
  }
			  
  

  //no --shell
  for(;;){
    char buff[256];
    int bytesRead = read(0, buff, sizeof(char) * 256);
    if(bytesRead < 0){
      fprintf(stderr, "Error: %s\n", strerror(errno));
      restoreTerm();
      exit(1);
    }
    for(int i = 0; i < bytesRead; i++){
      if(buff[i] == 0x4){
	char* exitCode = "^D";
	write(1, exitCode, 2);
	restoreTerm();
	exit(0);
      }
      else if(buff[i] == '\r' || buff[i] == '\n'){
	char* newreturn = "\r\n";
	write(1, newreturn, 2);
      }
      else
	write(1, &buff[i], 1);
    }
  }
}
