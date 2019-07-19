#ifndef RWLOCK_H
#define RWLOCK_H

#include "pthread.h"

typedef struct __rwlock_t {
	pthread_mutex_t lock;
	pthread_cond_t cv;
	int num_readers;
	int writing; // 0 or 1
} rwlock_t;

void rwlock_init(rwlock_t *);

void rwlock_acquire_readlock(rwlock_t*);

void rwlock_release_readlock(rwlock_t *);

void rwlock_acquire_writelock(rwlock_t *);

void rwlock_release_writelock(rwlock_t *);

// destroys locks and conditional variables and frees memory
void rwlock_destroy(rwlock_t *);

#endif


