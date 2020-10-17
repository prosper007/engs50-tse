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
#include "webpage.h"
#include <stdio.h>

int main(void) {
	//create a single new webpage at depth 0, w/ seed URL: https://thayer.github.io/engs50/
	char *url = "https://thayer.github.io/engs50/";
	webpage_t *thayer_webpage = webpage_new(url, 0, NULL);

	//check if fetch webpage html to local computer is successful
	bool fetch = webpage_fetch(*thayer_webpage);
	if(fetch != TRUE) {
		exit(EXIT_FAILURE);
	}
	
	// if successful, scan url
	char *html = webpage_getHTML(page);
	int pos = 0;
	char *result;
	while((pos = webpage_getNextURL(thayer_webpage,pos, &result)) > 0) {
		char* internal;
		if(IsInternal(result)) {
			internal = "internal";
		}
		else {
			internal = "external";
		}
		printf("Found url: %s, is %s \n", result, internal);
		free(result);
	}

	free(html);
	webpage_delete(page);
	exit(EXIT_SUCCESS);
}	
