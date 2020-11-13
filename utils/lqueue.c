/* lqueue.c --- 
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Thu Nov 12 19:28:11 2020 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <pthread.h>
#include "lqueue.h"

/* the queue representation is hidden from users of the module */
typedef struct lqueue {
  queue_t* queue;
  pthread_mutex_t* mutex;
} lqueue_i;

/* create an empty queue */
lqueue_t* lqopen(void){
	lqueue_i* lqueue = (lqueue_i*)malloc(sizeof(lqueue_i));

	queue_t* queue = qopen();
	lqueue->queue = queue;

	pthread_mutex_t* mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	lqueue->mutex = mutex;
	return (lqueue_t*) lqueue;
}

/* deallocate a queue, frees everything in it */
void lqclose(lqueue_t *lqp){
	lqueue_i* lqi = (lqueue_i*) lqp;
	pthread_mutex_lock(lqi->mutex);
	qclose(lqi->queue);
	pthread_mutex_unlock(lqi->mutex);
	pthread_mutex_destroy(lqi->mutex);
	free(lqi->mutex);
	free(lqi);
	lqp=NULL;
}

/* put element at the end of the queue
 * returns 0 is successful; nonzero otherwise 
 */
int32_t lqput(lqueue_t *lqp, void *elementp) {
	lqueue_i* lqi = (lqueue_i*) lqp;
	pthread_mutex_lock(lqi->mutex);
	int32_t result = qput(lqi->queue, elementp);
	pthread_mutex_unlock(lqi->mutex);
	return result;
}

/* get the first first element from queue, removing it from the queue */
void* lqget(lqueue_t *lqp){
	lqueue_i* lqi = (lqueue_i*) lqp;
	pthread_mutex_lock(lqi->mutex);
	void* result = qget(lqi->queue);
	pthread_mutex_unlock(lqi->mutex);
	return result;
}

/* apply a function to every element of the queue */
void lqapply(lqueue_t *lqp, void (*fn)(void* elementp)){
	lqueue_i* lqi = (lqueue_i*) lqp;
	pthread_mutex_lock(lqi->mutex);
	qapply(lqi->queue, fn);
	pthread_mutex_unlock(lqi->mutex);
}

/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* lqsearch(lqueue_t *lqp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							 const void* skeyp){
	lqueue_i* lqi = (lqueue_i*) lqp;
	pthread_mutex_lock(lqi->mutex);
	void* result = qsearch(lqi->queue, searchfn, skeyp);
	pthread_mutex_unlock(lqi->mutex);
	return result;
}

