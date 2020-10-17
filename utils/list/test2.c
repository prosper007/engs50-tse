/*
* test2.c 
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


void add_price(car_t *car) {
		
		/*Add 100 to price*/
		car->price += 100;
}

int main(void) {
	
	car_t *car = make_car("123",100.00,2020);
	
	/*apply to empty list*/
	printf("apply to an empty list\n");
	lapply(add_price);
	
	printf("Put car with price = %lf into list\n",car->price);
	lput(car);
	
	//apply to a list with one entry
	printf("apply to a list with one entry\n");
	lapply(add_price);
	
	//Check if applied
	printf("Check if price is right, new price = %lf\n",car->price);
	if(car->price != 200) {
		printf("Price not correct");
		exit(EXIT_FAILURE);
	}
	
	//put second car
	car_t *car2 = make_car("124",101.00,2021);
	printf("Put another car with price = %lf into list\n",car2->price);	
	lput(car2);
	
	//apply to a list with more then one entry
	printf("apply to a list with more then one entry\n");
	lapply(add_price);
	
	//Check if applied
	printf("Check if prices are right, new prices: car = %lf, car2 = %lf\n",car->price,car2->price);
	if((car->price!=300.00)||(car2->price!=201.00)) {
		printf("Prices not correct");
		exit(EXIT_FAILURE);
	}
	
	printf("Note: the function passed into lapply adds $100.00 to the price of the car\n");
	printf("Test passed! Good job.\n");
	free(car);
	free(car2);
	exit(EXIT_SUCCESS);
}





