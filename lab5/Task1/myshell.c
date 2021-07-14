#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 2048

void printPath(){
  char path_name[PATH_MAX];
  getcwd(path_name,PATH_MAX);
  fprintf(stdout,"%s\n",path_name);
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
  if(isSpecial)
    freeCmdLines(command);
  return isSpecial;
}

void execute(cmdLine* pCmdLine, int debug){
  if(!specialCommand(pCmdLine)){
    int childPid;
    if(!(childPid=fork())){
        if(execvp(pCmdLine->arguments[0],pCmdLine->arguments)<0){
          perror("can't execute the command");
          _exit(EXIT_FAILURE);
      }
    }
    if(debug)
      fprintf(stderr, "PID: %d\nExecuting command: %s\n",childPid,pCmdLine->arguments[0]);
    if(pCmdLine->blocking)  //& ? 1 : 0
      waitpid(childPid,NULL,0);
  }
}

int main(int argc, char const *argv[]) {
  FILE* input = stdin;
  char buf[BUFFER_SIZE];
  int debug=0;
  for(int i=1;i<argc;i++){
    if((strcmp("-D",argv[i])==0)){
      debug=1;
    }
  }
  while(1){
    printPath();
    fgets(buf,BUFFER_SIZE,input);
    cmdLine* line = parseCmdLines(buf);
    execute(line,debug);
    freeCmdLines(line);
  }
  return 0;
}