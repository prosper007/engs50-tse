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
#include "lqueue.h"

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

int main(void) {
	/*test qopen */
	printf("Testing qopen()...\n");
	queue_t* q1 = qopen();
	if(q1 == NULL) {
		printf("Test failed! qopen returned a NULL queue.\n");
		exit(EXIT_FAILURE);
	}

	// put two persons on queue
	printf("Testing qput...\n");
	person_t *p1 = make_person("Lerty", 42, 6.13);
	int32_t put_p1 = qput(q1, p1);
	if(put_p1 != 0){
		printf("Test failed! error putting to queue.\n");
		exit(EXIT_FAILURE);
	}
	
	person_t *p2 = make_person("Telitm", 36, 4.02);
	int32_t put_p2 = qput(q1, p2);
	if(put_p2 != 0){
		printf("Test failed! error putting to queue.\n");
		exit(EXIT_FAILURE);
	}


	// get back persons and confirm their returned in right order
	person_t *pp = (person_t*) qget(q1);
	if(pp != p1) {
		printf("Test failed! qget did not return correct object.\n");
		exit(EXIT_FAILURE);
	}
	printf("returned p1 name: %s\n", pp->name);

	pp = (person_t*) qget(q1);
	if(pp != p2) {
		printf("Test failed! qget did not return correct object.\n");
		exit(EXIT_FAILURE);
	}
	printf("returned p2 name: %s\n", pp->name);
	
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
	
	qclose(q1);
	qclose(q3);
	printf("Tests passed! qopen, qclose, qput, and qget were tested\n");
	free(p1);
	free(p2);
	free(p3);
	exit(EXIT_SUCCESS);
	
}

