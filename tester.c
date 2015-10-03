#include "malloc.h"
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "brk.h"


void start_time(struct timeval*);
long stop_time(struct timeval);

int status;
int size_count;

/*Define strategies for more readability*/
#define STRATEGY_FIRST 1
#define STRATEGY_BEST  2
#define STRATEGY_WORST 3
#define STRATEGY_QUICK 4

/*Define test types for more readability*/
#define TYPE_BEST 1
#define TYPE_WORST  2
#define TYPE_REASONABLE_LARGE 3
#define TYPE_REASONABLE_SMALL 4

/*Define allocation sizes for more readability*/
#define SIZE_FRAGMENTATION sizeof(struct node)+4
#define SIZE_SMALL 160
#define SIZE_MEDIUM 220
#define SIZE_LARGE 300
#define SIZE_LARGEST_QUICK 128
#define SIZE_HUGE 10000
#define SIZE_REASONABLE_LARGE 1024
#define SIZE_REASONABLE_SMALL 24

#define BLOCK_AMOUNT 100
#define INTERNAL_ITERATIONS 1000
#define REASONABLE_LARGE_ITERATIONS 100
#define REASONABLE_SMALL_ITERATIONS 1500
#define TEST_ITERATIONS 100

#define SEED 171717913


void start_time(struct timeval*);
long stop_time(struct timeval);
void calculate_sizes(int, int, int*);
void worst_case(void*, void*, int, int, int);
void best_case(void*, int);
void reasonable_case(void**, int, int*, int, int);
void pre_allocate(int, int, int, void**, int, int);
void random_pre_allocate(int*, int, void**, int);
void test(int, int*);
void test_starter(int, int);

int main(int argc, char **argv){
    if(argc < 3){
        perror("You need to specify test type and stratgy!");
        exit(1);
    }
    test_starter(atoi(argv[1]), atoi(argv[2]));
    exit(0);
}


void test_starter(int type, int strategy){
    int i;
    int sizes[100];

    calculate_sizes(type, strategy, sizes);

    for(i = 0; i < TEST_ITERATIONS; ++i){
        if(fork() == 0){
            test(type, sizes);
            exit(0);
        }
        wait(&status);
    }
}

/*Will be executed within a fork*/
void test(int type, int sizes[]){
    void *pointer;
    void *realloc_pointer;
    void *pointers[BLOCK_AMOUNT];
    void *low_heap;
    void *high_heap;
    struct timeval tv;
    long time_passed;

    low_heap = end_heap();
    if(type == TYPE_REASONABLE_LARGE || type ==  TYPE_REASONABLE_SMALL)
        random_pre_allocate(sizes, size_count, pointers, BLOCK_AMOUNT);
    else
        pre_allocate(sizes[0], sizes[1], sizes[2], pointers, BLOCK_AMOUNT, sizes[3]);
    pointer = pointers[0];
    realloc_pointer = pointers[BLOCK_AMOUNT - 2];
    start_time(&tv);

    switch(type){
        case TYPE_BEST: best_case(pointer, sizes[0]); break;
        case TYPE_WORST: worst_case(pointer, realloc_pointer, sizes[0], sizes[1], sizes[2]); break;
        default: reasonable_case(pointers, BLOCK_AMOUNT, sizes, size_count, type); break;
    }

    time_passed = stop_time(tv);
    high_heap = end_heap();

    printf("%ld,%u\n",time_passed, (unsigned)(high_heap - low_heap));
    fflush(stdout);
    
}

void best_case(void *pointer, int allocation_size){
    int i;

    for (i = 0; i < INTERNAL_ITERATIONS; ++i){
        free(pointer);
        pointer = malloc(allocation_size);
        pointer = realloc(pointer, allocation_size);
    }
}

void worst_case(void *pointer, void *realloc_pointer, int allocation_size, int reallocation_size, int fragmentation_size){
    int i;

    for (i = 0; i < INTERNAL_ITERATIONS; ++i){
        free(pointer);
        pointer = malloc(allocation_size);
        reallocation_size += 4;
        realloc_pointer = realloc(realloc_pointer, reallocation_size);
        malloc(fragmentation_size);
    }
}

void reasonable_case(void *pointers[], int pointer_count, int sizes[], int size_count, int type){
    unsigned int seed = SEED;
    int i;
    int pointer_index;
    int iterations;

    if(type == TYPE_REASONABLE_LARGE)
        iterations = REASONABLE_LARGE_ITERATIONS;
    else
        iterations = REASONABLE_SMALL_ITERATIONS;

    for(i = 0; i < iterations; ++i){
        pointer_index = rand_r(&seed)%pointer_count;
        free(pointers[pointer_index]);
        pointers[pointer_index] = malloc((size_t)sizes[rand_r(&seed)%size_count]);
        pointers[pointer_index] = realloc(pointers[pointer_index], (size_t)sizes[rand_r(&seed)%size_count]);
    }
}

void pre_allocate(int primary_size, int secondary_size, int fragmentation_size, void* pointers[], int amount, int first_worst){
    int i = 0;
    int primary_count = amount;
    int fragmentation_count = 0;

    if(secondary_size != 0)
        primary_count = 1;

    if(fragmentation_size != 0)
        fragmentation_count = 1;

    if(first_worst != 0)
        primary_count = amount - fragmentation_count - 1;

    for(; i < primary_count; ++i){
        pointers[i] = malloc(primary_size);
    }

    for(; i < amount-fragmentation_count; ++i){
        pointers[i] = malloc(secondary_size);
    }

    for(; i < amount; ++i){
        pointers[i] = malloc(fragmentation_size);
    }

}

void random_pre_allocate(int sizes[], int size_count, void* pointers[], int amount){
    unsigned int seed = SEED;
    int i;

    for(i = 0; i < amount; ++i){
        pointers[i] = malloc(sizes[rand_r(&seed)%size_count]);
    }
}

/*Calculate allocation sizes according to wiki*/
void calculate_sizes(int type, int strategy, int sizes[]){
    int i;
    sizes[0] = 0;
    sizes[1] = 0;
    sizes[2] = 0;
    sizes[3] = 0;
    switch(type){
        case TYPE_BEST:
            switch(strategy){
                case STRATEGY_FIRST:
                    sizes[0] = SIZE_MEDIUM;
                break;
                case STRATEGY_BEST:
                    sizes[0] = SIZE_MEDIUM;
                    sizes[1] = SIZE_SMALL;
                break;
                case STRATEGY_WORST:
                    sizes[0] = SIZE_MEDIUM;
                    sizes[1] = SIZE_SMALL;
                break;
                case STRATEGY_QUICK:
                    sizes[0] = SIZE_LARGEST_QUICK;
                break;
            }
        break;
        case TYPE_WORST:
            switch(strategy){
                case STRATEGY_FIRST:
                    sizes[0] = SIZE_SMALL;
                    sizes[1] = SIZE_MEDIUM;
                    sizes[2] = SIZE_FRAGMENTATION;
                    sizes[3] = 1;
                break;
                case STRATEGY_BEST:
                    sizes[0] = SIZE_SMALL;
                    sizes[1] = SIZE_MEDIUM;
                    sizes[2] = SIZE_FRAGMENTATION;
                break;
                case STRATEGY_WORST:
                    sizes[0] = SIZE_HUGE;
                    sizes[1] = SIZE_SMALL;
                    sizes[2] = SIZE_FRAGMENTATION;
                break;
                case STRATEGY_QUICK:
                    sizes[0] = SIZE_SMALL;
                    sizes[1] = SIZE_MEDIUM;
                    sizes[2] = SIZE_LARGEST_QUICK+4;
                break;
            }
        break;
        case TYPE_REASONABLE_LARGE:
            size_count = 5;
            for(i = 0; i < size_count; ++i){
                sizes[i] = SIZE_REASONABLE_LARGE+(128*i);
            }
        break;
        case TYPE_REASONABLE_SMALL:
            size_count = 20;
            for(i = 0; i < size_count; ++i){
                sizes[i] = SIZE_REASONABLE_SMALL+(10*i);
            }
        break;
    }
}

void start_time(struct timeval* tv){
    gettimeofday(tv,NULL);
}

long stop_time(struct timeval old_tv){
    int sec, usec;
    long time;
    struct timeval curr_tv;
    gettimeofday(&curr_tv, NULL);
    sec = curr_tv.tv_sec - old_tv.tv_sec;
    usec = curr_tv.tv_usec - old_tv.tv_usec;
    time = sec*1000000+usec;
    return time;
}