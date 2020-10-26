/* crawler.c --- 
 * 
 * 
 * Author: Team Higher Learning
 * Created: Sat Oct 24 17:18:47 2020 
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
#include "pageio.h"

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
	if(fprintf(pagefile, "%s\n%s%s", url, depth_html_len, html) < 0) {
		fclose(pagefile);
		return 1;
	}
	fclose(pagefile);
	return 0;
}

/* 
 * pageload -- loads the numbered filename <id> in directory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
webpage_t *pageload(int id, char *dirnm) {
	
	//Open file ID at dirnm
	char full_file_name[50];
	sprintf(full_file_name, "%s/%d", dirnm, id);
	FILE *pagefile = fopen(full_file_name, "r");    //"r" bc file is only to be read
	
	//Check if pagefile is non-empty
	if(pagefile == NULL){
		printf("Requested file is empty\n");
		return NULL;
	}

	char url[50];
	int depth = 0;
	int htmlLen;
	fscanf(pagefile, "%s %d %d", url, &depth, &htmlLen);

	// skip over new line character
	fgetc(pagefile);
	
	int ch;
	//	char html[3000];
	char* html = (char*) malloc(3000 * sizeof(char));
	int i = 0;
	while( (ch=fgetc(pagefile)) != EOF) {
		html[i] = (char) ch;
		i++;
	}
	html[i] = '\0';
	
	webpage_t *loaded_webpage = webpage_new(url, depth, html);
	fclose(pagefile);
	return loaded_webpage;
}
	
	
	
	
	
	
