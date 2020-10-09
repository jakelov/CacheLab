#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>

int verbose = 0;

typedef struct Cache
{
    unsigned long int sets;
    int index_bits;
    unsigned long int associativity;
    int block_bits;
    int block_size;
    unsigned long int **tags;
}Cache;

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

unsigned long getBits(unsigned long source, int low, int high)
{
    if (low < 0 || high > 63)
    {
        return 0;
    }
    else
    {
        unsigned long bitsResult = source << (63 - high);
        bitsResult = bitsResult >> (63 - high);
        return bitsResult >> (low);
    }
}

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
        //printf("line = %lu\n", temp_tags[set_index][j]);   
    }
    //printf("\n");

}

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

        //for (int j = 0; j < cache->associativity; j++)
        //{
        //    if (cache->tags[set_index][j] == -1)
        //    {
        //        cache->tags[set_index][j] = tag;
        //        insertAtFront(cache, set_index, j);
        //        break;   
        //    }
        //    else if (j == ((cache->associativity) - 1))
        //    {
        //        cache->tags[set_index][j] = tag;
        //        insertAtFront(cache, set_index, j);
        //        break;
        //    }
        //}

    }
    
    return; 
}

int main(int argc, char** argv)
{
    Cache cache;

    int hit_count = 0;
    int miss_count = 0;
    int eviction_count = 0;
    int option = 0;

    char *traceFile;

    while ((option = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch(option)
        {
            case 'h':
                printUsage();
                exit(0);
            case 'v':
                verbose = 1;
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

    cache.sets = pow(2.0, cache.index_bits);
    cache.block_size = pow(2.0, cache.block_bits);
    
    cache.tags = malloc(cache.sets * sizeof(unsigned long int *));
    
    for (int i = 0; i < cache.sets; i++)
    {
        cache.tags[i] = malloc(cache.associativity * sizeof(unsigned long int));
    }

    for (int j = 0; j < cache.sets; j++)
    {
        for (int k = 0; k < cache.associativity; k++)
        {
            cache.tags[j][k] = -1;
        }
    }

    FILE *file = fopen(traceFile, "r");

    char operation;
    unsigned long address;
    int size;

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


    printSummary(hit_count, miss_count, eviction_count);
    fclose(file);
    return 0;
}
