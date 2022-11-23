#ifndef UMALLOC_H
#define UMALLOC_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#define default_size 10485760

// #ifndef malloc(X)
#define malloc(X) umalloc(X, __FILE__, __LINE__)
void* umalloc( size_t, char*, int);

// #ifndef free(Y)
#define free(Y) ufree(Y, __FILE__, __LINE__)
void ufree( void*, char*, int);

struct eblock
{
    int isFree;
    // size_t location;
    size_t dataSize; // internal data size. Always >= user specified data size
};

struct eblock *next(struct eblock *block);
void initialize();
void newEBlock(struct eblock *recentlyAllocated, int size, int index);
void printArray();

// #endif
// #endif
#endif
