#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


void startTime(struct timeval*);
long stopTime(struct timeval);

int status;

#define SMALL_NUMBERS 10000
#define SMALL_SIZE 1
#define SMALL_RANDOM 149
#define SMALL_TIMES 100

#define BIG_NUMBERS 100
#define BIG_SIZE 8000
#define BIG_RANDOM 2000
#define BIG_TIMES 100

#define RANDOM_NUMBERS 1000
#define RANDOM_SIZE 1
#define RANDOM_RANDOM 299
#define RANDOM_TIMES 100

void startTime(struct timeval*);
long stopTime(struct timeval);
void testAll(int);
void allocate(int, int, int);
void allocateSmall(int);
void allocateBig(int);
void allocateRandom(int);
void prettyAllocate(int, int, int, int, int);

#define SEED 15486433

int main(){
testAll(0);
exit(0);
}



void testAll(int strat){
 printf("{'strategy':%d,'tests':[",strat);
 allocateSmall(strat);
 printf(",");
 allocateBig(strat);
 printf(",");
 allocateRandom(strat);
 printf("]}");
 fflush(stdout);
}

void prettyAllocate(int times, int chunks, int strat, int lower, int random){
int i;
printf("{'lowerBound':%d,'upperBound':%d,'data':[",lower,lower+random);
fflush(stdout);
for(i = 1; i<= times; ++i){
	if(i != 1) printf(",");
	fflush(stdout);
	allocate(lower,random, chunks);
}
printf("]}");
fflush(stdout);
}

void allocateSmall(int strat){
	prettyAllocate(SMALL_TIMES, SMALL_NUMBERS, strat, SMALL_SIZE, SMALL_RANDOM);
}

void allocateBig(int strat){
	prettyAllocate(BIG_TIMES, BIG_NUMBERS, strat, BIG_SIZE, BIG_RANDOM);
}

void allocateRandom(int strat){
	prettyAllocate(RANDOM_TIMES, RANDOM_NUMBERS, strat, RANDOM_SIZE, RANDOM_RANDOM);
}

void allocate(int minSize, int randomSize, int cc){

if(fork() == 0){
void * ptrs[cc];
int i;
struct timeval tv;
long timePassed;
int seed = SEED;
void *startHeap = sbrk(0);
void *endHeap = startHeap;
int chunkSize;
startTime(&tv);

for(i = 0; i < cc; ++i){
 chunkSize = minSize + rand_r(&seed)%randomSize;
 ptrs[i] = malloc(chunkSize);
}

 if(sbrk(0) > endHeap)
	endHeap = sbrk(0);

for(i = 0; i < cc; ++i)
 free(ptrs[i]);

for(i = 0; i < cc; ++i){
 chunkSize = minSize + rand_r(&seed)%randomSize;
 ptrs[i] = malloc(chunkSize);
}

 if(sbrk(0) > endHeap)
	endHeap = sbrk(0);
for(i = 0; i < cc; ++i){
 chunkSize = minSize + rand_r(&seed)%randomSize;
 ptrs[i] = realloc(ptrs[i], chunkSize);
}
 if(sbrk(0) > endHeap)
	endHeap = sbrk(0);

for(i = 0; i < cc; ++i)
 free(ptrs[i]);


timePassed = stopTime(tv);
startHeap = endHeap - startHeap;

printf("{'memory':%d,'time':%d}",(unsigned) startHeap ,timePassed);

fflush(stdout);

exit(0);
}

wait(&status);
}


void startTime(struct timeval* tv){

 gettimeofday(tv,NULL);
}


long stopTime(struct timeval oldTv){
int sec, usec;
long time;
struct timeval currTv;
gettimeofday(&currTv, NULL);
sec = currTv.tv_sec - oldTv.tv_sec;
usec = currTv.tv_usec - oldTv.tv_usec;
time = sec*1000000+usec;
return time;

}
