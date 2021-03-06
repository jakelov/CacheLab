#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
/**
 *  
 *   Jacob Lovingood, Spencer Withee
 *
 *   lovingoodja1, witheesc
 *  
 */

// Verbose flag used for verbose output.
int verbose = 0;

/** Cache holds different variables for abstraction of a cache.
 * Associativity: Number of lines per set (slots).
 * Index Bits: Equalivalent to the s in 2^s for the number of sets.
 * Block Bits: Number of bits for a block.
 * Block Size: The number 2^b where b is the number of bits per block.
 * Sets: The number 2^s which is the number of sets in the cache.
 * Tags is a ref to number of tags.
 */
typedef struct Cache
{
    unsigned long int sets;
    int index_bits;
    unsigned long int associativity;
    int block_bits;
    int block_size;
    unsigned long int **tags;
}Cache;

/**
 * Print for help calling and testing the Cache.
*/
void printUsage()
{
    printf("Usage: ./csim-ref [-h] [-v] -s <s> -E <E> -b <b> -t <tracefile>\n");
    printf("-h: Optional help flag that prints usage info\n");
    printf("-v: Optional verbose flag that displays trace info\n");
    printf("-s <s>: Number of set index bits (S = 2^s is the number of sets)\n");
    printf("-E <E>: Associativity (number of lines per set)\n");
    printf("-b <b>: Number of block bits (B = 2^b is the block size)\n");
    printf("-t <tracefile>: Name of the valgrind trace to replay\n");
}

/**
 * Inserts at the front of the array to continue the trend that tags[setindex][0] is the LRU line
*/
void insertAtFront(Cache *cache, int set_index, int index)
{
    unsigned long int temp_tags[cache->sets][cache->associativity];

    temp_tags[set_index][0] = cache->tags[set_index][index];

    for (int i = 0; i < cache->associativity; i++)
    {
        if ((i != index) && (i + 1 != cache->associativity))
        {
            temp_tags[set_index][i + 1] = cache->tags[set_index][i];

        }
        else if (i != index)
        {
            temp_tags[set_index][i] = cache->tags[set_index][i];
        }

    }

    for (int j = 0; j < cache->associativity; j++)
    {
        cache->tags[set_index][j] = temp_tags[set_index][j];     
    }
}

/**
 * Function to access the cache and determine if the access is a hit, miss, or eviction
*/
void accessCache(Cache *cache, unsigned long address, int *hit_count, int *miss_count, int *eviction_count)
{

    unsigned long int tag = address >> ((cache->index_bits) + (cache->block_bits));
    int set_index = (address >> (cache->block_bits)) - (tag << (cache->index_bits));

    

    int hit_bool = 0;
    int evict_bool = 1;

    for (int i = 0; i < cache->associativity; i++)
    {
        if (cache->tags[set_index][i] != -1)
        {
            if (cache->tags[set_index][i] == tag)
            {
                hit_bool = 1;
                insertAtFront(cache, set_index, i);
                break;
            }
        }
        else
        {
            evict_bool = 0;
            break;
        }
    }

    if (hit_bool)
    {
        *hit_count = *hit_count + 1;
        if (verbose)
        {
            printf(" hit");
        }
    }
    else
    {
        *miss_count = *miss_count + 1;
        if (verbose)
        {
            printf(" miss");
        }

        if (evict_bool)
        {
            *eviction_count = *eviction_count + 1;
            if (verbose)
            {
                printf(" eviction");
            }
        }

        cache->tags[set_index][(cache->associativity) - 1] = tag;
        insertAtFront(cache, set_index, ((cache->associativity) - 1));
    }
    
    return; 
}
/**
 * Main Method to run the program, determine what type of operation is occuring, and perform the operation. Also does cache setup/global flag initialization.
*/
int main(int argc, char** argv)
{
    Cache cache;

    int hit_count = 0;
    int miss_count = 0;
    int eviction_count = 0;
    int option = 0;

    char *traceFile;
    // Determine what arguments were passed.
    while ((option = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        // Initialization of fields.
        switch(option)
        {
            case 'h':
                printUsage();
                exit(0);
            case 'v':
                verbose = 1; // verbose flag.
                break;
            case 's':
                cache.index_bits = atoi(optarg);
                break;
            case 'E':
                cache.associativity = atoi(optarg);
                break;
            case 'b':
                cache.block_bits = atoi(optarg);
                break;
            case 't':
                traceFile = optarg;
                break;
            default:
                exit(1);
        }
    }
    // Field Initialization
    cache.sets = pow(2.0, cache.index_bits);
    cache.block_size = pow(2.0, cache.block_bits);
    // Array Allocation
    cache.tags = malloc(cache.sets * sizeof(unsigned long int *));
  
    for (int i = 0; i < cache.sets; i++)
    {
        cache.tags[i] = malloc(cache.associativity * sizeof(unsigned long int));
    }
    // initialization of the tags for validity and checking for need to eviction in access.
    for (int j = 0; j < cache.sets; j++)
    {
        for (int k = 0; k < cache.associativity; k++)
        {
            cache.tags[j][k] = -1;
        }
    }
    // File opening/reading.
    FILE *file = fopen(traceFile, "r");

    char operation;
    unsigned long address;
    int size;
    // Scan the file and initialize the operation/address/size fields.
    if (file != NULL)
    {
        while(fscanf(file, " %c %lx,%d", &operation, &address, &size) == 3)
        {
            switch(operation)
            {
                case 'I':
                    break;
                case 'L':
                    if (verbose)
                    {
                        printf("%c %lx,%d", operation, address, size);
                        accessCache(&cache, address, &hit_count, &miss_count, &eviction_count);
                        printf("\n");
                    }
                    else
                    {
                        accessCache(&cache, address, &hit_count, &miss_count, &eviction_count);
                    }
                    break;
                case 'S':
                    if (verbose)
                    {
                        printf("%c %lx,%d", operation, address, size);
                        accessCache(&cache, address, &hit_count, &miss_count, &eviction_count);
                        printf("\n");
                    }
                    else
                    {
                        accessCache(&cache, address, &hit_count, &miss_count, &eviction_count);
                    }
                    break;
                case 'M':
                    if (verbose)
                    {
                        printf("%c %lx,%d", operation, address, size);
                        accessCache(&cache, address, &hit_count, &miss_count, &eviction_count);
                        accessCache(&cache, address, &hit_count, &miss_count, &eviction_count);
                        printf("\n");
                    }
                    else
                    {
                        accessCache(&cache, address, &hit_count, &miss_count, &eviction_count);
                        accessCache(&cache, address, &hit_count, &miss_count, &eviction_count);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // Print and close.
    printSummary(hit_count, miss_count, eviction_count);
    fclose(file);

    //free memory
    for (int x = 0; x < cache.sets; x ++)
    {
        free(cache.tags[x]);
    }
    free(cache.tags);

    return 0;
}
