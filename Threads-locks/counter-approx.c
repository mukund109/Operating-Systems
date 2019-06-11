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

void global_count_increment(counter *c, int size){
	pthread_mutex_lock(c->lock);
	c->count += size;
	pthread_mutex_unlock(c->lock);
}

counter global_counter;

void* run(void * args){
	int local_count = 0;
	for(int i = 0; i<1000000; i++){
		local_count++;
		if (local_count%1024 == 0)
			global_count_increment(&global_counter, 1024);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	struct timeval start, end;
	
	//initialize counter
	counter_init(&global_counter);
	
	assert(argc==2);

	int num_threads = atoi(argv[1]);
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
	printf("%ld", elapsed_time);
	
	return 0;
}

