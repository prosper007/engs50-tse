/* test1.c --- 
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Sun Oct 25 16:09:32 2020 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "pageio.h"
#include "webpage.h"


int main(void) {
	char *url = "https://thayer.github.io/engs50/";
	char* root_url = (char*)malloc(strlen(url)+1);
	strcpy(root_url, url);
	webpage_t *thayer_webpage = webpage_new(root_url, 0, NULL);

	//check if fetch webpage html to local computer is successful
	bool fetch = webpage_fetch(thayer_webpage);
	if(fetch != true) {
		webpage_delete(thayer_webpage);
		exit(EXIT_FAILURE);
	}
	pagesave(thayer_webpage, 1, "./pages");
	webpage_t *loaded_webpage = pageload(1, "./pages");
	if(strcmp(webpage_getURL(thayer_webpage), webpage_getURL(loaded_webpage)) != 0 ) {
			printf("Test failed! loaded URL diffferent from saved URL\n");
			exit(EXIT_FAILURE);
	}
	if(webpage_getDepth(thayer_webpage) != webpage_getDepth(loaded_webpage)) {
		printf("Test failed! loaded depth diffferent from saved depth\n");
		exit(EXIT_FAILURE);		
	}
	if(webpage_getHTMLlen(thayer_webpage) != webpage_getHTMLlen(loaded_webpage)) {
		printf("Test failed! loaded HTML len diffferent from saved HTML len\n");
		exit(EXIT_FAILURE);
	}
	if(strcmp(webpage_getHTML(thayer_webpage), webpage_getHTML(loaded_webpage)) != 0 ) {
		printf("Test failed! loaded HTML diffferent from saved HTML\n");
		exit(EXIT_FAILURE);
	}
	printf("Test passed!\n");
	exit(EXIT_SUCCESS);
		
}
