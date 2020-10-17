/* test2.c --- tests hremove and happly
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Sun Oct 12 20:44:41 2020 (-0400)
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
		printf("Error: malloc failed allocating car\n");
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

void add_age(void* person) {
	person_t* pp = (person_t*) person;
	pp->age += 1;
}

int main(void) {
	//Open hashtable and put in person
	printf("Open hashtable h1 and put person 'Lala' inside w/ age 23...\n");
	hashtable_t* h1 = hopen(10);
	
	person_t* p1 = make_person("Lala", 23, 16.8);
	hput(h1, p1, p1->name, 5);
	
	//Test happly 
	printf("Apply add_age (+1 to age) to one person...\n");
	happly(h1, add_age);
	printf("Search for Lala...\n");
	
	person_t* pp = hsearch(h1, name_search, "Lala", 5);
	if(pp != p1) {
		printf("Test failed! hsearch found wrong person\n");
		exit(EXIT_FAILURE);
	}
	printf("Found name: %s\n", pp->name);
	printf("Checking Lala's age...\n");
	
	if(p1->age != 24) {
		printf("Test failed! happly did not add 1 to year\n");
		exit(EXIT_FAILURE);
	}
	printf("Lala's age is correctly %d\n", p1->age);
	
	//Add another person to test happly on two people 
	printf("Now add another person, 'Obis', with age 15\n");
	person_t* p2 = make_person("Obis", 15, 10);
	hput(h1, p2, p2->name, 5);
	printf("Apply add_age (+1 to age) to two people...\n");
	happly(h1, add_age);
	printf("Search for Obis...\n");
	
	pp = hsearch(h1, name_search, "Obis", 5);
	if(pp != p2) {
		printf("Test failed! hsearch found wrong person\n");
		exit(EXIT_FAILURE);
	}
	printf("Found name: %s\n", pp->name);
	printf("Checking Lala's and Obis's age...\n");
	
	if(p2->age != 16 || p1->age != 25) {
		printf("Test failed! happly did not add 1 to year\n");
		exit(EXIT_FAILURE);
	}
	printf("Lala's age is correctly %d\n", p1->age);
	printf("Obis's age is correctly %d\n", p2->age);
	
	//Test hremove. First, add a third person 
	printf("Now test hremove...\n");
	printf("Add one more person, 'Bobe', for total of three people in h1...\n");
	person_t* p3 = make_person("Bobe", 20, 10);
	hput(h1, p3, p3->name, 5);
	
	//remove from front of queue in hashtable
	printf("Removing from beginning of queue in hashtable...\n");
	pp = (person_t*) hremove(h1, name_search, "Lala", 5);
	if(pp != p1) {
		printf("Test failed! hremove did not remove from front of queue\n");
		exit(EXIT_FAILURE);
	}
	printf("removed name: %s\n", pp->name);
	
	//put back p1
	printf("Putting back Lala...\n");
	hput(h1, p1, p1->name, 5);
	
	//remove from middle of queue in hashtable
	printf("Removing from middle of queue in hashtable...\n");
	pp = (person_t*) hremove(h1, name_search, "Obis", 5);
	if(pp != p2) {
		printf("Test failed! hremove did not remove from middle of queue\n");
		exit(EXIT_FAILURE);
	}
	printf("removed name: %s\n", pp->name);
	
	//put back p2
	printf("Putting back Obis...\n");
	hput(h1, p2, p2->name, 5);
	
	//remove from end of queue in hashtable
	printf("Removing from end of queue in hashtable...\n");
	pp = (person_t*) hremove(h1, name_search, "Bobe", 5);
	if(pp != p3) {
		printf("Test failed! hremove did not remove from end of queue\n");
		exit(EXIT_FAILURE);
	}
	printf("removed name: %s\n", pp->name);
	
	//put back p3
	printf("Putting back Bobe...\n");
	hput(h1, p3, p3->name, 5);
	
	// remove non-existing element. should return null
	pp = hremove(h1, name_search, "Rose", 5);
	if(pp != NULL) {
		printf("Test failed! hremove did not return NULL for non-existing element\n");
		exit(EXIT_FAILURE);
	}
	
	//free everything
	hclose(h1);
	free(p1);
	free(p2);
	free(p3);
	
	printf("Tests Passed! happly and hremove were tested\n");
	
	/*//remove from front of queue
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
	*/
}
