#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define virusHeaderSize (18)
#define nop (0x90)

typedef struct virus {
    unsigned short SigSize; 
    char virusName[16];     
    unsigned char* sig;     
} virus;

typedef struct link link;
struct link {
    link *nextVirus;
    virus *vir;
};

struct newMenu{
    char *name;
    link* (*fun)(link*,const char*);
};

void PrintHex(FILE* output, unsigned char* buffer,unsigned short length) {
    for (int i=0; i<length; i++)
        fprintf(output,"%02hhX ", buffer[i]);
    fprintf(output,"\n\n");
}

virus* readVirus(FILE* input){
  virus* virus = malloc(sizeof(struct virus));
  if(fread(virus,1,virusHeaderSize,input)==virusHeaderSize){
    virus->sig=malloc(virus->SigSize);
    if(fread(virus->sig,1,virus->SigSize,input)==virus->SigSize)
      return virus;
    free(virus->sig);
    return NULL;
  }
  free(virus);
  return NULL;
}

void printVirus(virus* virus, FILE* output){
  fprintf(output,"virus name: %s\n", virus->virusName);
  fprintf(output,"virus size: %d\n", virus->SigSize);
  fprintf(output,"signature:\n");
  PrintHex(output, virus->sig, virus->SigSize);
}

int getSize(FILE* file){
  fseek(file, 0L, SEEK_END);
  int fileSize = ftell(file);
  rewind(file);     //sets the file position to the beginning
  return fileSize;
}

int getLinkSize(link* link){
  if(link==NULL)
    return 0;
  return getLinkSize(link->nextVirus)+1;
}

link* getLink(link* node, int index){
  if(index==0)
    return node;
  return getLink(node->nextVirus,index-1);
}


/* Print the data of every link in list to the given stream. Each item followed by a newline character. */
void list_print(link *virus_list, FILE* input){
  link* currLink = virus_list;
  while (currLink!=NULL){
    printVirus(currLink->vir,input);
    currLink = currLink->nextVirus;
  }
}
 
link* newLink(virus* data){
    link* newLink = malloc(sizeof(struct link));
    newLink->vir=data;
    newLink->nextVirus=NULL;
    return newLink;
}

/*  Add a new link with the given data to the list 
    (either at the end or the beginning, depending on what your TA tells you),
    and return a pointer to the list (i.e., the first link in the list).
    If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data){
  if (virus_list == NULL)
    virus_list = newLink(data);
  else
    virus_list->nextVirus = list_append(virus_list->nextVirus,data);
  return virus_list;
}

/* Free the memory allocated by the list. */
void list_free(link *virus_list){
  if(virus_list!=NULL){
    list_free(virus_list->nextVirus);
    free(virus_list->vir->sig);
    free(virus_list->vir);
    free(virus_list);
  }
  return;
}

link* load_list(FILE* file){
    link* head = NULL;
    int fileSize = getSize(file);
    char buffer[4];
    fread(&buffer, 1, 4, file);
    int bytes = 4;       //readen bytes
    while(bytes<fileSize){
        virus* nextVirus = readVirus(file);  //to free??
        head=list_append(head,nextVirus);
        bytes=bytes+virusHeaderSize+nextVirus->SigSize;
    }
    return head;
}

link* load_signatures(link* link, const char* unusedFile){
  char* fileName=NULL;
  char buf[BUFSIZ];
  printf("Enter signature file name: ");
  fgets(buf,sizeof(buf),stdin);
  sscanf(buf,"%ms",&fileName);
  FILE* file = fopen(fileName,"rb");
  if(file==NULL){
    fprintf(stderr,"Reading File Error\n");
    exit(EXIT_FAILURE);
  }
  free(fileName);
  struct link *head = load_list(file);
  fclose(file);
  return head;
}

link *print_signatures(link *list, const char* unusedFile){
  list_print(list, stdout);
  return list;
}

void detect_virus(char *buffer, unsigned int size, link *virus_list, FILE* output){
  for (int offset = 0; offset < size; offset++){
    for (int i = 0; i < getLinkSize(virus_list); i++){
      virus *virus = NULL;
      virus = getLink(virus_list, i)->vir;
      int cmp = -1;
      if(size-offset >= virus->SigSize)
        cmp=memcmp(buffer+offset, virus->sig, virus->SigSize);
      if (cmp==0){
        fprintf(output, "Starting byte: %d\n", offset);
        fprintf(output, "Virus name: %s\n", virus->virusName);
        fprintf(output, "Virus size: %d\n", virus->SigSize);
      }
    }
  }
}

link* detect_viruses(link* list, const char* fileName){
    FILE* file = fopen(fileName,"rb");
    if(file==NULL){
      fprintf(stderr,"Reading File Error\n");
      exit(EXIT_FAILURE);
    }
    unsigned int fileSize = getSize(file);
    char* buf = NULL;
    buf = malloc(fileSize);
    fread(buf,1,fileSize,file);
    fclose(file);
    detect_virus(buf,fileSize,list,stdout);
    free(buf);
    return list;
}

void kill_virus(FILE *fileName, int signitureOffset, int signitureSize){  //CHANGED char* to FILE*
  fseek(fileName, signitureOffset,SEEK_SET);
  char *change = malloc(signitureSize);
  for(int i=0;i<signitureSize;i++)
    change[i]=nop;
  fwrite(change,1,signitureSize,fileName);
  free(change);
}

link* fix_file(link* list, const char* fileName){
    unsigned short byteLocation;
    unsigned short signatureSize;
    FILE *file = fopen(fileName,"r+");
    if(file==NULL){
      fprintf(stderr,"Reading File Error\n");
      exit(EXIT_FAILURE);
    }
    printf("Enter Starting byte and Virus size:\n");
    scanf("%hd %hd",&byteLocation,&signatureSize);
    kill_virus(file, byteLocation,signatureSize);
    fclose(file);
    return list;
}

int main(int argc, char **argv) {
  link* list = NULL;
  FILE* file=NULL;
  struct newMenu menu[] = {{"Load signatures",load_signatures},{"Print signatures",print_signatures},
                          {"Detect viruses",detect_viruses},{"Fix file", fix_file},{NULL, NULL}};
  int bounds = sizeof(menu) / (sizeof(struct newMenu))-1;
  int option;
  const char* fileName;
  if (argc>1){
    fileName = argv[1];
    file=fopen(fileName,"rb");
    if(file==NULL){
      fprintf(stderr,"Reading File Error\n");
      exit(EXIT_FAILURE);
    }
    fclose(file);
  }
  while(1){
    for (int i = 0; i < bounds; i++){
      printf("%d) %s\n", i + 1, menu[i].name);
    }
    printf("Option: ");
    scanf("%d", &option);
    fgetc(stdin);
    if (option < 1 || option > bounds){
      printf("Not within bounds\n");
      exit(EXIT_SUCCESS);
    }
    else{
      list = menu[option-1].fun(list, fileName);
      printf("\n");
    }
  }
  list_free(list);
  return 0;
}