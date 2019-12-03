
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long num_lists = 5;

long hashFunc(const char* key){
  long hash;
  for(int i = 0; key[i] != '\0'; i++){
    hash = ((long)key[i] ^ 17) << 4;
  }
  if(hash < 0)
    hash *= -1;
  return hash % num_lists;
}


int main(void){

  char* wrd = "funkyturn";

  for(int i = 0; i < 20; i++){
    printf("%d\n", hashFunc(wrd));
  }
  
}
