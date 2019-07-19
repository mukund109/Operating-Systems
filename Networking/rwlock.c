#include "rwlock.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>

void rwlock_init(rwlock_t *rw) {
	pthread_mutex_init(&rw->lock, NULL);
	pthread_cond_init(&rw->cv, NULL);
	rw->num_readers = 0;
	rw->writing = 0;
}

void rwlock_acquire_readlock(rwlock_t *rw) {
	pthread_mutex_lock(&rw->lock);
	while(rw->writing==1)
		pthread_cond_wait(&rw->cv, &rw->lock);
	rw->num_readers++;
	pthread_mutex_unlock(&rw->lock);
		
}

void rwlock_release_readlock(rwlock_t *rw) {
	pthread_mutex_lock(&rw->lock);
	rw->num_readers--;
	pthread_cond_broadcast(&rw->cv);
	pthread_mutex_unlock(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
	pthread_mutex_lock(&rw->lock);
	while(rw->writing ==1)
		pthread_cond_wait(&rw->cv, &rw->lock);
	rw->writing = 1;
	while(rw->num_readers>0)
		pthread_cond_wait(&rw->cv, &rw->lock);
	pthread_mutex_unlock(&rw->lock);
}

void rwlock_release_writelock(rwlock_t *rw) {
	pthread_mutex_lock(&rw->lock);
	rw->writing = 0;
	pthread_cond_broadcast(&rw->cv);
	pthread_mutex_unlock(&rw->lock);
}

void rwlock_destroy(rwlock_t *rw){
	while(pthread_mutex_destroy(&rw->lock)!=0){};
	while(pthread_cond_destroy(&rw->cv)!=0){};
	free(rw);
}
