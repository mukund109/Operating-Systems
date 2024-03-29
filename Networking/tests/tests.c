#include "rwlock.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>

int loops;
int value = 0;
int read_write_ratio = 1;

rwlock_t lock;

void workload(unsigned int usec){
	struct timeval  start, end;
	gettimeofday(&start, NULL);
	long elapsed_time, i = 0;
	while(1){
		if(i%1000 == 0){
			i=0;
			gettimeofday(&end, NULL);
			elapsed_time = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);
			if(elapsed_time>usec)
				return;
		}
		i++;
	}
}


void *thread(void *arg){
	// to calculate wait time
	struct timeval  start, end;
	long elapsed_time, elapsed_time2;

	for(int i= 0; i<loops; i++){
		if(rand()%(read_write_ratio+1) == 0){

			gettimeofday(&start, NULL);
			rwlock_acquire_writelock(&lock);
			gettimeofday(&end, NULL);
			elapsed_time = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);

			workload(rand()%100000);
			value++;
			printf("write %d, wait time %ld \n", value, elapsed_time);
			rwlock_release_writelock(&lock);
		}
		else{
			gettimeofday(&start, NULL);
			rwlock_acquire_readlock(&lock);
			gettimeofday(&end, NULL);
			elapsed_time = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);

			gettimeofday(&start, NULL);
			workload(rand()%100000);
			gettimeofday(&end, NULL);
			elapsed_time2 = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);

			printf("read %d, wait time %ld, read time %ld \n", value, elapsed_time, elapsed_time2);
			rwlock_release_readlock(&lock);
		}

	}
	return NULL;
}


int main(int argc, char *argv[]) {
    
    assert(argc == 4);
    int num_threads = atoi(argv[1]);
    read_write_ratio = atoi(argv[2]);
    loops = atoi(argv[3]);

    pthread_t p[num_threads];

    rwlock_init(&lock);

    printf("begin\n");
	struct timeval  start, end;
	long elapsed_time;
	gettimeofday(&start, NULL);

    int i;
    for (i = 0; i < num_threads; i++)
		pthread_create(&p[i], NULL, thread, NULL);
	for (i = 0; i < num_threads; i++)
		pthread_join(p[i], NULL);

	gettimeofday(&end, NULL);
	elapsed_time = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);
    printf("end: value %d, elapsed_time: %ld \n", value, elapsed_time);

    return 0;
}
