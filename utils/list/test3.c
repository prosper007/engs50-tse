/*
* test3.c 
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

int check_length(int expected_len, car_t *expected_head) {
	car_t* curr_head = lget();
	car_t* cp;
	int len = 0;
	
	if(expected_head != curr_head) {
		printf("Test failed! expected head's plate: %s but actual head's plate: %s\n", expected_head->plate, curr_head->plate);
		return 1;
	}		
		
	for(cp=curr_head; cp!=NULL; cp=cp->next) {
		printf("%s\n", cp->plate);
		len++;
	}
	lput(curr_head);
		
	if(len!=expected_len) {
		printf("Test failed! expected list len: %d but actual list len: %d\n", expected_len, len);
		return 1;
	}
	return 0;
}

void add_price(car_t *car) {
		
		/*Add 100 to price*/
		car->price += 100;
}

int main(void) {
	
	//put 2 cars
	car_t *car = make_car("123",100.00,2020);
	car_t *car2 = make_car("124",101.00,2021);
	car_t *car3 = make_car("125",102.00,2022);
	//lput(car);
	//lput(car2);
	
	//check when remove from an empty list
	printf("Check when removing from an empty list...\n");
	printf("Returned NULL as expected\n");
	if(lremove(car->plate) != NULL) {
		printf("Test failed\n");
		exit(EXIT_FAILURE);
	}
	
	//now put in cars
	printf("put three car into list with plate numbers: %s, %s, %s\n",car->plate,car2->plate,car3->plate);
	lput(car3);
	lput(car2);
	lput(car);
	
	//remove from non-empty list
	printf("remove car with plate %s which is at beginning of the list\n",car->plate);
	if(lremove(car->plate) != car) {
		printf("Test failed\n");
		exit(EXIT_FAILURE);
	}
	
	// check list is intact (head is correct & len is correct)
	printf("check if list head and length are correct after remove was applied\n");
	printf("list head and length correct!\n");
	if(check_length(2,car2) != 0){
		exit(EXIT_FAILURE);
	}
	
	printf("put the car back in the list\n");
	//put car back 
	lput(car);
	
	//remove car2 (middle of list) 
	printf("remove car with plate %s which is at the middle of the list\n",car2->plate);
	if(lremove(car2->plate) != car2) {
		printf("Test failed\n");
		exit(EXIT_FAILURE);
	}


	// check list is intact (head is correct & len is correct)
	printf("check if list head and length are correct\n");
	printf("list head and length correct!\n");
	if(check_length(2,car) != 0){
		exit(EXIT_FAILURE);
	}
	
	//put car 2 back
	printf("put car back into the list\n");
	lput(car2);
	
	//remove car3 (last of list) 
	printf("remove car with plate %s which is at the end of the list\n",car3->plate);
	if(lremove(car3->plate) != car3) {
		printf("Test failed\n");
		exit(EXIT_FAILURE);
	}
	
	// check list is intact (head is correct & len is correct)
	printf("check if list head and length are correct\n");
	printf("list head and length correct!\n");
	if(check_length(2,car2) != 0){
		exit(EXIT_FAILURE);
	}
	
	//free cars from memory
	free(car);
	free(car2);
	free(car3);
	
	printf("Test passed! Good job.\n");
	exit(EXIT_SUCCESS);
}





