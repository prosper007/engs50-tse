/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "../queue/queue.h"

/* 
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
#define get16bits(d) (*((const uint16_t *) (d)))

static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL)
		return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}

typedef struct hashtable {
	uint32_t hsize;
	queue_t **list_of_queues;
	struct hashtable *next_ht;
} hashtable_i;

//static hashtable_i *hashtable_interface=NULL;

//define function to find a specific hashtable in an interface of hashtables
/*hashtable_i* find_ht(hashtable_t *htp) {
  hashtable_i *htfind;
  for (htfind=hashtable_interface; htfind!= NULL; htfind=hashtable_interface->next_ht) {
    if(htfind==htp) {
      return htfind;
    }   
  }
	printf("Hashtable not found in interface\n");
  return NULL; 
}*/

hashtable_t *hopen(uint32_t hsize) {
	hashtable_i* ht = (hashtable_i*)malloc(sizeof(hashtable_i));
	ht->hsize = hsize;
	ht->list_of_queues = (queue_t*)malloc(sizeof(queue_t*) * hsize);

	for(uint32_t i = 0; i < hsize; i++) {
		ht->list_of_queues[i] = qopen();
	}
	// add new hashtable to list of hashtables in interface
	//ht->next_ht = hashtable_interface;
	//hashtable_interface = ht;
	return (hashtable_t*) ht;
}

void hclose(hashtable_t *htp) {
	//hashtable_i* ht = find_ht(htp);
	hashtable_i* ht = (hashtable_i *) htp;
	if(ht == NULL) {
		return;
	}
	//close all queues
	for(uint32_t i = 0; i < ht->hsize; i++) {
		qclose(ht->list_of_queues[i]);
	}
	
	free(ht->list_of_queues);
	
	// remove hashtable from hashtable_interface
	/*hashtable_i* prev = NULL;
	for(hashtable_i* i = hashtable_interface; i!=NULL; i=i->next_ht) {
		if(i == ht) {
			if(prev == NULL){
				hashtable_interface = i->next_ht;
				break;
			}
			prev->next_ht = i->next_ht;
			break;
		}
		prev = i;
	}*/
	free(ht);
	ht=NULL;
	htp=NULL;
}


int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen) {
	//hashtable_i* ht = find_ht(htp);
	hashtable_i* ht = (hashtable_i *) htp;
	if(ht == NULL) {
		return 1;
	}
	uint32_t ht_idx = SuperFastHash(key, keylen, ht->hsize);
	return qput(ht->list_of_queues[ht_idx], ep);
}

void *hsearch(hashtable_t *htp, bool (*searchfn)(void* elementp, const void* searchkeyp), const char *key, int32_t keylen) {
	//hashtable_i* ht = find_ht(htp);
	hashtable_i* ht = (hashtable_i *) htp;
	if(ht == NULL) {
		return NULL;
	}
	uint32_t ht_idx = SuperFastHash(key, keylen, ht->hsize);
	return qsearch(ht->list_of_queues[ht_idx], searchfn, key);
}

void *hremove(hashtable_t *htp, bool (*searchfn)(void* elementp, const void* searchkeyp), const char *key, int32_t keylen) {
	//hashtable_i* ht = find_ht(htp);
	hashtable_i* ht = (hashtable_i *) htp;
	if(ht == NULL) {
		return NULL;
	}
	uint32_t ht_idx = SuperFastHash(key, keylen, ht->hsize);
	return qremove(ht->list_of_queues[ht_idx], searchfn, key);
}

void happly(hashtable_t *htp, void (*fn)(void* ep)) {
	//hashtable_i* ht = find_ht(htp);
	hashtable_i* ht = (hashtable_i *) htp;
	if(ht == NULL) {
		return;
	}

	for(uint32_t i = 0; i < ht->hsize; i++) {
		qapply(ht->list_of_queues[i], fn);
	}
}

