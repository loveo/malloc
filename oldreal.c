#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


void startTime(struct timeval*);
long stopTime(struct timeval);

int status;

#define SMALL_NUMBERS 100
#define SMALL_SIZE 19
#define SMALL_TIMES 100

#define BIG_NUMBERS 50
#define BIG_SIZE 84359
#define BIG_TIMES 100

#define SMALLEST_RANDOM 1
#define BIGGEST_RANDOM 50000
#define RANDOM_NUMBERS 250
#define RANDOM_TIMES 100

void startTime(struct timeval*);
long stopTime(struct timeval);
void testStrategy(void);
void testAll(int);
void allocate(int, int, int, int, char*);
void allocateSmall(int);
void allocateBig(int);
void allocateRandom(int, int);

int main(){

testStrategy();

exit(0);
}



void testStrategy(){
int i;
for(i = 1; i <= 1; ++i){
 testAll(i);
}
}

void testAll(int strat){
 allocateSmall(strat);
 allocateBig(strat);
 allocateRandom(strat, 1717523);
}


void allocateSmall(int strat){
int i;
printf("Allocating %d * %d chunks %d times\n",SMALL_SIZE, SMALL_NUMBERS, SMALL_TIMES);
fflush(stdout);
for(i = 1; i<= SMALL_TIMES; ++i)
	allocate(SMALL_SIZE, SMALL_NUMBERS, i, strat, "Small alloc test");

}

void allocateBig(int strat){
int i;
printf("Allocating %d * %d chunks %d timesn",BIG_SIZE, BIG_NUMBERS, BIG_TIMES);
fflush(stdout);
for(i = 1; i<= BIG_TIMES; ++i)
	allocate(BIG_SIZE, BIG_NUMBERS, i, strat, "Big alloc test");

}

void allocateRandom(int strat, int seed){
int i;
int rand;

printf("Allocating %d chunks between [%d and %d] %d times with seed %d\n",RANDOM_NUMBERS,SMALLEST_RANDOM, BIGGEST_RANDOM, RANDOM_TIMES, seed);
fflush(stdout);
for(i = 1; i <= RANDOM_TIMES; ++i){
 int chunkSize;
 rand = rand_r(&seed);
 chunkSize = rand%(BIGGEST_RANDOM - SMALLEST_RANDOM);
 chunkSize += SMALLEST_RANDOM; 
 allocate(chunkSize, RANDOM_NUMBERS, i, strat, "Random alloc test");
}

}

void allocate(int cs, int cc, int iter, int strat, char *print){

if(fork() == 0){
void * ptrs[cc];
int i;
struct timeval tv;
long timePassed;
void * memStart = sbrk(0);
startTime(&tv);
for(i = 0; i < cc; ++i)
 ptrs[i] = malloc(cs);


timePassed = stopTime(tv);

printf("%s #%d (memory, time) - (%d, %d)\n",print, iter,sbrk(0)-memStart ,timePassed);

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
