#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <assert.h>
#include <stdlib.h>

typedef struct __counter_t {
	int count;
	pthread_mutex_t *lock;
} counter;

void counter_init(counter * c){
	c->count = 0;
	c->lock = malloc(sizeof(pthread_mutex_t));
	assert(c->lock != NULL);
	pthread_mutex_init(c->lock, NULL);
}

void count_increment(counter *c){
	pthread_mutex_lock(c->lock);
	c->count++;
	pthread_mutex_unlock(c->lock);
}

counter C;

void* run(void * args){
	for(int i = 0; i<1000000; i++){
		count_increment(&C);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	struct timeval start, end;
	
	//initialize counter
	counter_init(&C);
	
	int num_threads = 1;
	pthread_t threads[num_threads];
	
	
	//start threads
	gettimeofday(&start, NULL);
	for (int t = 0; t<num_threads; t++){
		pthread_create(threads+t, NULL, run, NULL);
	}
	for (int t = 0; t<num_threads; t++){
		pthread_join(*(threads+t), NULL);
	}
	gettimeofday(&end, NULL);
	
	
	
	long elapsed_time = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);
	printf("Time taken : %ld \n", elapsed_time);
	
	printf("Count: %d \n", C.count);
	return 0;
}

