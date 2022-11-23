#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "umalloc.h"

#define default_size 10485760

int init = -1;
unsigned char heap[default_size];

struct eblock
{
    int isFree;
    // size_t location;
    size_t dataSize; // internal data size. Always >= user specified data size
};

struct eblock *next(struct eblock *block)
{
    char *temp;
    temp = (char *)block;
    temp += sizeof(struct eblock);
    temp += block->dataSize;
    // printf("%d\n", *temp);
    return (struct eblock *)temp;
}

struct eblock *head = (struct eblock *)&heap[0];

void initialize()
{
    memset(heap, 0, sizeof(heap));

    struct eblock *temp = head;
    temp->dataSize = default_size - sizeof(struct eblock);
    temp->isFree = 1;
    memcpy(&heap[0], temp, sizeof(struct eblock));
    init = 0;
}

void newEBlock(struct eblock *recentlyAllocated, int size, int index)
{
    struct eblock *newKid = next(recentlyAllocated);
    newKid->dataSize = recentlyAllocated->dataSize - size;
    newKid->isFree = 1;
    memcpy(&heap[index], newKid, sizeof(struct eblock));
}

void *umalloc(size_t bytes, char *file, int line)
{

    if (bytes <= 0)
    {
        printf("Error on malloc(): byte size is less than or equal to 0 in line %d of %s", line, file);
        exit(1);
    }

    if (bytes >= default_size - sizeof(struct eblock) + 1)
    {
        printf("Error on malloc(): byte size is greater than heap size in line %d of %s", line, file);
        exit(1);
    }

    int assigned_bytes = bytes;
    if (assigned_bytes % 8 > 0)
    {
        assigned_bytes = (assigned_bytes + 8) - (assigned_bytes % 8);
    }

    if (init == -1)
    {
        initialize();

        newEBlock(head, assigned_bytes + sizeof(struct eblock), sizeof(struct eblock) + assigned_bytes);
        head->isFree = 0;
        head->dataSize = assigned_bytes;

        char *result = (char *)head;
        result += sizeof(struct eblock);
        return (void *)result;
    }
    else
    {
        int freeblocksize = 0;
        struct eblock *temp = head;
        init = 0;
        int freeflag = 0;

        while (init < default_size)
        {
            if (temp->isFree)
            {
                freeblocksize += temp->dataSize;
            }
            if (temp->isFree == 1 && temp->dataSize >= assigned_bytes)
            {
                break;
            }
            else
            {
                // printf("before init = %d\n", init);
                init += temp->dataSize + sizeof(struct eblock);
                temp = next(temp);
                // printf("init = %d\n", init);
            }
        }

        if (init >= default_size)
        {
            if (freeblocksize > assigned_bytes)
            {
                printf("Error on malloc(): there is enough free memory, but there is no block large enough for the allocation in line %d of %s", line, file);
                exit(1);
            }
            else if (freeblocksize > 0)
            {
                printf("Error on malloc(): memory is not full but there is not enough free memory for the allocation in line %d of %s", line, file);
                exit(1);
            }
            else
            {
                printf("Error on malloc(): there is no free memory in line %d of %s", line, file);
                exit(1);
            }
        }
        else if (temp->dataSize >= assigned_bytes + sizeof(struct eblock) + 8)
        {
            // temp->dataSize has enough space for a new meta data header
            newEBlock(temp, assigned_bytes + sizeof(struct eblock), init + sizeof(struct eblock) + assigned_bytes);
            temp->isFree = 0;
            temp->dataSize = assigned_bytes;

            char *result = (char *)temp;
            result += sizeof(struct eblock);
            return (void *)result;
        }
        else
        {
            // temp->dataSize only enough space for assigned_bytes (assigned_bytes <= temp->dataSize AND temp->dataSize < assigned_bytes + sizeof(struct eblock) + 8)
            temp->isFree = 0;
            char *result = (char *)temp;
            result += sizeof(struct eblock);
            return (void *)result;
        }
    }
}

void ufree(void *ptr, char *file, int line)
{
    if (ptr == NULL)
    {
        printf("Error on free(): attempted to free() a null pointer in line %d of %s", line, file);
        exit(1);
    }

    char *testptr = (char *)ptr;
    int isInHeap = 0;
    for(int i = 0; i < default_size; i++){
        if (testptr == &heap[i])
        {
            isInHeap = 1;
            break;
        }
    }

    if(!isInHeap){
        printf("Error on free(): No pointer found in line %d of %s", line, file);
        exit(1);
    }

    struct eblock *currBlock = head;
    struct eblock *prevBlock = NULL;
    char *freePtr = ((char *)ptr) - sizeof(struct eblock);
    int count = 0;
    while (count + currBlock->dataSize + sizeof(struct eblock) < default_size)
    {
        if (freePtr == &heap[count])
        {
            // printf("found block!\n");
            break;
        }
        count += sizeof(struct eblock) + currBlock->dataSize;
        prevBlock = currBlock;
        currBlock = next(currBlock);
    }
    if (count + currBlock->dataSize + sizeof(struct eblock) >= default_size)
    {
        printf("Error on free(): Free()ing pointer that was not allocated by malloc() properly in line %d of %s", line, file);
        exit(1);  
    }

    if(currBlock->isFree == 1){
        printf("Error on free(): Free()ing pointer that was already freed in line %d of %s", line, file);
        exit(1);
    }
    else{
        currBlock->isFree = 1;
    }
    
    // printf("final found = %d\n", found);
    if (count + sizeof(struct eblock) + currBlock->dataSize < default_size && next(currBlock)->isFree == 1)
    {
        // printf("consolidated after\n");
        currBlock->dataSize += next(currBlock)->dataSize + sizeof(struct eblock);
    }
    if (prevBlock != NULL && prevBlock->isFree == 1)
    {
        // printf("consolidated before\n");
        prevBlock->dataSize += currBlock->dataSize + sizeof(struct eblock);
    }
}

void printArray()
{
    // struct eblock *curr = head;
    // for(int i = 0; i < default_size; i++)
    // {
    //     if(heap[i] != 0)
    //     {
    //         printf("heap[%d] = %d\n", i, heap[i]);
    //         // printf("heap[%d]->isFree = %d\n", i, curr->isFree);
    //         // printf("heap[%d]->dataSize = %ld\n", i, curr->dataSize);
    //     }
    // }
    struct eblock *temp = head;
    int count = 0;
    while (next(temp) != NULL && count + temp->dataSize + sizeof(struct eblock) <= default_size)
    {
        printf("rheap[%d] = %d\n", count, heap[count]);
        printf("temp->isFree = %d\n", temp->isFree);
        printf("temp->dataSize = %ld\n", temp->dataSize);
        count += temp->dataSize + sizeof(struct eblock);
        temp = next(temp);
    }
}

int main(int argc, char *argv[])
{
    char *x = (char *)malloc(5 * sizeof(char));
    char *y = (char *)malloc(5 * sizeof(char));
    char *z = (char *)malloc(3 * sizeof(char));
    char *a = (char *)malloc(3 * sizeof(char));
    printArray();
    free(x);
    printf("\n");
    printArray();
    free(y);
    printf("\n");
    printArray();
    free(a);
    printf("\n");
    printArray();
    // printArray();
    // size_t temp;
    // memcpy(&temp, &heap[8], sizeof(size_t));
    // printf("%ld", temp);
}
