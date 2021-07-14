#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char** argv){
    // //1c
    // int iarray[3];
    // float farray[3];
    // double darray[3];
    // char carray[3];
    // int* iptrarray[3];
    // char* cptrarray[3];

    // printf("- &iarray: %p\n",&iarray);
    // printf("- &farray: %p\n",&farray);
    // printf("- &darray: %p\n",&darray);
    // printf("- &carray: %p\n",&carray);
    // printf("- &iptrarray: %p\n",&iptrarray);
    // printf("- &cptrarray: %p\n",&cptrarray);

    // printf("- iarray: %p\n",iarray);
    // printf("- iarray+1: %p\n",iarray+1);
    // printf("- farray: %p\n",farray);
    // printf("- farray+1: %p\n",farray+1);
    // printf("- darray: %p\n",darray);
    // printf("- darray+1: %p\n",darray+1);
    // printf("- carray: %p\n",carray);
    // printf("- carray+1: %p\n",carray+1);
    // printf("- iptrarray: %p\n",iptrarray);
    // printf("- iptrarray+1: %p\n",iptrarray+1);
    // printf("- cptrarray: %p\n",cptrarray);
    // printf("- cptrarray+1: %p\n",cptrarray+1);

    //1d
    int iarray[] = {1,2,3};
    char carray[] = {'a','b','c'};
    int* iarrayPtr = &iarray[0];
    char* carrayPtr = &carray[0];
    int p;

    for (int i=0; i<3; i++)
        printf("- iarray[%d]: %d\n",i,*iarrayPtr+i);
    for (int i=0; i<3; i++)
        printf("- carray[%d]: %c\n",i,*carrayPtr+i);
    printf("- p: %p\n",p);

    return 0;
}