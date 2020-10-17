/*
* test1.c 
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

car_t* make_car(char* platep, double price, int year) {
	car_t* cp;
	if(!(cp=(car_t*)malloc(sizeof(car_t)))) {
		printf("Error: malloc failed allocating car");
		return NULL;
	}
	cp->next = NULL;
	strcpy(cp->plate,platep);
	cp->price = price;
	cp->year = year;
	return cp;
}	

int test_head(car_t* expected_head, car_t* curr_head) {
	if(strcmp(expected_head->plate, curr_head->plate) != 0) {
		printf("Test failed! Wrong front\n"); // Mit thinkk about this error message
		return 1;
	}
		
	return 0;
}

int main(void) {
	
	/*test lget*/
	printf("Testing lget() on an empty list...\n");
	if(lget()!=NULL){
		printf("Test failed, lget did not return NULL\n");
		exit(EXIT_FAILURE);
	}
	printf("lget() correctly returned NULL\n");
	car_t *car = NULL;
	int putCar = lput(car);
	if(putCar != 1) {
		printf("Test failed! cannot put null car\n");
		exit(EXIT_FAILURE);
	}
	/*Make a nice car*/
	car = make_car("123", 100.00, 2020);
	
	
	/*test put to an empty list */
	printf("Testing put() to an empty list...\n");
	printf("Put car with plate=%s\n",car->plate);
	putCar = lput(car);
	if(putCar != 0) {
		printf("Test failed! *car may be null\n");
		exit(EXIT_FAILURE);
	}
	car_t* curr_head = lget();
	printf("Get car that was put, check if it's the right car...\n");
	printf("Plate of gotten car = %s\n",curr_head->plate);
	if(test_head(car, curr_head) != 0) {
		printf("Not the right car\n");
		exit(EXIT_FAILURE);
	}

	
	/*Put to an nonempty list*/
	printf("Testing put() to a non-empty list, put first car...\n");
	putCar = lput(car);
	car_t* nextCar = make_car("124", 100.00, 2020);
	printf("Put second car with plate=%s\n",nextCar->plate);
	lput(nextCar);
	
	printf("get() the second car and check if it's the right car\n");
	curr_head = lget();
	printf("Plate of gotten car = %s\n",curr_head->plate);
	if(test_head(nextCar, curr_head) != 0) {
		printf("Not the right car\n");
		exit(EXIT_FAILURE);
	}
	
	printf("get the first car and check if it's the right car\n");
	curr_head = lget();
	printf("Plate of gotten car = %s\n",curr_head->plate);
	if(test_head(car, curr_head) != 0) {
		printf("Not the right car\n");
		exit(EXIT_FAILURE);
	}
	
	printf("get() again and check if it's NULL/ensure all cars were removed\n");
	curr_head = lget();
	if(curr_head != NULL) {
		printf("Last car was not removed");
		exit(EXIT_FAILURE);
	}
	
	printf("Test passed! Good job\n");
	free(car);
	free(nextCar);
	exit(EXIT_SUCCESS);
}


