
#include "SortedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <signal.h>


long num_threads, num_iter;
char opt_sync;
int opt_yield;

SortedList_t head;
SortedListElement_t* elements;

long spinlock;
pthread_mutex_t mut;


void* threadFunc(void* args){
  SortedListElement_t* thread_elements = (SortedListElement_t*) args;
  
  if(opt_sync == 'm')
    pthread_mutex_lock(&mut);
  if(opt_sync == 's')
    while(__sync_lock_test_and_set(&spinlock, 1));
  
  for(int i = 0; i < num_iter; i++)
    SortedList_insert(&head, thread_elements + i);

  long len = SortedList_length(&head);
  if(len == -1){
    fprintf(stderr, "Error: Problem inserting elements into list\n");
    exit(2);
  }

  for(int i = 0; i < num_iter; i++){
    SortedListElement_t* to_be_deleted = SortedList_lookup(&head, thread_elements[i].key);
    if(to_be_deleted == NULL){
      fprintf(stderr, "Error: Unable to find inserted element\n");
      exit(2);
    }
    if(SortedList_delete(to_be_deleted)){
      fprintf(stderr, "Error: corrupted prev/next pointers on delete\n");
      exit(2);
    }
  }

  if(opt_sync == 'm')
    pthread_mutex_unlock(&mut);
  if(opt_sync == 's')
    __sync_lock_release(&spinlock);
  
  return NULL;
}



void segFaultHandler(){
  fprintf(stderr, "Error: Segmentation fault.\n");
  exit(2);
}

int main(int argc, char* argv[]){

  signal(SIGSEGV, segFaultHandler);
  num_threads = num_iter = 1;
  opt_sync = 'n';
  opt_yield = 0;
  
  struct timespec start, end;
  
  struct option options[] =
    {
     {"threads", required_argument, NULL, 't'},
     {"iterations", required_argument, NULL, 'i'},
     {"yield", required_argument, NULL, 'y'},
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
      for(int i = 0; i < (int)strlen(optarg); i++){
	if(optarg[i] == 'i')
	  opt_yield |= INSERT_YIELD;
	else if(optarg[i] == 'd')
	  opt_yield |= DELETE_YIELD;
	else if(optarg[i] == 'l')
	  opt_yield |= LOOKUP_YIELD;
	else{
	  fprintf(stderr, "Error: Unknown --yield= option used.\n");
	  exit(1);
	}
      }
      break;

    case 's':
      ;
      char let = optarg[0];
      if(let == 'm')
	opt_sync = 'm';
      else if(let == 's')
	opt_sync = 's';
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
  
  head.next = NULL;
  head.prev = NULL;
  head.key = NULL;
  
  elements = malloc(num_threads * num_iter * sizeof(SortedListElement_t));

  char** keys = malloc(num_threads * num_iter * sizeof(char*));

  for(int i = 0; i < (num_threads * num_iter); i++){
    keys[i] = malloc(sizeof(char) * 11);
    for(int j = 0; j < 10; j++){
      keys[i][j] = (rand() % 26) + 'a';
    }
    keys[i][10] = '\0';
    elements[i].key = keys[i];
  }

  
  pthread_t* threads = malloc(num_threads * sizeof(pthread_t));

  clock_gettime(CLOCK_MONOTONIC, &start);
  
  for(int i = 0; i < num_threads; i++){
    if(pthread_create(&threads[i], NULL, &threadFunc, (void*)(elements + (num_iter * i))) != 0){
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

  
  long len = SortedList_length(&head);

  if(len != 0){
    fprintf(stderr, "Error: End list length != 0\n");
    exit(2);
  } 

  
  long num_lists = 1;
  long num_ops = num_threads * num_iter * 3;
  long time_diff = 1000000000 * ((long) (end.tv_sec - start.tv_sec)) +
    end.tv_nsec - start.tv_nsec;
  long nsec_per_op = time_diff / num_ops;


  char tag_field[30] = "list-";
  char yieldopts[10] = "";
  char syncopts[10] = "-";

  if(opt_yield & INSERT_YIELD)
    strcat(yieldopts, "i");
  if(opt_yield & DELETE_YIELD)
    strcat(yieldopts, "d");
  if(opt_yield & LOOKUP_YIELD)
    strcat(yieldopts, "l");
  if(strlen(yieldopts) == 0)
    strcat(yieldopts, "none");

  if(opt_sync == 'm')
    strcat(syncopts, "m");
  else if(opt_sync == 's')
    strcat(syncopts, "s");
  else
    strcat(syncopts, "none");

  strcat(tag_field, yieldopts);
  strcat(tag_field, syncopts);
  
  fprintf(stdout, "%s,%ld,%ld,%ld,%ld,%ld,%ld\n",
	  tag_field, num_threads, num_iter, num_lists, num_ops, time_diff, nsec_per_op);
  
  free(threads);
  free(keys);
  
  exit(0);
  
  
}




