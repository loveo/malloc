#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


void start_time(struct timeval*);
long stop_time(struct timeval);

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

void start_time(struct timeval*);
long stop_time(struct timeval);
void test_all(int);
void allocate(int, int, int);
void allocate_small(int);
void allocate_big(int);
void allocate_random(int);
void pretty_allocate(int, int, int, int, int);

#define SEED 15486433

int main(){
test_all(0);
exit(0);
}



void test_all(int strat){
 printf("{'strategy':%d,'tests':[",strat);
 allocate_small(strat);
 printf(",");
 allocate_big(strat);
 printf(",");
 allocate_random(strat);
 printf("]}");
 fflush(stdout);
}

void pretty_allocate(int times, int chunks, int strat, int lower, int random){
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

void allocate_small(int strat){
	pretty_allocate(SMALL_TIMES, SMALL_NUMBERS, strat, SMALL_SIZE, SMALL_RANDOM);
}

void allocate_big(int strat){
	pretty_allocate(BIG_TIMES, BIG_NUMBERS, strat, BIG_SIZE, BIG_RANDOM);
}

void allocate_random(int strat){
	pretty_allocate(RANDOM_TIMES, RANDOM_NUMBERS, strat, RANDOM_SIZE, RANDOM_RANDOM);
}

void allocate(int min_size, int random_size, int cc){

if(fork() == 0){
void * ptrs[cc];
int i;
struct timeval tv;
long time_passed;
int seed = SEED;
void *start_heap = sbrk(0);
void *end_heap = start_heap;
int chunk_size;
start_time(&tv);

for(i = 0; i < cc; ++i){
 chunk_size = min_size + rand_r(&seed)%random_size;
 ptrs[i] = malloc(chunk_size);
}

 if(sbrk(0) > end_heap)
	end_heap = sbrk(0);

for(i = 0; i < cc; ++i)
 free(ptrs[i]);

for(i = 0; i < cc; ++i){
 chunk_size = min_size + rand_r(&seed)%random_size;
 ptrs[i] = malloc(chunk_size);
}

 if(sbrk(0) > end_heap)
	end_heap = sbrk(0);
for(i = 0; i < cc; ++i){
 chunk_size = min_size + rand_r(&seed)%random_size;
 ptrs[i] = realloc(ptrs[i], chunk_size);
}
 if(sbrk(0) > end_heap)
	end_heap = sbrk(0);

for(i = 0; i < cc; ++i)
 free(ptrs[i]);


time_passed = stop_time(tv);
start_heap = end_heap - start_heap;

printf("{'memory':%d,'time':%d}",(unsigned) start_heap ,time_passed);

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
