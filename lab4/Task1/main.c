#include "util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SEEK_SET 0

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_CREATE 64

#define STDIN 0
#define STDOUT 1
#define STDERR 2

extern int system_call();

void errorHandler(int output,char* message){
  system_call(SYS_WRITE,output,message, strlen(message));
  system_call(SYS_EXIT,0x55,0,0);
}

int main (int argc , char* argv[], char* envp[])
{
  int input = STDIN;
  int output = STDOUT;

  char nextChar;
  int debug=0;
  int i;
  char *name;
  int numOfLetters=0;

  int checkFile;

  for(i=1; i<argc; i++){
    if((strcmp("-D",argv[i])==0)){
      debug=1;
    }
    else if(strncmp(argv[i],"-i",2)==0){
      int inputFile;
      name = argv[i]+2;
      inputFile = system_call(SYS_OPEN,name,O_RDONLY,0777);
      if(inputFile<=0)
        errorHandler(STDOUT,"inputFile Error\n");
      input=inputFile;
    }
    else if(strncmp(argv[i],"-o",2)==0){
      int outputFile;
      name = argv[i]+2;
      outputFile=system_call(SYS_OPEN,name,O_WRONLY | O_CREATE,0777);
      if(outputFile<=0)
        errorHandler(STDOUT,"outputFile Error\n");
      output=outputFile;
    }
    else{
      errorHandler(output,"Invalid parameter\n");
    }
  }


  while(1){
    checkFile=system_call(SYS_READ,input,&nextChar,1);
    if(checkFile<=0 || nextChar!='\n'){
      if(!debug){
        if(checkFile<=0){
          errorHandler(output,"\n");
        }
        if(nextChar>='A' && nextChar<='Z'){
          nextChar=nextChar+32;
        }
        checkFile=system_call(SYS_WRITE,output,&nextChar,1);
        if(checkFile<=0){
          errorHandler(output,"\n");
        }
      }
      if(debug){
        if(checkFile<=0){
          errorHandler(output,"\n");
        }
        system_call(SYS_WRITE,STDERR,itoa(nextChar),strlen(itoa(nextChar)));
        system_call(SYS_WRITE,STDERR,"\t",1);
        if(nextChar>='A' && nextChar<='Z'){
          numOfLetters++;
          system_call(SYS_WRITE,STDERR,itoa(nextChar+32),strlen(itoa(nextChar+32)));
          system_call(SYS_WRITE,STDERR,"\n",1);
        }
        else{
          system_call(SYS_WRITE,STDERR,itoa(nextChar),strlen(itoa(nextChar)));
          system_call(SYS_WRITE,STDERR,"\n",1);
        }
        if(checkFile<=0){
          errorHandler(output,"\n");
        }
      }
    }
    else{
      system_call(SYS_WRITE,output,"\n",1);
    }
  }

  if(debug){
    system_call(SYS_WRITE,STDERR,"The number of letters: ",strlen("\nthe number of letters: "));
    system_call(SYS_WRITE,STDERR,itoa(numOfLetters),strlen(itoa(numOfLetters)));
    system_call(SYS_WRITE,STDERR,"\n",1);
  }

  system_call(SYS_CLOSE,input,0,0);
  system_call(SYS_CLOSE,output,0,0);
  return 0;
}



