#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE (128)

int main(int argc, char **argv) {
  
  fputs(argv[1], stdout);
  char buf[BUFFER_SIZE];
  fflush(stdout);
  fgets(buf, BUFFER_SIZE, stdout);
  fprintf(stdout,"\n");

  return 0;
}
