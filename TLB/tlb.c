#define _GNU_SOURCE
#include <stdio.h>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[]){

	//pinning process to a single CPU
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(1, &set);

	sched_setaffinity(0, sizeof(set), &set);
	
	long int PAGESIZE = sysconf(_SC_PAGESIZE); // = 4096 bytes
	
	int num_pages = 4;
	int num_samples = 10000;
	
	// parse args

	if(argc==2)
		num_pages = atoi(argv[1]);
	else if (argc==3){
		num_pages = atoi(argv[1]);
		num_samples = atoi(argv[2]);
	}
	
	// allocate array
	int* a = (int*)calloc(10000000, sizeof(int));
	// array should be bigger than what can fit into num_pages
	assert(num_pages*PAGESIZE/sizeof(int)<10000000);

	struct timeval  start, end;
	gettimeofday(&start, NULL);
	int skip = PAGESIZE/sizeof(int);
	for(int n=0; n<num_samples/num_pages; n++){
		for(int i = 0; i<num_pages*skip; i += skip){
			a[i] +=1;
		}
	}
	gettimeofday(&end, NULL);
	long elapsed_time = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);
	a[32];
	printf("%ld", elapsed_time);
	return 0;
}