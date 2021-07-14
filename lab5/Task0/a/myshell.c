#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 2048

void printCWD(){
  char path_name[PATH_MAX];
  getcwd(path_name,PATH_MAX);
  fprintf(stdout, "%s\n",path_name);
}

void execute(cmdLine* pCmdLine){
  if(strcmp(pCmdLine->arguments[0],"quit")==0){exit(EXIT_SUCCESS);}
  int returnVal=execvp(pCmdLine->arguments[0],pCmdLine->arguments);
  if(returnVal<0){
    perror("Could not execute the command");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char const *argv[]) {
  FILE* input = stdin;
  char buf[BUFFER_SIZE];
  while(1){
    printCWD();
    fgets(buf,BUFFER_SIZE,input);
    cmdLine* line = parseCmdLines(buf);
    execute(line);
    freeCmdLines(line);
  }
  return 0;
}