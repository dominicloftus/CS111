

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <poll.h>


mraa_aio_context temp;
mraa_gpio_context button;
int running;
int period;
char scale;
int fd;
char* filename;
struct timespec next_time;


float readTemp(){
  int reading = mraa_aio_read(temp);
  float ret = 1023.0/((float) reading) - 1.0;
  ret *= 100000.0;
  float C = 1.0/(log(ret/100000.0)/4275 + 1/298.15) - 273.15;
  float F = (C * 9)/5 + 32;
  if(scale == 'F')
    return F;
  return C;
}

struct tm* getTime(){
  struct timespec ts;
  struct tm* real_time;
  clock_gettime(CLOCK_REALTIME, &ts);
  real_time = localtime(&(ts.tv_sec));
  return real_time;
}

void shutDown(){
  struct tm* real_time = getTime();
  fprintf(stdout, "%d:%d:%d SHUTDOWN\n", real_time->tm_hour, real_time->tm_min,
          real_time->tm_sec);
  if(strcmp(filename, "") != 0){
    dprintf(fd, "%d:%d:%d SHUTDOWN\n", real_time->tm_hour, real_time->tm_min,
          real_time->tm_sec);
  }
  mraa_gpio_close(button);
  mraa_aio_close(temp);
  exit(0);
}

void report(char* rep){
  struct timespec prd_check;
  clock_gettime(CLOCK_MONOTONIC, &prd_check);
  if(running && (prd_check.tv_sec >= next_time.tv_sec)){
    struct tm* real_time = getTime();
    float tempr = readTemp();
    fprintf(stdout, "%d:%d:%d %.1f\n", real_time->tm_hour, real_time->tm_min,
	    real_time->tm_sec, temp);
    if(strcmp(filename, "") != 0){
      dprintf(fd, "%d:%d:%d %.1f\n", real_time->tm_hour, real_time->tm_min,
	      real_time->tm_sec, temp);
    }
    next_time.tv_sec = prd_check.tv_sec + period;
  }
}

void processCmd(char* command){
  if(strcmp(command, "SCALE=F") == 0)
    scale = 'F';
  if(strcmp(command, "SCALE=C") == 0)
    scale = 'C';
  if(strcmp(command, "STOP") == 0)
    running = 0;
  if(strcmp(command, "START") == 0)
    running = 1;
  if(strcmp(command, "OFF") == 0){
    strcat(command, "\n");
    write(fd, command, len+1);
    shutDown();
  }
  int len = strlen(command);
  if(len > 7 && command[0] == 'P' && command[1] == 'E' &&
     command[2] == 'R' && command[3] == 'I' && command[4] == 'O' &&
     command[5] == 'D' && command[6] == '='){
    int prd = atoi(command+7);
    period = prd;
  }

  if(strcmp(filename, "") != 0){
    strcat(command, "\n");
    write(fd, command, len+1);
  }

}


int main(int argc, char* argv[]){

  period = 1;
  scale = 'F';
  filename = ""; 
  running = 1;
  clock_gettime(CLOCK_MONOTONIC, &next_time);
  
  struct option options[] =
    {
     {"period", required_argument, NULL, 'p'},
     {"scale", required_argument, NULL, 's'},
     {"log", required_argument, NULL, 'l'},
     {0, 0, 0, 0},
    };

  int opt;
  while((opt = getopt_long(argc, argv, "", options, NULL)) != -1){
    switch(opt){
    case 'p':
      period = atoi(optarg);
      break;

    case 's':
      ;
      char let = optarg[0];
      if(let == 'C')
	scale = 'C';
      else if(let == 'F');
      else{
	fprintf(stderr, "Error: Unknown --scale= option used.\n");
	exit(1);
      }
      break;

    case 'l':
      strcat(filename, optarg);
      fd = open(filename, (O_RDWR | O_CREAT), 0666);
      if(fd == -1){
	fprintf(stderr, "Error: Unable to open file.\n");
	exit(1);
      }
      break;

    default:
      fprintf(stderr, "Error: Unknown flag used.\n");
      exit(1);
      break;
    }
  }

  temp = mraa_aio_init(1);
  button = mraa_gpio_init(60);
  mraa_gpio_dir(button, MRAA_GPIO_IN);
  
  struct pollfd pfd;
  pfd.fd = 0;
  pfd.events = POLLIN | POLLHUP | POLLERR;
  
  for(;;){
    if(poll(&pfd, 1, 0) == -1){
      fprintf(stderr, "Error: Polling failed.\n");
      exit(1);
    }
    if(pfd.revents & POLLIN){
      char input[512];
      char* command = "";
      int n = read(0, input, 512);

      for(int i = 0; i < n; i++){
	if(input[i] == '\n'){
	  processCmd(command);
	  strcpy(command, "");
	}
	else{
	  char c[2];
	  c[0] = input[i];
	  c[1] = '\0';
	  strcat(command, c);
	}
      }
     
    }

    if(mraa_gpio_read(button))
      shutDown();
  }
  
}





