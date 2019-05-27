#include <stdio.h>
#include <sys/time.h>

int main(int argc, char *argv[]){
	struct timeval  tv1, tv2;
	int sum = 0;
	int num_calls = 1000000;
	for(int i=0; i<num_calls; i++){
		gettimeofday(&tv1, NULL);
		gettimeofday(&tv2, NULL);
		sum += (int)(tv2.tv_usec-tv1.tv_usec);
	};

	printf("elapsed time between two consecutive calls: %f \n", sum/(float)(num_calls));
	return 0;
}
