#include "util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define STDOUT 1
#define SEEK_SET 0
#define O_RDRW 2

extern int system_call();

int main (int argc , char* argv[], char* envp[]){
  int shira_pos = 657;
  char * name_to_replace;
  char* filename;
  int file;
  if (argc==3){
    filename=argv[1];
    name_to_replace=argv[2];
    file=system_call(SYS_OPEN,filename,O_RDRW,0777);
    if(file<0){
      system_call(SYS_EXIT,0x55);
    }
    system_call(SYS_LSEEK,file,shira_pos,SEEK_SET);
    system_call(SYS_WRITE,file,name_to_replace,strlen(name_to_replace));
    system_call(SYS_CLOSE,file);
  }
  else{
    system_call(SYS_WRITE,STDOUT,"Invalid number of arguments\n",strlen("Invalid number of arguments\n"));
  }
  return 0;
}



