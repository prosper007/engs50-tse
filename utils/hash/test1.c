/* test1.c --- file to test implementation of queue interface
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Sat Oct 10 17:52:18 2020 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hash.h"                                                                                                                                                                                          
#include "../queue/queue.h" 

/* the representation of a person */
typedef struct person {
	char name[10];
	double height;
	int age;
} person_t;

person_t* make_person(char* namep, int age, double height) {
	person_t* pp;
	if(!(pp=(person_t*)malloc(sizeof(person_t)))) {
		printf("Error: malloc failed allocating car");
		return NULL;
	}
	strcpy(pp->name,namep);
	pp->age = age;
	pp->height = height;
	return pp;
}	

bool name_search(void* personp, const void* namep) {                                                                                                                                                       
  person_t* pp = (person_t*) personp;                                                                                                                                                                
  if(strcmp(pp->name, (char*)namep) == 0){                                                                                                                                                           
    return true;                                                                                                                                                                               
  } else {                                                                                                                                                                                           
    return false;                                                                                                                                                                              
  }                                                                                                                                                                                                  
}   

int main(void) {
	/*test hopen */
  printf("Testing hopen()...\n");
  hashtable_t* h1 = hopen(10);
  if(h1 == NULL) {
    printf("Test failed! qopen returned a NULL hashtable.\n");
    exit(EXIT_FAILURE);
  }

  // put two persons on queue
  printf("Testing hput...\n");
  person_t *p1 = make_person("Mike", 42, 6.13);
  char* key1 = p1->name;
  // int keylen1 = sizeof(key1);
  int32_t put_p1 = hput(h1, p1, key1, 5);
  if(put_p1 != 0){
    printf("Test failed! error putting to queue.\n");
    exit(EXIT_FAILURE);
  }
  
  person_t *p2 = make_person("Rafe", 36, 4.02);
  char* key2 = p2->name;
  //int keylen2 = sizeof(key2);
  int32_t put_p2 = hput(h1, p2, key2, 5);
  if(put_p2 != 0){
    printf("Test failed! error putting to queue.\n");
    exit(EXIT_FAILURE);
  }

	
  // search the persons to see if they are in the hashtable
  person_t* pp = hsearch(h1, name_search, key1, 5);
  if(pp != p1) {
    printf("Test failed hsearch did not find right person");
    exit(EXIT_FAILURE);
  }
  printf("found name: %s\n", pp->name);
  
  pp = hsearch(h1, name_search, key2, 5);                                                                                                                                                
  if(pp != p2) {                                                                                                                                                                                           
    printf("Test failed hsearch did not find right person");                                                                                                                                               
    exit(EXIT_FAILURE);                                                                                                                                                                                    
  }
  printf("found name: %s\n", pp->name); 
  
  
  
  
  
  

  
  
  
  
  
  
	/*
	// get from now empty queue. should return NULL
	pp = (person_t*) qget(q1);
	if(pp != NULL) {
		printf("Test failed! did not return NULL from empty queue.\n");
		exit(EXIT_FAILURE);
	}

	// put back person
	put_p1 = qput(q1, p1);

	// create another queue
	queue_t* q2 = qopen();
	if(q2 == NULL) {
		printf("Test failed! qopen returned a NULL queue.\n");
		exit(EXIT_FAILURE);
	}

	// put person2 in queue2
	int32_t put_p2_q2 = qput(q2, p2);
	if(put_p2_q2 != 0){
		printf("Test failed! error putting to queue.\n");
		exit(EXIT_FAILURE);
	}

	// get back person2 and confirm it's in right queue
	pp = (person_t*) qget(q2);
	if(pp != p2) {
		printf("Test failed! qget did not return correct object from queue 2.\n");
		exit(EXIT_FAILURE);
	}
	printf("p2 in q2 name: %s\n", pp->name); 
		
	// check that queue 1 is intact
	pp = (person_t*) qget(q1);
	if(pp != p1) {
		printf("Test failed! qget did not return correct object from queue 2.\n");
		exit(EXIT_FAILURE);
	}
	printf("p1 in q1 name: %s\n", pp-> name);
	
	// get from now empty queue2. should return NULL
	pp = (person_t*) qget(q2);
	if(pp != NULL) {
		printf("Test failed! did not return NULL from empty queue 2.\n");
		exit(EXIT_FAILURE);
	}

	// get from now empty queue1. should return NULL
	pp = (person_t*) qget(q1);
	if(pp != NULL) {
		printf("Test failed! did not return NULL from empty queue 1.\n");
		exit(EXIT_FAILURE);
	}

	queue_t* q3 = qopen();
	person_t *p3 = make_person("Telitm3", 46, 4.52);
	int32_t put_p3 = qput(q3, p3);
	if(put_p3 != 0){
		printf("Test failed! error putting to queue.\n");
		exit(EXIT_FAILURE);
	}

	qclose(q2);

	pp = (person_t*) qget(q3);
	if(pp != p3) {
		printf("Test failed! qget did not return correct object from queue 3.\n");
		exit(EXIT_FAILURE);
	}
	printf("p3 in q3 name: %s\n", pp-> name);
	*/
	hclose(h1);
	printf("Tests passed! hopen, hclose, hput, and hsearch were tested\n");
	free(p1);
	free(p2);
	exit(EXIT_SUCCESS);
	
}

