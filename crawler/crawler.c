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

int32_t pagesave(webpage_t *pagep, int id, char *dirname){
	char full_file_name[50];
	sprintf(full_file_name, "%s/%d", dirname, id);
	printf("full file name: %s\n", full_file_name);
	FILE *pagefile = fopen(full_file_name, "w");
	if(pagefile == NULL){
		return 1;
	}
	char *url = webpage_getURL(pagep);
	char depth_html_len[20];
	sprintf(depth_html_len, "%d\n%d\n", webpage_getDepth(pagep), webpage_getHTMLlen(pagep));
	char* html = webpage_getHTML(pagep);
	if(fprintf(pagefile, "%s\n%s%s\n", url, depth_html_len, html) < 0) {
		return 1;
	}
	return 0;
}


int main(int argc, char *argv[]) {
	//Check for correct # of arguements
	if(argc != 4) {
		printf("usage: cralwer <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}
	//Check if valid directory
	if(access(argv[2], F_OK) != 0) {
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		printf("Not a valid directory\n");
		exit(EXIT_FAILURE);
	}
	//Check if depth is non-negative
	int maxdepth = atoi(argv[3]);
	if(maxdepth < 0) {
		printf("usage: cralwer <seedurl> <pagedir> <maxdepth>\n");
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
	pagesave(thayer_webpage, 1, "../pages");
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
