/* test2.c --- tests qsearch, qremove, qconcat, qapply
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Sun Oct 11 20:44:41 2020 (-0400)
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
		printf("Error: malloc failed allocating car\n");
		return NULL;
	}
	strcpy(pp->name,namep);
	pp->age = age;
	pp->height = height;
	return pp;
}	

void add_age(void* person) {
	person_t* pp = (person_t*) person;
	pp->age += 1;
}

int main(void) {
	queue_t* q1 = qopen();
	person_t* p1 = make_person("Lala", 23, 16.8);
	qput(q1, p1);
	qapply(q1, add_age);
	person_t* pp = qget(q1);
	if(pp->age != 24) {
		printf("Test failed! Did not apply function to element of queue\n");
		exit(EXIT_FAILURE);
	}
	printf("age: %d\n", pp->age);

	person_t* p2 = make_person("Hmhmh", 43, 75.8);
	qput(q1, p2);
	qput(q1, p1);
	qapply(q1, add_age);

	pp = qget(q1);
	if(pp->age != 44) {
		printf("Test failed! Did not apply function to element of queue\n");
		exit(EXIT_FAILURE);
	}
	printf("age: %d\n", pp->age);

	pp=qget(q1);
	if(pp->age != 25) {
		printf("Test failed! Did not apply function to element of queue\n");
		exit(EXIT_FAILURE);
	}
	printf("age: %d\n", pp->age);

	queue_t* q2 = qopen();
	qput(q1, p1);
	qput(q2, p2);
	qconcat(q1, q2);
	pp = qget(q1);
	if(pp != p1) {
		printf("Test failed! Incorrect first element after concat\n");
		exit(EXIT_FAILURE);
	}
	printf("age: %d\n", pp->age);

	pp = qget(q1);
	if(pp != p2) {
		printf("Test failed! Incorrect second element after concat\n");
		exit(EXIT_FAILURE);
	}
	printf("age: %d\n", pp->age);

	printf("Test passed! qapply and qconcat tested\n");
	qclose(q1);
	free(p1);
	free(p2);

}
