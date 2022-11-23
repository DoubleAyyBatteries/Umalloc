
#ifndef UMALLOC_H_
#define UMALLOC_H_
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#define default_size 10485760

#define malloc(X) umalloc(X, __FILE__, __LINE__)
#define free(Y) ufree(Y, __FILE__, __LINE__)

void* umalloc( size_t, char*, int);
void ufree( void*, char*, int);

struct eblock *next(struct eblock *block);
void initialize();
void newEBlock(struct eblock *recentlyAllocated, int size, int index);
void printArray();

#endif
