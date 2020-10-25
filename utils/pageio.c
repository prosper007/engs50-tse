/* pageio.c --- Module that implements pageio.h
 * 
 * 
 * Author: Tyler Hatch
 * Created: Sun Oct 25 13:22:32 2020 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>                                                                                                                                                                                         
#include <stdlib.h>                                                                                                                                                                                        
#include <string.h>                                                                                                                                                                                        
#include <inttypes.h>                                                                                                                                                                                      
#include <unistd.h>                                                                                                                                                                                        
#include "webpage.h"
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
	if(fprintf(pagefile, "%s\n%s%s\n", url, depth_html_len, html) < 0) {
		fclose(pagefile);
		return 1;
	}
	fclose(pagefile);
	return 0;
}

webpage_t *pageload(int id, char *dirnm);

