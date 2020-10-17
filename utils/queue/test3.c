/* test3.c --- test qsearch and qremove
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Mon Oct 12 09:39:24 2020 (-0400)
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
#include "queue.h"

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
	queue_t* q1 = qopen();
	person_t* p1 = make_person("Lala", 23, 16.8);
	person_t* p2 = make_person("Obi", 27, 4.20);
	person_t* p3 = make_person("Rose", 25, 5.13);

	qput(q1, p1);
	qput(q1, p2);
	qput(q1, p3);

	person_t* pp = qsearch(q1, name_search, "Obi");
	if(pp != p2) {
		printf("Test failed! qsearch found wrong person\n");
		exit(EXIT_FAILURE);
	}
	printf("found name: %s\n", pp->name);

	// ensure queue is intact
	if(qget(q1) != p1){
		printf("Test failed! queue was modified after qsearch\n");
		exit(EXIT_FAILURE);
	}
	if(qget(q1) != p2){
		printf("Test failed! queue was modified after qsearch\n");
		exit(EXIT_FAILURE);
	}
	if(qget(q1) != p3){
		printf("Test failed! queue was modified after qsearch\n");
		exit(EXIT_FAILURE);
	}

	//put persons back
	qput(q1, p1);
	qput(q1, p2);
	qput(q1, p3);

	pp = qsearch(q1, name_search, "Lala");
	if(pp != p1) {
		printf("Test failed! qsearch found wrong person\n");
		exit(EXIT_FAILURE);
	}
	printf("found name: %s\n", pp->name);

	// test remove
	//remove from front of queue
	pp = (person_t*) qremove(q1, name_search, "Lala");
	if(pp != p1) {
		printf("Test failed! qremove did not remove from front of queue\n");
		exit(EXIT_FAILURE);
	}
	printf("removed name: %s\n", pp->name);

	//ensure rest of queue intact
	if(qget(q1) != p2){
		printf("Test failed! queue order is incorrect after qremove\n");
		exit(EXIT_FAILURE);
	}
	if(qget(q1) != p3){
		printf("Test failed! queue order is incorrect after qremove\n");
		exit(EXIT_FAILURE);
	}
	//put back removed persons
	qput(q1, p1);
	qput(q1, p2);
	qput(q1, p3);

	// remove from middle of queue
 pp =qremove(q1, name_search, "Obi");
	if(pp != p2) {
		printf("Test failed! qremove did not remove from middle of queue\n");
		exit(EXIT_FAILURE);
	}
	printf("removed name: %s\n", pp->name);

	//ensure rest of queue is intact
	if(qget(q1) != p1){
		printf("Test failed! queue order is incorrect after qremove from middle\n");
		exit(EXIT_FAILURE);
	}
	if(qget(q1) != p3){
		printf("Test failed! queue order is incorrect after qremove from middle\n");
		exit(EXIT_FAILURE);
	}
	//put back removed persons
	qput(q1, p1);
	qput(q1, p2);
	qput(q1, p3);

	// remove from end of queue
	pp =qremove(q1, name_search, "Rose");
	if(pp != p3) {
		printf("Test failed! qremove did not remove from end of queue\n");
		exit(EXIT_FAILURE);
	}
	printf("removed name: %s\n", pp->name);

	// remove non-existing element. should return null
	pp=qremove(q1, name_search, "Rose");
	if(pp != NULL) {
		printf("Test failed! qremove did not return NULL for non-existing element\n");
		exit(EXIT_FAILURE);
	}

	//ensure rest of queue is intact
	if(qget(q1) != p1){
		printf("Test failed! queue order is incorrect after qremove from end\n");
		exit(EXIT_FAILURE);
	}
	if(qget(q1) != p2){
		printf("Test failed! queue order is incorrect after qremove from end\n");
		exit(EXIT_FAILURE);
	}

	//free everything
	qclose(q1);
	free(p1);
	free(p2);
	free(p3);

	printf("Tests Passed! qsearch and qremove were tested\n");
	
}


