#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE (128)

int main(int argc, char **argv) {
  FILE * output=stdout;
  FILE * input=stdin;

  char nextChar;
  int a=1,b=0,c=0;
  int i;
  char value;
  char *name;
  int numOfLetters=0;

  for(i=1; i<argc; i++){ //args
    if(strcmp(argv[i],"-D")==0){ //b
      printf("-D\n");
	    b=1;
    }

    else if(strncmp(argv[i],"-e", 2)==0){ //c-
      c=1;
      a=0;
      value = argv[i][2];
      if(value<='9')
        value = value-48;
      else if(value>='A')
        value = value-55;
    }
    else if(strncmp(argv[i],"+e", 2)==0){ //c+
	    c=2;
      a=0;
      value = argv[i][2];
      if(value<='9')
        value = value-48;
      else if(value>='A')
        value = value-55;
    }

    else if(strncmp(argv[i],"-i", 2)==0){ //d
      name = argv[i]+2;
      input = fopen(name, "r");
      if (input == NULL) {
        printf("File Error");
      }
    }

    else if(strncmp(argv[i],"-o", 2)==0){ //2
      name = argv[i]+2;
      output = fopen(name, "w");
    }

    else{
	    printf("invalid parameter >> %s\n",argv[i]);
	    return 1;
    }
  }


  do {
    nextChar=fgetc(input);
    if(feof(input) | nextChar == '\n') {
      break;
    }

    if (a == 1 & b==0){  //a
      if(nextChar>='A' && nextChar<='Z'){
        nextChar=nextChar+32;
        fprintf(output,"%c", nextChar);
      }
      else
        fprintf(output,"%c", nextChar);
    }

    if(b == 1 & c==0){   //b&a
      fprintf(stderr,"%d    ", nextChar);
      if(nextChar>='A' && nextChar<='Z'){
        numOfLetters++;
        nextChar=nextChar+32;
        fputc(nextChar, output);
      }
      else
        fputc(nextChar, output);
      fprintf(stderr,"%d\n", nextChar);
    }
    if(b == 1 & c==1){   //b&c-
      fprintf(stderr,"%d    ", nextChar);
      numOfLetters++;
      nextChar=nextChar-value;
      putc(nextChar, output);
      fprintf(stderr,"%d\n", nextChar);
    }
    if(b == 1 & c==2){   //b&c+
      fprintf(stderr,"%d    ", nextChar);
      numOfLetters++;
      nextChar=nextChar+value;
      putc(nextChar, output);
      fprintf(stderr,"%d\n", nextChar);
    }

    if(c == 1 & b==0){   //c-
      nextChar=nextChar-value;
      fprintf(output,"%c", nextChar);
    }
    if(c == 2 & b==0){   //c+
      nextChar=nextChar+value;
      fprintf(output,"%c", nextChar);
    }

   } while(1);


   if(b == 1){   //b
    fprintf(stderr,"\nthe number of letters: %i\n\n", numOfLetters);
    putc('\n', output);
   }

  if (c!=0 | a==1)   //c
    fprintf(output,"%c", '\n');

  fclose(input);
  return 0;
}
