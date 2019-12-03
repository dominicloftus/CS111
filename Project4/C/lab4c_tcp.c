

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
#include <mraa.h>
#include <sys/socket.h>
#include <netdb.h>


mraa_aio_context temp;
int running;
int period;
char scale;
int fd;
char filename[64];
char id[10];
char hostname[64];
int portno;
struct timespec next_time;

int sockfd;
struct sockaddr_in serv_addr;
struct hostent* server;

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
  char message[64];
  sprintf(message, "%02d:%02d:%02d SHUTDOWN\n", real_time->tm_hour, real_time->tm_min,
          real_time->tm_sec);
  write(sockfd, message, 64);
  if(strcmp(filename, "") != 0){
    dprintf(fd, "%02d:%02d:%02d SHUTDOWN\n", real_time->tm_hour, real_time->tm_min,
          real_time->tm_sec);
  }
  mraa_aio_close(temp);
  exit(0);
}

void report(){
  struct timespec prd_check;
  clock_gettime(CLOCK_MONOTONIC, &prd_check);
  if(running && (prd_check.tv_sec >= next_time.tv_sec)){
    struct tm* real_time = getTime();
    float tempr = readTemp();
    char message[64];
    sprintf(message, "%02d:%02d:%02d %.1f\n", real_time->tm_hour, real_time->tm_min,
	    real_time->tm_sec, tempr);
    write(sockfd, message, 64);
    if(strcmp(filename, "") != 0){
      dprintf(fd, "%02d:%02d:%02d %.1f\n", real_time->tm_hour, real_time->tm_min,
	      real_time->tm_sec, tempr);
    }
    next_time.tv_sec = prd_check.tv_sec + period;
  }
}

void processCmd(char* command){
  int len = strlen(command);
  char log[512];
  log[0] = '\0';
  strcpy(log, command);
  if(strcmp(filename, "") != 0){
    strcat(log, "\n");
    write(fd, log, len+2);
  }
  if(strcmp(command, "SCALE=F") == 0)
    scale = 'F';
  if(strcmp(command, "SCALE=C") == 0)
    scale = 'C';
  if(strcmp(command, "STOP") == 0)
    running = 0;
  if(strcmp(command, "START") == 0)
    running = 1;
  if(strcmp(command, "OFF") == 0)
    shutDown();
  if(len > 7 && command[0] == 'P' && command[1] == 'E' &&
     command[2] == 'R' && command[3] == 'I' && command[4] == 'O' &&
     command[5] == 'D' && command[6] == '='){
    int prd = atoi(command+7);
    period = prd;
  }

}


int main(int argc, char* argv[]){

  period = 1;
  scale = 'F';
  filename[0] = '\0';
  id[0] = '\0';
  hostname[0] = '\0';
  portno = 0;
  running = 1;
  clock_gettime(CLOCK_MONOTONIC, &next_time);
  
  struct option options[] =
    {
     {"period", required_argument, NULL, 'p'},
     {"scale", required_argument, NULL, 's'},
     {"log", required_argument, NULL, 'l'},
     {"id", required_argument, NULL, 'i'},
     {"host", required_argument, NULL, 'h'},
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

    case 'i':
      if(strlen(optarg) != 9){
	fprintf(stderr, "Error: id should be 9 digit number.\n");
	exit(1);
      }
      strcat(id, optarg);
      break;

    case 'h':
      strcat(hostname, optarg);
      break;
      
    default:
      fprintf(stderr, "Error: Unknown flag used.\n");
      exit(1);
      break;
    }
  }

  int i;
  for(int i = 1; i < argc; i++){
    if(argv[i][0] != '-')
      portno = atoi(argv[i]);
  }

  if(id[0] == '\0'){
    fprintf(stderr, "Error: no --id provided.\n");
    exit(1);
  }
  if(host[0] == '\0'){
    fprintf(stderr, "Error: no --host provided.\n");
    exit(1);
  }
  if(log[0] == '\0'){
    fprintf(stderr, "Error: no --log provided.\n");
    exit(1);
  }
  if(portno == 0){
    fprintf(stderr, "Error: no port number provided.\n");
    exit(1);
  }
  
  temp = mraa_aio_init(1);

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    fprintf(stderr, "Error: failed to open socket.\n");
    exit(1);
  }
  
  if((server = gethostbyname(hostname)) == NULL){
    fprintf(stderr, "Error: failed to find host.\n");
    exit(1);
  }

  bzero((char*) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);

  if(connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1){
    fprintf(stderr, "Error: Unable to connect to host.\n");
    exit(1);
  }

  write(sockfd, id, 10);
  write(fd, id, 10);
  
  struct pollfd pfd;
  pfd.fd = sockfd;
  pfd.events = POLLIN | POLLHUP | POLLERR;
  
  for(;;){
    report();
    if(poll(&pfd, 1, 0) == -1){
      fprintf(stderr, "Error: Polling failed.\n");
      exit(1);
    }
    if(pfd.revents & POLLIN){
      char input[512];
      char command[512];
      command[0] = '\0';
      bzero((char*) input, 512);
      int n = read(sockfd, input, 512);
      
      for(i = 0; i < n; i++){
	if(input[i] == '\r');
	else if(input[i] == '\n'){
	  processCmd(command);
	  command[0] = '\0';
	}
	else{
	  char c[2];
	  c[0] = input[i];
	  c[1] = '\0';
	  strcat(command, c);
	}
      }
     
    }

  }
  
}





