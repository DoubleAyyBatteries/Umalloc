#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "umalloc.h"

#define default_size 10485760

int init = 0;
unsigned char heap[default_size];

struct eblock
{
    int isfree;
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
    temp->isfree = 1;
    memcpy(&heap[0], temp, sizeof(struct eblock));
    init = 1;
}

void *umalloc(size_t bytes)
{

    if (bytes <= 0)
    {
        return NULL;
    }

    if (!init)
    {
        initialize();

        int assigned_bytes = bytes;
        while(assigned_bytes % 8 != 0){
            assigned_bytes++;
        }

        struct eblock *temp = head;
        while(temp != NULL && temp->isfree != 1){
            if(temp->dataSize < assigned_bytes)
                temp = next(temp);
            else
                break;
        }
        if(temp == NULL){
            perror("No more free space");
        }
        else{
            if(temp->dataSize == assigned_bytes){
                temp->isfree = 0;
                return temp;
            }
            else{
                if(temp->dataSize >= assigned_bytes + sizeof(struct eblock) + 8){
                    temp->isfree = 0;
                    temp->dataSize = assigned_bytes;
                    //make new metadata header with isfree == 1
                }
                else{
                    temp->isfree = 0;
                    return temp;
                }
                

            }
        }
    }
    return NULL;
}

void ufree(void *ptr)
{
}

void printArray(){
    for(int i = 0; i < default_size; i++){
        if(heap[i] != 0){
            printf("heap[%d] = %d\n", i, heap[i]);
        }
    }
    printf("head isFree = %d\n", head->isfree);
    printf("head dataSize = %ld\n", head->dataSize);
}

int main(int argc, char *argv[]){
    initialize();
    printArray();
    size_t temp;
    memcpy(&temp, &heap[8], sizeof(size_t));
    printf("%ld", temp);
}