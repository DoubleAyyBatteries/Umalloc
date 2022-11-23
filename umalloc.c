
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

void *umalloc(size_t bytes, char* file, int line)
{

    if (bytes <= 0 || bytes >= default_size - sizeof(struct eblock) + 1)
    {
        return NULL;
    }

    int assigned_bytes = bytes;
    if(assigned_bytes % 8 > 0)
    {
        assigned_bytes = (assigned_bytes + 8) - (assigned_bytes % 8);
    }
    
    if (init == -1)
    {
        initialize();

        newEBlock(head, assigned_bytes + sizeof(struct eblock), sizeof(struct eblock) + assigned_bytes);
        head->isFree = 0;
        head->dataSize = assigned_bytes;

        char *result = (char *) head;
        result += sizeof(struct eblock);
        return (void*)result;
    }
    else
    {
        struct eblock *temp = head;
        init = 0;
        while(init < default_size)
        {
            if(temp->isFree == 1 && temp->dataSize >= assigned_bytes)
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
        if(init >= default_size)
        {
            perror("No more free space");
        }
        else if(temp->dataSize >= assigned_bytes + sizeof(struct eblock) + 8)
        {
            //temp->dataSize has enough space for a new meta data header
            newEBlock(temp, assigned_bytes + sizeof(struct eblock), init + sizeof(struct eblock) + assigned_bytes);
            temp->isFree = 0;
            temp->dataSize = assigned_bytes;

            char *result = (char *) temp;
            result += sizeof(struct eblock);
            return (void*)result;
        }
        else
        {
            //temp->dataSize only enough space for assigned_bytes (assigned_bytes <= temp->dataSize AND temp->dataSize < assigned_bytes + sizeof(struct eblock) + 8)
            temp->isFree = 0;
            char *result = (char *) temp;
            result += sizeof(struct eblock);
            return (void*)result;
        }
        return NULL;
    }
}

void ufree(void *ptr, char* file, int line)
{
    if(ptr == NULL)
    {
        return;
    }

    struct eblock *currBlock = head;
    struct eblock *prevBlock = NULL;
    char *freePtr = ((char*) ptr) - sizeof(struct eblock);
    int count = 0;
    while(count + currBlock->dataSize + sizeof(struct eblock) <= default_size)
    {
        if(freePtr == &heap[count])
        {
            // printf("found block!\n");
            break;
        }
        count += sizeof(struct eblock) + currBlock->dataSize;
        prevBlock = currBlock;
        currBlock = next(currBlock);
    }
    currBlock->isFree = 1;
    // printf("final found = %d\n", found);
    if(count + sizeof(struct eblock) + currBlock->dataSize < default_size && next(currBlock)->isFree == 1)
    {
        // printf("consolidated after\n");
        currBlock->dataSize += next(currBlock)->dataSize + sizeof(struct eblock);
    }
    if(prevBlock != NULL && prevBlock->isFree == 1)
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
    //         printf("heap[%d]->isFree = %d\n", i, curr->isFree);
    //         printf("heap[%d]->dataSize = %ld\n", i, curr->dataSize);
    //     }
    // }
    struct eblock *temp = head;
    int count = 0;
    while(count + temp->dataSize + sizeof(struct eblock) <= default_size)
    {
        printf("rheap[%d] = %d\n", count, heap[count]);
        printf("rheap[%d]->isFree = %d\n", count, temp->isFree);
        printf("rheap[%d]->dataSize = %ld\n", count, temp->dataSize);
        count += temp->dataSize + sizeof(struct eblock);
        temp = next(temp);
    }
}

void prettyPrint()
{
    struct eblock *temp = head;
    int count = 0;
    int inUse = 0;
    int free = 0;
    while(count + temp->dataSize + sizeof(struct eblock) <= default_size)
    {
        if(temp->isFree == 0)
        {
            inUse += temp->dataSize;
        }
        else
        {
            free += temp->dataSize;
        }
        inUse += sizeof(struct eblock);
        count += temp->dataSize + sizeof(struct eblock);
        temp = next(temp);
    }
    printf("Amount of Data in Use: %d bytes\nAmount of Data Available: %d bytes\n", inUse, free);
}

int main(int argc, char *argv[])
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
    prettyPrint();

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
    prettyPrint();


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
    prettyPrint();


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
    prettyPrint();


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
    prettyPrint();
    

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
    prettyPrint();
    return 0;
}
