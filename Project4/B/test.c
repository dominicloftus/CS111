
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void){
  char input[512];
      char command[512] = "";
      int ncommand = 0;
      int n = read(0, input, 512);

      for(int i = 0; i < n; i++){
	if(input[i] == '\n'){
          strcat(command, "\0");
          processCmd(command);
          command = "";
          ncommand = 0;
        }
	else{
          command[ncommand] = input[i];
          ncommand++;
	}
      }
}
