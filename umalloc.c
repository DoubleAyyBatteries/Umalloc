#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "umalloc.h"

#define default_size 10485760

int init = -1;
unsigned char heap[default_size];

struct eblock
{
    int isFree;
    // size_t location;
    size_t dataSize; //internal data size. Always >= user specified data size
};

struct eblock *next(struct eblock *block)
{
    char *temp;
    temp = (char *)block;
    temp += sizeof(struct eblock);
    temp += block->dataSize;
    return (struct eblock *)temp;
}

struct eblock *head = (struct eblock *)&heap[0];

void initialize()
{
    memset(heap, 0, sizeof(heap));

    struct eblock *temp = head;
    temp->dataSize = default_size - sizeof(head);
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

void *umalloc(size_t bytes)
{

    if (bytes <= 0)
    {
        return NULL;
    }

    if (init == -1)
    {
        initialize();

        int assigned_bytes = bytes;
        if(assigned_bytes % 8 > 0)
        {
            assigned_bytes = (assigned_bytes + 8) - (assigned_bytes % 8);
        }

        struct eblock *temp = head;
        while(temp != NULL && temp->isFree != 1)
        {
            if(temp->dataSize < assigned_bytes)
            {
                init += temp->dataSize + sizeof(struct eblock);
                temp = next(temp);
            }
            else
            {
                break;
            }
        }
        if(temp == NULL)
        {
            perror("No more free space");
        }
        else if(temp->dataSize >= assigned_bytes + sizeof(struct eblock) + 8)
        {
            //temp->dataSize has enough space for a new meta data header
            newEBlock(temp, assigned_bytes + sizeof(struct eblock), init + sizeof(struct eblock) + assigned_bytes);
            temp->isFree = 0;
            temp->dataSize = assigned_bytes;
        }
        else
        {
            //temp->dataSize only enough space for assigned_bytes (assigned_bytes <= temp->dataSize AND temp->dataSize < assigned_bytes + sizeof(struct eblock) + 8)
            temp->isFree = 0;
            return temp;
        }
    }
    return NULL;
}

void ufree(void *ptr)
{
    if(ptr == NULL)
    {
        return;
    }

    // struct eblock *freePtr = (struct eBlock*)ptr;
}

void printArray()
{
    for(int i = 0; i < default_size; i++)
    {
        if(heap[i] != 0)
        {
            printf("heap[%d] = %d\n", i, heap[i]);
        }
    }
    printf("head isFree = %d\n", head->isFree);
    printf("head dataSize = %ld\n", head->dataSize);
}

int main(int argc, char *argv[])
{
    umalloc(1000);
    printArray();
    // size_t temp;
    // memcpy(&temp, &heap[8], sizeof(size_t));
    // printf("%ld", temp);
}
