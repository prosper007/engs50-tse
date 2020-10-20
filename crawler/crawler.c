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
#include <stdbool.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"


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


int main(void) {
	//create a single new webpage at depth 0, w/ seed URL: https://thayer.github.io/engs50/
	char *url = "https://thayer.github.io/engs50/";
	char* root_url = (char*)malloc(strlen(url)+1);
	strcpy(root_url, url);
	webpage_t *thayer_webpage = webpage_new(root_url, 0, NULL);

	//put thayer webpage in queue
	queue_t* thayer_queue = qopen();
	if(qput(thayer_queue, thayer_webpage) == 1) {
		printf("thayer_webpage not put in thayer_queue\n");
		exit(EXIT_FAILURE);
	}
	//create hash table
	hashtable_t* thayer_hash = hopen(50);
	if(hput(thayer_hash, root_url, root_url, strlen(root_url)+1)==1) {
		printf("Putting url into hash didn't work");
		exit(EXIT_FAILURE);
	}
	
	//check if fetch webpage html to local computer is successful
	bool fetch = webpage_fetch(thayer_webpage);
	if(fetch != true) {
		exit(EXIT_FAILURE);
	}
	
	// if successful, scan url
	//char *html = webpage_getHTML(thayer_webpage);
	int pos = 0;
	char *result;
	while((pos = webpage_getNextURL(thayer_webpage,pos, &result)) > 0) {
		int32_t keylen = strlen(result)+1;
		char* hresult = hsearch(thayer_hash, url_search, result, keylen);
		if(IsInternalURL(result) && hresult == NULL) {
			webpage_t *webpage_result = webpage_new(result, 1, NULL);			
			if(hput(thayer_hash, result, result, keylen)==1) {
				printf("Putting url into hash didn't work");
				exit(EXIT_FAILURE);
			}
			if(qput(thayer_queue, webpage_result) == 1) {
				printf("Putting webpage into queue didn't work");
				exit(EXIT_FAILURE);
			}
		}	else {
			free(result);
		}
	}
	
	webpage_t *webpage;
	while( (webpage=qget(thayer_queue)) != NULL) {
		printf("URL: %s\n", webpage_getURL(webpage));
		webpage_delete(webpage);
	}
	
	hclose(thayer_hash);
	qclose(thayer_queue);
	exit(EXIT_SUCCESS);
}	
