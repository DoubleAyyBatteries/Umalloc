#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#define default_size 10485760

#define malloc(x) umalloc(x, __FILE__, __LINE__)
#define free(x) ufree(x, __FILE__, __LINE__)

void* umalloc( size_t, char*, int);
void ufree( void*, char*, int);
