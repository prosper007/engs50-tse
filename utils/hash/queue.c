/* queue.c --- 
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Mon Oct 12 16:13:48 2020 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

/* a struct to represent a single node of a queue */
typedef struct qnode {
  void* elementp;
  struct qnode *next;
} qnode_t;


/* the queue representation is hidden from users of the module */
typedef struct queue{
  qnode_t *front;
  qnode_t *back;
  struct queue *next_queue;
} queue_i;

/*
//define function to find a specific queue in an interface of queues
queue_i* find_q(queue_t *qp) {
  queue_i *qfind;
  for (qfind=queue_interface; qfind!= NULL; qfind=queue_interface->next_queue) {
    if(qfind==qp) {
      return qfind;
    }   
  }
	printf("Queue not found in interface\n");
  return NULL; 
}
*/

/* A function that creates a new queue node and allocates memory for it */
static qnode_t* new_qnode(void* elementp) {
  qnode_t *new_node;
  new_node = (qnode_t*)malloc(sizeof(qnode_t));
  new_node->elementp = elementp;
  new_node->next = NULL;
  return new_node;
}

/* create an empty queue */
queue_t* qopen(void) {
  queue_i* queue = (queue_i*)malloc(sizeof(queue_i)); // assign memory for queue
  queue->front = queue->back = NULL; // set front and back to NULL

	//add new queue to list of queues in interface
	/*queue->next_queue = queue_interface;
		queue_interface = queue; */
  return (queue_t*) queue;
}

/* deallocate a queue, frees everything in it */
void qclose(queue_t *qp) {
	/*queue_i* qi = find_q(qp); */
	queue_i* qi = (queue_i *) qp;
	if(qi == NULL){
		return;
	}
	
  while (qi->front != NULL) { // checks if qp is empty 
    qnode_t* temp = qi->front; // store initial value of front
    qi->front = qi->front->next; // move front to next node
    free(temp); // free the node
  }
  qi->front = qi->back = NULL; // just for safety reset front and back to NULL

	// remove queue from queue_interface
	/*
	queue_i* prev = NULL;
	for(queue_i* i = queue_interface; i!=NULL; i=i->next_queue) {
		if(i == qi) {
			if(prev == NULL){
				queue_interface = i->next_queue;
				break;
			}
			prev->next_queue = i->next_queue;
			break;
		}
		prev = i;
	}
	*/
  free(qi); //free the queue
  qi=NULL;
	qp=NULL;
	
}

/* Function to put an element at the back of the queue */
int32_t qput(queue_t *qp, void *elementp) {
  if (elementp == NULL) {
    return 1; // if element youre trying to add is NULL exit non-zero
  }
  qnode_t* node = new_qnode(elementp);

	//queue_i* qi = find_q(qp);
	queue_i* qi = (queue_i *) qp;
	if(qi == NULL){
		return 1; // return non-zero if queue doesn't exist
	}
	
  if (qi->front == NULL) {        // if queue is empty set both front and back to new node
    qi->front = qi->back = node;
  } else {
    qi->back->next = node;   // adding new element to a non-empty queue
    qi->back = node;
  }
  return 0;
}


void* qget(queue_t *qp) {
	//queue_i* qi = find_q(qp);
	queue_i* qi = (queue_i *) qp;
	if(qi == NULL){
		return NULL;
	}
	
  if (qi->front == NULL) {
    return NULL;
  }
  qnode_t* temp = qi->front;
  qi->front = qi->front->next;
  if (qi->front == NULL) {
    qi->back = NULL;
  }
	void* elementp = temp->elementp;
  free(temp);
	return elementp;
}

/* apply a function to every element of the queue */
void qapply(queue_t *qp, void (*fn)(void *elementp)) {
	//	queue_i *q = find_q(qp);
	queue_i* q = (queue_i *) qp;
	if (q==NULL) {
		return; // queue not found
	}
	//	qnode_t* node = q->front;
	for(qnode_t* node = q->front; node!=NULL; node=node->next) {
		fn(node->elementp);
	}
}


/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* qsearch(queue_t *qp, bool (*searchfn)(void* elementp,const void* keyp), const void* skeyp) {
	//queue_i *q = find_q(qp);
	queue_i* q = (queue_i *) qp;
	if(q == NULL){
		return NULL;
	}
	
	for(qnode_t* node = q->front; node!=NULL; node=node->next) {
		/*const void* keyp;
			keyp = skeyp;*/
		if (searchfn(node->elementp, skeyp)) {
			return node->elementp;
		}
	}
	return NULL;
}

/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void* qremove(queue_t *qp, bool (*searchfn)(void* elementp,const void* keyp), const void* skeyp) {
	//	queue_i* qi = find_q(qp);
	queue_i* qi = (queue_i *) qp;
	if(qi == NULL){
		return NULL; // return null if queue doesn't exist
	}

	qnode_t* prev = NULL;
	for(qnode_t* node = qi->front; node!=NULL; node=node->next) {
		if(searchfn(node->elementp, skeyp)){
			// remove node
			if(node==qi->front || node==qi->back){
				if(node==qi->front){ // node to be removed is front
					qi->front=qi->front->next;
				}
				if(node==qi->back) {
					qi->back = prev;
					if(qi->back != NULL) {
						qi->back->next = NULL;
					}
				}

				void* removed_element = node->elementp;
				free(node);
				return removed_element;
			} else{
				prev->next = node->next;
				void* removed_element = node->elementp;
				free(node);
				return removed_element;
			}
		}
		prev = node;
	}
	return NULL;
}

/* concatenates elements of q2 into q1
 * q2 is dealocated, closed, and unusable upon completion 
 */
void qconcat(queue_t *q1p, queue_t *q2p) {
	//	queue_i *qi_1 = find_q(q1p);
	queue_i* qi_1 = (queue_i *) q1p;
	if(qi_1 == NULL) {
		return;
	}
	//queue_i *qi_2 = find_q(q2p);
	queue_i* qi_2 = (queue_i *) q2p;
	if(qi_2 == NULL) {
		return;
	}

	qnode_t *node;
	for(node=qi_2->front; node!=NULL; node=node->next){
		//convert to appropriate data type instead of void*?
		qput(q1p, node->elementp);
	}
	qclose(q2p);
}


