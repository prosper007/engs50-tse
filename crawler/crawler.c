/* crawler.c --- 
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Sat Oct 17 17:10:47 2020 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "webpage.h"
#include "lqueue.h"
#include "lhash.h"
#include "queue.h"
#include "pageio.h"
#include <pthread.h>

lqueue_t* thayer_queue;
lhashtable_t* thayer_hash;
int id_count = 0;
pthread_mutex_t count_lock;
int has_error = 0;

typedef struct crawler_args{
	int maxdepth;
	char* pagedir;
} crawler_args_t;

//function to compare URLs
bool url_search(void* url_element, const void* url_key) {
	if(url_element == NULL || url_key == NULL) {
		return false;
	}
	char* u_e = (char *)url_element;
	char* u_k = (char *) url_key;
	if(strcmp(u_e, u_k) == 0) {
		return true; 
	}
	
	return false;
}

void* crawler_func(void *crawler_arg) {
	// update with lock
	int curr_id_count = 0;
	
	pthread_mutex_lock(&count_lock);
	id_count = 1;
	curr_id_count = id_count;
	pthread_mutex_unlock(&count_lock);
	
	crawler_args_t* argsp = (crawler_args_t*) crawler_arg;
	int maxdepth = argsp->maxdepth;
	char* pagedir = argsp->pagedir;
	
	// while loop over queue starts here
	webpage_t* curr_webpage;
	int curr_depth;
	while( (curr_webpage=lqget(thayer_queue)) != NULL && (curr_depth = webpage_getDepth(curr_webpage)) <= maxdepth )  {
	// need to remove last depth without expanding
	
	  //check if fetch webpage html to local computer is successful
		bool fetch = webpage_fetch(curr_webpage);
		if(fetch != true) {
			//exit(EXIT_FAILURE);
			webpage_delete(curr_webpage);
			continue;
		}
		
		// save page to directory
		pagesave(curr_webpage, curr_id_count, pagedir);
	
		// crawl through page for other URLs
		int pos = 0;
		char *result;
		while((pos = webpage_getNextURL(curr_webpage,pos, &result)) > 0) {
			int32_t keylen = strlen(result)+1;
			char* hresult = lhsearch(thayer_hash, url_search, result, keylen);
			if(IsInternalURL(result) && hresult == NULL) {
				webpage_t *webpage_result = webpage_new(result, curr_depth+1, NULL);			
				if(lhput(thayer_hash, result, result, keylen)==1) {
					printf("Putting url into hash didn't work");
					exit(EXIT_FAILURE);
				}
				if(lqput(thayer_queue, webpage_result) == 1) {
					printf("Putting webpage into queue didn't work");
					exit(EXIT_FAILURE);
				}
			} else {
				free(result);
			}
		}
		webpage_delete(curr_webpage);
		
		pthread_mutex_lock(&count_lock);
		id_count += 1;
		curr_id_count = id_count;
		pthread_mutex_unlock(&count_lock);
	}
	
	return NULL;
}

void join_threads(void* elementp){
	pthread_t* tp = (pthread_t*) elementp;
	if(pthread_join(*tp, NULL) != 0) {
		has_error = 1;
	}
}	

int main(int argc, char *argv[]) {
	//Check for correct # of arguements
	char* usage_msg = "usage: cralwer <seedurl> <pagedir> <maxdepth> <numthreads>\n";
	if(argc != 5) {
		printf("%s\n", usage_msg);
		exit(EXIT_FAILURE);
	}
	//Check if valid directory
	struct stat path_stat;
	if(stat(argv[2], &path_stat) != 0) {
		printf("error accessing path\n");
		exit(EXIT_FAILURE);
	}
	
	if(!S_ISDIR(path_stat.st_mode)) {
		printf("%s\n", usage_msg);
		printf("Not a valid directory\n");
		exit(EXIT_FAILURE);
	}
	
	//Check if depth is non-negative
	int maxdepth = atoi(argv[3]);
	if(maxdepth < 0) {
		printf("%s\n", usage_msg);
		printf("max depth must be positive\n");
		exit(EXIT_FAILURE);
	}
	
	//Check if numthreads is non-negative
	int numthreads = atoi(argv[4]);
	if(numthreads < 0) {
		printf("%s\n", usage_msg);
		printf("max depth must be positive\n");
		exit(EXIT_FAILURE);
	}
	
	//create a single new webpage at depth 0, w/ seed URL: https://thayer.github.io/engs50/
	//char *url = "https://thayer.github.io/engs50/";
	char *url = argv[1];
	char* root_url = (char*)malloc(strlen(url)+1);
	strcpy(root_url, url);
	webpage_t *thayer_webpage = webpage_new(root_url, 0, NULL);

	//put thayer webpage in queue
	thayer_queue = lqopen();
	if(lqput(thayer_queue, thayer_webpage) == 1) {
		printf("thayer_webpage not put in thayer_queue\n");
		exit(EXIT_FAILURE);
	}
	//create hash table
	int lhsize = (maxdepth+1) * 50;
	thayer_hash = lhopen(lhsize);
	if(lhput(thayer_hash, root_url, root_url, strlen(root_url)+1)==1) {
		printf("Putting url into hash didn't work");
		exit(EXIT_FAILURE);
	}
	
	pthread_mutex_init(&count_lock, NULL);
	
	crawler_args_t argsp = {maxdepth, argv[2]};
	
	queue_t* threads = qopen();
	for(int i = 0; i < numthreads; i++) {
		pthread_t* tp = (pthread_t*) malloc(sizeof(pthread_t));
		
		if(pthread_create(tp, NULL, crawler_func, (void*) &argsp)!= 0){
			exit(EXIT_FAILURE);
		}
		qput(threads, tp);
	}
	
	qapply(threads, join_threads);
	if(has_error != 0) {
		printf("Error in joining thread");
		exit(EXIT_FAILURE);
	}
	
	//webpage_delete(curr_webpage);
	webpage_t* curr_webpage;
	while((curr_webpage=lqget(thayer_queue)) != NULL) {
		webpage_delete(curr_webpage);
	}
	pthread_mutex_destroy(&count_lock);
	lhclose(thayer_hash);
	lqclose(thayer_queue);
	qclose(threads);
	exit(EXIT_SUCCESS);
}	
