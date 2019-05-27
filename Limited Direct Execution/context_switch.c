#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


int main(int argc, char *argv[]){

  //restrict process to run on a single CPU
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(1, &set);

  sched_setaffinity(0, sizeof(set), &set);

  char buf;
  int pipe_one[2], pipe_two[2];

  //creating two pipes
  int o = pipe(pipe_one);
  int t = pipe(pipe_two);

  if((o==-1) || (t==-1)){
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  int sum = 0;
  int num_calls = 100;
  struct timeval  tv1, tv2;

  int cpid = fork();

  if (cpid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (cpid==0){
    int j = 1;
    gettimeofday(&tv1, NULL);
    for(int i =0; i<num_calls; i++){
      read(pipe_one[0], &buf, 1);
      gettimeofday(&tv2, NULL);
      sum += (int)(tv2.tv_usec-tv1.tv_usec);
      gettimeofday(&tv1, NULL);
      write(pipe_two[1], "o", 1);
      //printf("I'm child: %d\n", getpid());
    }
    printf("Child time delay, avg time for 2 context switches: %f \n", sum/(2.0*num_calls));
  }
  else{
    for(int i =0; i<num_calls; i++){
      gettimeofday(&tv1, NULL);
      write(pipe_one[1], "o", 1);
      read(pipe_two[0], &buf, 1);
      gettimeofday(&tv2, NULL);
      sum += (int)(tv2.tv_usec-tv1.tv_usec);
      //printf("I'm parent: %d\n", getpid());
    }
    write(pipe_one[1], "yolo", 1);
    printf("Parent time delay: %f \n", sum/(2.0*num_calls));
  }
	return 0;

}
