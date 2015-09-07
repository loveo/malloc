#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


void start_time(struct timeval*);
long stop_time(struct timeval);

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

void start_time(struct timeval*);
long stop_time(struct timeval);
void testStrategy(void);
void test_all(int);
void allocate(int, int, int, int, char*);
void allocate_small(int);
void allocate_big(int);
void allocate_random(int, int);

int main(){

testStrategy();

exit(0);
}



void testStrategy(){
int i;
for(i = 1; i <= 1; ++i){
 test_all(i);
}
}

void test_all(int strat){
 allocate_small(strat);
 allocate_big(strat);
 allocate_random(strat, 1717523);
}


void allocate_small(int strat){
int i;
printf("Allocating %d * %d chunks %d times\n",SMALL_SIZE, SMALL_NUMBERS, SMALL_TIMES);
fflush(stdout);
for(i = 1; i<= SMALL_TIMES; ++i)
	allocate(SMALL_SIZE, SMALL_NUMBERS, i, strat, "Small alloc test");

}

void allocate_big(int strat){
int i;
printf("Allocating %d * %d chunks %d timesn",BIG_SIZE, BIG_NUMBERS, BIG_TIMES);
fflush(stdout);
for(i = 1; i<= BIG_TIMES; ++i)
	allocate(BIG_SIZE, BIG_NUMBERS, i, strat, "Big alloc test");

}

void allocate_random(int strat, int seed){
int i;
int rand;

printf("Allocating %d chunks between [%d and %d] %d times with seed %d\n",RANDOM_NUMBERS,SMALLEST_RANDOM, BIGGEST_RANDOM, RANDOM_TIMES, seed);
fflush(stdout);
for(i = 1; i <= RANDOM_TIMES; ++i){
 int chunk_size;
 rand = rand_r(&seed);
 chunk_size = rand%(BIGGEST_RANDOM - SMALLEST_RANDOM);
 chunk_size += SMALLEST_RANDOM; 
 allocate(chunk_size, RANDOM_NUMBERS, i, strat, "Random alloc test");
}

}

void allocate(int cs, int cc, int iter, int strat, char *print){

if(fork() == 0){
void * ptrs[cc];
int i;
struct timeval tv;
long time_passed;
void * memStart = sbrk(0);
start_time(&tv);
for(i = 0; i < cc; ++i)
 ptrs[i] = malloc(cs);


time_passed = stop_time(tv);

printf("%s #%d (memory, time) - (%d, %d)\n",print, iter,sbrk(0)-memStart ,time_passed);

fflush(stdout);

exit(0);
}

wait(&status);
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
