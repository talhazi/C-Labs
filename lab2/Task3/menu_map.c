#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

char encrypt(char c){
  if(c<32 | c>126)
    return c;
  else
    return c+3;
}

char decrypt(char c){
  if(c<32 | c>126)
    return c;
  else
    return c-3;
}

char cprt(char c){
  if(c<32 | c>126){
    printf(".\n");
    return c;
  }
  else
    printf("%c\n", c);
    return c;
}

char my_get(char c){
    char nextChar=fgetc(stdin);
    return nextChar;
}
 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  for(int i=0; i<array_length; i++){
    mapped_array[i]=f(*(array+i));
  }
  return mapped_array;
}
 
struct fun_desc {
  char *name;
  char (*fun)(char);
};

int main(int argc, char **argv){
  char* carray = (char*)(malloc(5*sizeof(char)));
  char buf[BUFSIZ];
  int option;
  struct fun_desc menu[] = {{"Encrypt",&encrypt},{"Decrypt",&decrypt},{"Print string",&cprt},{"Get string",&my_get},{NULL,NULL}};
  int bounds = sizeof(menu) / (sizeof(struct fun_desc))-1;
  while(1){
    printf("Please choose a function:\n");
    for (int i=0; i<bounds; i++){
      printf("%d) %s\n", i, menu[i].name);
    }
    printf("Option: ");
    fgets(buf, sizeof(buf), stdin);
    sscanf(buf, "%d", &option);
    if (option<0 | option>bounds-1){
      printf("Not within bounds\n");
      exit(1);
    }
    else{
      printf("Within bounds\n");
      char* tmp = map(carray,5,menu[option].fun);
      free(carray);
      carray = tmp;
      printf("DONE.\n\n");
    }
  }
  free(carray);
}