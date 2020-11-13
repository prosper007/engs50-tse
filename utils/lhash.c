/* lhash.c --- 
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Thu Nov 12 20:48:00 2020 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdint.h>
#include <stdbool.h>
#include "hashtable.h"
#include "lhash.h"
#include <pthread.h>

typedef struct lhashtable {
	hashtable_t* hashtable;
	pthread_mutex_t* mutex;
} lhashtable_i

/* hopen -- opens a hash table with initial size hsize */
lhashtable_t *lhopen(uint32_t lhsize){
	lhashtable_i* lhashtable = (lhashtable_i*)malloc(sizeof(lhashtable_i));
	
	hashtable_t* hashtable = hopen(lhsize);
	lhashtable->hashtable = hashtable;

	pthread_mutex_t* mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	lhashtable->mutex = mutex;
	return (lhashtable_t*) lhashtable;
}

/* hclose -- closes a hash table */
void lhclose(lhashtable_t *lhtp){
	lhashtable_i* lhti = (lhashtable_i*) lhtp;
	pthread_mutex_lock(lhti->mutex);
	hclose(lhti->hashtable);
	pthread_mutex_unlock(lhti->mutex);
	pthread_mutex_destroy(lhti->mutex);
	free(lhti->mutex);
	free(lhti);
	lhtp=NULL;
}

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen){
	lhashtable_t *lhti = (lhashtable_i*) lhtp;
	pthread_mutex_lock(lhti->mutex);
	int32_t result = hput(lhti->hashtable, ep, key, keylen);
	pthread_mutex_unlock(lhti->mutex);
	return result;
}

/* happly -- applies a function to every entry in hash table */
void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep)){
	lhashtable_i *lhti = (lhashtable_i*) lhtp;
	pthread_mutex_lock(lhti->mutex);
	happly(lhti->hashtable, fn);
	pthread_mutex_unlock(lhti->mutex);
}

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *lhsearch(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
							 int32_t keylen){
	lhashtable_i* lhti = (lhahstable_i*) lhtp;
	pthread_mutex_lock(lhti->mutex);
	void* result = hsearch(lhti->hashtable, searchfn, key, keylen);
	pthread_mutex_unlock(lhti->mutex);
	return result;
}

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *lhremove(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
							 int32_t keylen){
	lhashtable_i* lhti = (lhashtable_i*) lhtp;
	pthread_mutex_lock(lhti->mutex);
	void* result = hremove(lhti->hashtable, searchfn, key, keylen);
	pthread_mutex_unlock(lhti->mutex);
	return result;
}

