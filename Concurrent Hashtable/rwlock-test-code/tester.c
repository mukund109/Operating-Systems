#include "rwlock.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

rwlock_t rwlock;

void *Reader(void* arg)
{
  int threadNUmber = *((int *)arg);
  
  // Occupying the Lock
  rwlock_acquire_readlock(&rwlock);
  printf("Reader: %d has acquired the lock\n", threadNUmber);
  
  usleep(2000000);
  
  // Releasing the Lock
  rwlock_release_readlock(&rwlock);
  printf("Reader: %d has released the lock\n",threadNUmber);
}

void *Writer(void* arg)
{
  int threadNUmber = *((int *)arg);
  
  // Occupying the Lock
  rwlock_acquire_writelock(&rwlock);
  printf("Writer: %d has acquired the lock\n",threadNUmber);
  
  usleep(2000000);
  
  // Releasing the Lock
  rwlock_release_writelock(&rwlock);
  printf("Writer: %d has released the lock\n",threadNUmber);
}

int main(int argc, char *argv[])
{
  
  int *threadNUmber;
  pthread_t *threads;

  int read_num_threads;
  int write_num_threads;
  
  if (argc < 3) {
    printf("./exe #num_readers #num_writers\n");
    exit(1);
  }
  else {
    read_num_threads = atoi(argv[1]);
    write_num_threads = atoi(argv[2]);
  }
  
  rwlock_init(&rwlock);
  
  int num_threads = 2*read_num_threads + write_num_threads;
  
  threads = (pthread_t*) malloc(num_threads * (sizeof(pthread_t)));
  
  int count = 0;
  for(int i=0;i<read_num_threads;i++)
    {
      
      int val = i;
      int ret = pthread_create(threads+count,NULL,Reader,(void*) &val);
      if(ret)
	{
	  fprintf(stderr,"Error - pthread_create() return code: %d\n",ret);
	  exit(EXIT_FAILURE);
	}
      count++;
      usleep(1000);
    }
  
  for(int i=0;i<write_num_threads;i++)
    {
      int val = i;
      int ret = pthread_create(threads+count,NULL,Writer,(void*) &val);
      if(ret)
	{
	  fprintf(stderr,"Error - pthread_create() return code: %d\n",ret);
	  exit(EXIT_FAILURE);
	}
      count++;
      usleep(1000);
    }
  
  for(int i=0;i<read_num_threads;i++)
    {
      
      int val = read_num_threads + i;
      int ret = pthread_create(threads+count,NULL,Reader,(void*) &val);
      if(ret)
	{
	  fprintf(stderr,"Error - pthread_create() return code: %d\n",ret);
	  exit(EXIT_FAILURE);
	}
      count++;
      usleep(1000);
    }
  
  
  for(int i=0;i<num_threads; i++)
    pthread_join(threads[i],NULL);
  
  exit(1);
}
