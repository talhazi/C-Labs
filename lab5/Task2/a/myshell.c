#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 2048

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;                   /* the parsed command line*/
    pid_t pid; 		                /* the process id that is running the command*/
    int status;                     /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	        /* next process in chain */
} process;

process* Globalprocess_list;

void updateProcessList(process **);
void updateProcessStatus(process* , int , int );

process* makeNewProcess(cmdLine* cmd, pid_t pid){
    process* new_process = malloc(sizeof(struct process));
    new_process->cmd=cmd;
    new_process->pid=pid;
    new_process->status=RUNNING;
    new_process->next=NULL;
    return new_process;
}

process* list_append(process* _process, cmdLine* cmd, pid_t pid){
    if(_process==NULL){
        process* new_process = makeNewProcess(cmd,pid);
        _process=new_process;
    }
    else
        _process->next=list_append(_process->next,cmd, pid);
    return _process;
}

/*Receive a process list (process_list), a command (cmd),
and the process id (pid) of the process running the command*/
void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
  (*process_list)=list_append((*process_list),cmd, pid);
}

char* getStatusString(int status){
    if(status == TERMINATED)
      return "Terminated";
    else if(status == RUNNING)
      return "Running";
    else          /*if(status == SUSPENDED)*/
      return "Suspended";
}

void printProcess(int index, process* process){
    char command[100]="";
    if(process!=NULL && process->cmd->argCount>0)
        for(int i=0;i<process->cmd->argCount;i++){
        const char* argument = process->cmd->arguments[i];
        strcat(command,argument);
        strcat(command," ");
    }
    printf("%d\t\t%s\t%s\n",process->pid,command,getStatusString(process->status));
}

void list_print(process* process_list){
    process* curr_process = process_list;
    int i=0;
    while(curr_process != NULL){
        printProcess(i,curr_process);
        curr_process=curr_process->next;
        i++;
    }
}

process* delete_single_process(process* toDelete){
    process* next = toDelete->next;
    freeCmdLines(toDelete->cmd);
    toDelete->cmd=NULL;
    //todelete->next=NULL;
    free(toDelete);
    //todelete=NULL;
    return next;
}
process* deleteTerminatedProcesses(process* process_list){
  if(process_list==NULL)
    return process_list;
  else if(process_list->status==TERMINATED){
    process_list=delete_single_process(process_list);
    return deleteTerminatedProcesses(process_list);
  }
  else
    return deleteTerminatedProcesses(process_list->next);
}

/*print the processes*/
void printProcessList(process** process_list){
    updateProcessList(process_list);
    printf("PID\t\tCommand\t\tSTATUS\n");
    list_print((*process_list));
    (*process_list)=deleteTerminatedProcesses((*process_list));
}

void free_list(process* process_list){
    process* curr_process=process_list;
    if(curr_process!=NULL){
        free_list(curr_process->next);
        freeCmdLines(curr_process->cmd);
        free(curr_process->cmd);
        free(curr_process);
    }
}

/*free all memory allocated for the process list*/
void freeProcessList(process* process_list){
  free_list(process_list);
}

/*go over the process list, and
for each process check if it is done*/
void updateProcessList(process **process_list){
    process* curr_process = (*process_list);
    while(curr_process!=NULL){
        int status;
        int w_status=RUNNING;
        int wait = waitpid(curr_process->pid,&status,WNOHANG);
        if(wait!=0){    //status changed
            if(WIFEXITED(status) || WIFSIGNALED(status)){w_status=TERMINATED;}
            else if(WIFSTOPPED(status)){w_status=SUSPENDED;}
            else if(WIFCONTINUED(status)){w_status=RUNNING;}
            updateProcessStatus(curr_process,curr_process->pid,w_status);
        }
        curr_process=curr_process->next;
    }
}

/*find the process with the given id in the process_list
and change its status to the received status*/
void updateProcessStatus(process* process_list, int pid, int status){
  process_list->status=status;
}

void printCWD(){
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
  else if(strcmp(command->arguments[0],"cd")==0){
    isSpecial=1;
    if(chdir(command->arguments[1])<0)
      perror("bad cd command");
  }
  else if(strcmp(command->arguments[0],"procs")==0){
    isSpecial=1;
    printProcessList(&Globalprocess_list);
  }
  if(isSpecial)
    freeCmdLines(command);
  return isSpecial;
}

void execute(cmdLine* pCmdLine, int debug){
  if(!specialCommand(pCmdLine)){
    pid_t childPid;
    if(!(childPid=fork())){
        if(execvp(pCmdLine->arguments[0],pCmdLine->arguments)<0){
          perror("can't execute the command");
          _exit(EXIT_FAILURE);
      }
    }
    if(childPid!=-1)
      addProcess(&Globalprocess_list,pCmdLine,childPid);
    if(debug){
      fprintf(stderr, "PID: %d\nExecuting command: %s\n",childPid,pCmdLine->arguments[0]);
    }
    if(pCmdLine->blocking){   //& ? 1 : 0
      waitpid(childPid,NULL,0);
    }
  }
}

int main(int argc, char const *argv[]) {
  FILE* input = stdin;
  char buf[BUFFER_SIZE];
  int debug=0;
  Globalprocess_list=NULL;
  for(int i=1;i<argc;i++){
    if((strcmp("-D",argv[i])==0)){
      debug=1;
    }
  }
  while(1){
    printCWD();
    fgets(buf,BUFFER_SIZE,input);
    cmdLine* line = parseCmdLines(buf);
    execute(line,debug);
  }
  return 0;
}