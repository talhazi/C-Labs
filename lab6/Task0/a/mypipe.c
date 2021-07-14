#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 32

int main(int argc, char** argv) {
  char buffer [BUFFER_SIZE];
  int fileDescriptors[2];
  char* msg = "hello";
  pid_t pid;

  //cleaning the buffer
  for (int i=0; i<BUFFER_SIZE; i++){buffer[i] = 0;}

  if (pipe(fileDescriptors) == -1){
      perror(strerror(errno));
      exit(EXIT_FAILURE);
  }

//child proccess
  if (!(pid = fork())){    
    close(fileDescriptors[0]);
    write(fileDescriptors[1],msg, strlen(msg));
  }
  //parent process
  else {
    close(fileDescriptors[1]);
    read(fileDescriptors[0], buffer, strlen(msg));
    printf("%s\n", buffer);
  }
  return 0;
}