#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define STDIN 0
#define STDOUT 1

#define BUFFER_SIZE 2048

int debug;

void printPath(){
  char path_name[PATH_MAX];
  getcwd(path_name,PATH_MAX);
  fprintf(stdout, "%s\n",path_name);
}

int specialCommand(cmdLine* command){
  int isSpecial=0;
  if(strcmp(command->arguments[0],"quit")==0){
    isSpecial=1;
    _exit(EXIT_SUCCESS);
    }
  if(strcmp(command->arguments[0],"cd")==0){
    isSpecial=1;
    if(chdir(command->arguments[1])<0)
      perror("bad cd command");
  }
  return isSpecial;
}

void execute(cmdLine* pCmdLine){
  int input=STDIN;
  int ouput=STDOUT;

  if(!specialCommand(pCmdLine)){
    int childPid;
    if(!(childPid=fork())){
      if(pCmdLine->inputRedirect){
        close(input);
        input=open(pCmdLine->inputRedirect, O_RDONLY);
        if(input<0){
          perror(strerror(errno));
          exit(EXIT_FAILURE);
        }
      }
      if(pCmdLine->outputRedirect){
        close(ouput);
        ouput=open(pCmdLine-> outputRedirect, O_WRONLY | O_CREAT);
        if(ouput<0){
          perror(strerror(errno));
          exit(EXIT_FAILURE);
        }
      }
      if(execvp(pCmdLine->arguments[0],pCmdLine->arguments)<0){
        perror(strerror(errno));
        _exit(EXIT_FAILURE);
      }
    }
    if(debug)
      fprintf(stderr, "PID: %d\nExecuting command: %s\n",childPid,pCmdLine->arguments[0] );
    if(pCmdLine->blocking)
      waitpid(childPid,NULL,0);
  }
}

int main(int argc, char const *argv[]) {
  FILE* input = stdin;
  char buffer[BUFFER_SIZE];
  debug=0;
  for(int i=1;i<argc;i++){
    if((strcmp("-D",argv[i])==0))
      debug=1;
  }
  while(1){
    printPath();
    fgets(buffer,BUFFER_SIZE,input);
    cmdLine* line = parseCmdLines(buffer);
    execute(line);
    freeCmdLines(line);
  }
  return 0;
}