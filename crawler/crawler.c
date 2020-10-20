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
	FILE *pagefile = fopen(full_file_name, "w");
	if(pagefile == NULL){
		return 1;
	}
	char *url = webpage_getURL(pagep);
	char depth_html_len[20];
	sprintf(depth_html_len, "%d\n%d\n", webpage_getDepth(pagep), webpage_getHTMLlen(pagep));
	char* html = webpage_getHTML(pagep);
	if(fprintf(pagefile, "%s\n%s%s\n", url, depth_html_len, html) < 0) {
		fclose(pagefile);
		return 1;
	}
	fclose(pagefile);
	return 0;
}


int main(int argc, char *argv[]) {
	//Check for correct # of arguements
	if(argc != 4) {
		printf("usage: cralwer <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}
	//Check if valid directory
	struct stat path_stat;
	if(stat(argv[2], &path_stat) != 0) {
		printf("error accessing path\n");
		exit(EXIT_FAILURE);
	}
	
	if(!S_ISDIR(path_stat.st_mode)) {
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
	int hsize = (maxdepth+1) * 50;
	hashtable_t* thayer_hash = hopen(hsize);
	if(hput(thayer_hash, root_url, root_url, strlen(root_url)+1)==1) {
		printf("Putting url into hash didn't work");
		exit(EXIT_FAILURE);
	}

	int id_count = 1;
	// while loop over queue starts here
	webpage_t* curr_webpage;
	int curr_depth;
	while( (curr_webpage=qget(thayer_queue)) != NULL && (curr_depth = webpage_getDepth(curr_webpage)) <= maxdepth )  {
	// need to remove last depth without expanding
	
	  //check if fetch webpage html to local computer is successful
		bool fetch = webpage_fetch(curr_webpage);
		if(fetch != true) {
			//exit(EXIT_FAILURE);
			webpage_delete(curr_webpage);
			continue;
		}
		pagesave(curr_webpage, id_count, argv[2]);

		int pos = 0;
		char *result;
		while((pos = webpage_getNextURL(curr_webpage,pos, &result)) > 0) {
			int32_t keylen = strlen(result)+1;
			char* hresult = hsearch(thayer_hash, url_search, result, keylen);
			if(IsInternalURL(result) && hresult == NULL) {
				webpage_t *webpage_result = webpage_new(result, curr_depth+1, NULL);			
				if(hput(thayer_hash, result, result, keylen)==1) {
					printf("Putting url into hash didn't work");
					exit(EXIT_FAILURE);
				}
				if(qput(thayer_queue, webpage_result) == 1) {
					printf("Putting webpage into queue didn't work");
					exit(EXIT_FAILURE);
				}
			} else {
				free(result);
			}
		}
		webpage_delete(curr_webpage);
		id_count++;
	}
	webpage_delete(curr_webpage);
	while((curr_webpage=qget(thayer_queue)) != NULL) {
		webpage_delete(curr_webpage);
	}
	hclose(thayer_hash);
	qclose(thayer_queue);
	exit(EXIT_SUCCESS);
}	
