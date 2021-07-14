#include <stdio.h>
#include <stdlib.h>

void PrintHex(char* buffer,int length) {
    for (int i=0; i<length; i++)
        fprintf(stdout,"%02hhX ", buffer[i]);
}

int main(int argc, char **argv) {
    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        printf("File Error\n");
        return 1;
    }
    char buffer[BUFSIZ];
    int count = fread(&buffer, sizeof(char), BUFSIZ, input);
    PrintHex(buffer, count);
    printf("\n");
    fclose(input);
    return 0;
}