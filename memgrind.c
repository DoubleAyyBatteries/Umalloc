
#include <unistd.h>

#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "umalloc.h"

int memorygrind()
{
    int* x;
    int* y;
    int size;
    int* pointers[10000];

    //case 0: Consistency
    x = (int *) malloc(1);
    *x = 100;
    int address0 = x;
    free(x);
    x = (int *) malloc(1);
    int address1 = x;
    printf("case 0:\ndo the addresses match? (1 is YES! 0 is NO!) %d\n", address0 == address1);
    free(x);


    //case 1: Maximization
    size = 1;
    x = (int *) malloc(size);
    printf("\ncase 1:\n");
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
    printf("\ncase 2:\n");
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
    size = size / 2;
    y = (int *) malloc(size);
    printf("allocated 1/4 of maximal allocation where size = %d\n", size);
    free(x);
    printf("freed pointer 1\n");
    free(y);
    printf("freed pointer 2\n");
    size = default_size - sizeof(struct eblock);
    x = (int *)malloc(size);
    if(x == NULL)
    {
        printf("failed to allocate full memory\n");
    }
    else
    {
        printf("successfully allocated full memory!\n");
    }
    free(x);


    //case 3: saturation
    printf("\ncase 3:\n");
    for(int i = 0; i < 9216; i++)
    {
        pointers[i] = (int*)malloc(1024);
    }
    printf("pointers is now over 9000\n");
    for(int i = 9216; i < 10000; i++)
    {
        pointers[i] = (int*)malloc(1);
        size = i;
        if(pointers[i] == NULL)
        {
            printf("\nSTOP\n");
            break;
        }
    }
    printf("saturation of space (without 9216 allocations): %d\n", (size-9217));

    //case 4: time overhead
    free(pointers[size]);
    struct timeval c4Start, c4End;
    printf("\ncase 4:\ntime start!\n");
    gettimeofday(&c4Start, 0);
    pointers[size] = (int*)malloc(1);
    gettimeofday(&c4End, 0);
    printf("time end!\n");
    int totalTime = (c4End.tv_sec-c4Start.tv_sec) * 1000000 + (c4End.tv_usec-c4Start.tv_usec);
    printf("max time overhead = %d ms\n", totalTime);
    

    //case 5: intermediate coalescence
    printf("\ncase 5:\nfreeing memory...\n");
    for(int i = 0; i < size+1; i++)
    {
        free(pointers[i]);
    }
    printf("freed all of pointers\n");
    size = default_size-sizeof(struct eblock);
    x = (int *)malloc(size);
    if(x == NULL)
    {
        printf("failed to allocate full memory\n");
    }
    else
    {
        printf("successfully allocated full memory!\n");
    }
    free(x);
    return 0;
}
