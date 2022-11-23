#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "umalloc.h"

int main()
{
    int* x;
    int* y;
    int size;

    //case 0: Consistency
    x = (int *) malloc(1);
    *x = 100;
    int address0 = x;
    free(x);
    x = (int *) malloc(1);
    printf("case 0:\naddress of the first pointer: %d\naddress of the second pointer: %d\n", address0, x);
    free(x);


    //case 1: Maximization
    size = 1;
    x = (int *) malloc(size);
    printf("case 1:\n");
    while(x != NULL)
    {
        printf("Success! size = %d, doubling...\n", size);
        free(x);
        size = size * 2;
        x = (int *) malloc(size);
    }
    printf("size = %d produced NULL, halving...\n", size);
    size = size / 2;
    x = (int *) malloc(size);
    while(x == NULL)
    {
        printf("size = %d also produced NULL, halving...\n", size);
        size = size / 2;
        x = (int *) malloc(size);
    }
    printf("size = %d successful! freeing...\n", size);
    free(x);


    //case 2: Basic Coalescence
    size = 1;
    x = (int *) malloc(size);
    printf("case 2:\n");
    while(x != NULL)
    {
        free(x);
        size = size * 2;
        x = (int *) malloc(size);
    }
    size = size / 2;
    x = (int *) malloc(size);
    while(x == NULL)
    {
        size = size / 2;
        x = (int *) malloc(size);
    }
    printf("allocated 1/2 of maximal allocation where size = %d\n", size);
    size = 1;
    y = (int *) malloc(size);
    printf("case 2:\n");
    while(y != NULL)
    {
        free(y);
        size = size * 2;
        y = (int *) malloc(size);
    }
    size = size / 2;
    y = (int *) malloc(size);
    while(y == NULL)
    {
        size = size / 2;
        y = (int *) malloc(size);
    }
    printf("allocated 1/4 of maximal allocation where size = %d\n", size);
    free(x);
    printf("freed pointer 1\n");
    free(y);
    printf("freed pointer 2\n");
    size = default_size;
    x = (int *)malloc(size);
    if(x == NULL)
    {
        printf("failed to allocate full memory\n");
    }
    else
    {
        printf("successfully allocated full memory!\n");
    }

}
