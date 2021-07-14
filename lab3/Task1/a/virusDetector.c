#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize; //2
    char virusName[16];     //16
    unsigned char* sig;     //8
} virus;

void PrintHex(FILE* output, unsigned char* buffer,unsigned short length) {
    for (int i=0; i<length; i++)
        fprintf(output,"%02hhX ", buffer[i]);
    fprintf(output,"\n\n");
}

virus* readVirus(FILE* input){
  virus* virus = malloc(sizeof(struct virus));
  if(fread(virus,1,18,input)!=0){
    virus->sig=malloc(virus->SigSize);
    fread(virus->sig,1,virus->SigSize,input);
  }
  return virus;
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


int main(int argc, char **argv) {
  FILE* input = fopen(argv[1],"r");
  if (input==NULL){
    fprintf(stderr,"Reading File Error\n");
    exit(EXIT_FAILURE);
  }
  FILE* output = stdout;
  int fileSize = getSize(input);
  char buffer[4];
  fread(&buffer, 1, 4, input);
  int bytes = 4;       //readen bytes
  while(bytes<fileSize){
    virus* nextVirus = readVirus(input);
    printVirus(nextVirus,output);
    bytes = bytes+18+nextVirus->SigSize;
    free(nextVirus);
  }
  return 0;
}