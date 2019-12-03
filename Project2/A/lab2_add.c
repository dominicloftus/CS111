
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>

long num_threads, num_iter;
long long count;
int opt_yield;
char opt_sync;

pthread_mutex_t mut;
long spinlock;

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if(opt_yield)
    sched_yield();
  *pointer = sum;
}

void addM(long long *pointer, long long value){
  pthread_mutex_lock(&mut);
  long long sum = *pointer + value;
  if(opt_yield)
    sched_yield();
  *pointer = sum;
  pthread_mutex_unlock(&mut);
}

void addS(long long *pointer, long long value) {
  while(__sync_lock_test_and_set(&spinlock, 1));
  long long sum = *pointer + value;
  if(opt_yield)
    sched_yield();
  *pointer = sum;
  __sync_lock_release(&spinlock);
}

void addC(long long *pointer, long long value) {
  long long oldval;
  do{
    oldval = *pointer;
    if(opt_yield)
      sched_yield();
  }while(__sync_val_compare_and_swap(pointer, oldval, oldval + value) != oldval);
}



void* threadFunc(){
  for(int i = 0; i < num_iter; i++){
    if(opt_sync == 'm'){
      addM(&count, 1);
      addM(&count, -1);
    }
    else if(opt_sync == 's'){
      addS(&count, 1);
      addS(&count, -1);
    }
    else if(opt_sync == 'c'){
      addC(&count, 1);
      addC(&count, -1);
    }
    else{
      add(&count, 1);
      add(&count, -1);
    }
  }
  return NULL;
}





int main(int argc, char* argv[]){
  num_threads = num_iter = 1;
  count = 0;
  opt_yield = 0;
  opt_sync = 'n';

  struct option options[] =
    {
     {"threads", required_argument, NULL, 't'},
     {"iterations", required_argument, NULL, 'i'},
     {"yield", no_argument, NULL, 'y'},
     {"sync", required_argument, NULL, 's'},
     {0, 0, 0, 0}
    };

  int opt;
  while((opt = getopt_long(argc, argv, "", options, NULL)) != -1){
    switch (opt){

    case 't':
      num_threads = atoi(optarg);
      break;

    case 'i':
      num_iter = atoi(optarg);
      break;

    case 'y':
      opt_yield = 1;
      break;

    case 's':
      ;
      char let = optarg[0];
      if(let == 'm')
	opt_sync = 'm';
      else if(let == 's')
	opt_sync = 's';
      else if(let == 'c')
	opt_sync = 'c';
      else{
	fprintf(stderr, "Error: Unknown --sync= option used\n");
	exit(1);
      }
      break;
	
    default:
      fprintf(stderr, "Error: Unknown option used\n");
      exit(1);
    }
    
  }

			   
  if(opt_sync == 'm'){
    pthread_mutex_init(&mut, NULL);
  }

  if(opt_sync == 's'){
    spinlock = 0;
  }
  
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
  
  for(int i = 0; i < num_threads; i++){
    if(pthread_create(&threads[i], NULL, &threadFunc, NULL) != 0){
      fprintf(stderr, "Error: Unable to create thread.\n");
      exit(1);
    }
  }

  for(int i = 0; i < num_threads; i++){
    if(pthread_join(threads[i], NULL) != 0){
      fprintf(stderr, "Error: Unable to join thread.\n");
      exit(1);
    }
  }
  
  clock_gettime(CLOCK_MONOTONIC, &end);

  free(threads);

  long num_ops = num_threads * num_iter * 2;
  long time_diff = 1000000000 * ((long) (end.tv_sec - start.tv_sec)) +
    end.tv_nsec - start.tv_nsec;
  long nsec_per_op = time_diff / num_ops;

  char* tag_field;

  if(opt_yield){
    if(opt_sync == 'm')
      tag_field = "add-yield-m";
    else if(opt_sync == 's')
      tag_field = "add-yield-s";
    else if(opt_sync == 'c')
      tag_field = "add-yield-c";
    else
      tag_field = "add-yield-none";
  }
  else{
    if(opt_sync == 'm')
      tag_field = "add-m";
    else if(opt_sync == 's')
      tag_field = "add-s";
    else if(opt_sync == 'c')
      tag_field	= "add-c";
    else
      tag_field	= "add-none";
  }
  
  fprintf(stdout, "%s,%ld,%ld,%ld,%ld,%ld,%lld\n",
	  tag_field, num_threads, num_iter, num_ops, time_diff, nsec_per_op, count);

  exit(0);
  
  
}




