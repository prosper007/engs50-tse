/*
* list.c -- implement interface in list.h
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define MAXREG 10

static car_t *front=NULL;
//static car_t *front_next=NULL;

/* the representation of a car 
typedef struct car {
	struct car *next;
	char plate[MAXREG];
    double price;
    int year;
} car_t; */
	

  /* put(): place a car at the beginning of the list
   * returns 0 if successful; nonzero otherwise
   */
int32_t lput(car_t *cp){
	if(cp==NULL){
		return 1;
	}
	if(front==NULL){ // empty list
		front = cp;
	} else { // add to non-empty list
		cp->next = front;
		front = cp;
	}
	/*printf("front: %s\n", front->plate);
	if(front->next) {
		printf("front next: %s\n", front->next->plate);
	} */
	return 0;	
}
	
  /* get(): remove and return the first car in the list;
   * return NULL if the list is empty
   */
car_t *lget(){
	if(front==NULL){
		return NULL;
	}
	car_t* front_v = front;
	front = front->next;
	return front_v;
}

  /* apply a function to every car in the list */
  void lapply(void (*fn)(car_t *cp)) {
	  
	  car_t *c;	  
	  for(c=front; c!=NULL; c=c->next) {
		fn(c);
	  }
  }
	  
  /* remove(): find, remove, and return any car with
   * the designated plate; return NULL if not present
   */
  car_t *lremove(char *platep) {

	  car_t *c;
	  car_t *f = NULL; 
	  
	  for(c=front;c!=NULL;c=c->next) {
		  if(strcmp(platep,c->plate)==0) {
			  if(f==NULL){
				  return lget();
			  }
			  f->next=c->next;
			  return c;
		  }
		  f=c;
	  }  
	  return NULL;
  }
  
				




