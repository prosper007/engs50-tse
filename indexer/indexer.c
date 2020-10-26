/* indexer.c --- 
 * 
 * 
 * Author: Team Higher Learning
 * Created: Sun Oct 25 17:18:47 2020 
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
#include "webpage.h"
#include <ctype.h>

char* NormalizeWord(char* word) {
	int str_len = strlen(word);
	//char* empty_word = "\0";
	//empty_word[0] = '\0';
	if(str_len < 3) {
		//strcpy(word,empty_word);
		return NULL;
	}
	for(int i = 0; i < str_len; i++) {
		if(isalpha(word[i]) == 0) {
			//strcpy(word,empty_word);
			return NULL;
		}
		if(isupper(word[i]) != 0) {
			//strncpy(word[i],tolower(word[i]), 1);
			//printf("here\n");
			word[i] = tolower(word[i]);
		}
	}
	return word;
}	

int main(void) {
	//Load webpage w/ ID 1
	webpage_t* loaded_page = pageload(1, "../utils/pages");
	//print words from HTML
	char *word;
	int pos = 0;
	while((pos = webpage_getNextWord(loaded_page,pos, &word)) > 0) {
		char* normalized = NormalizeWord(word);
		//strcpy(word,);
		if(normalized != NULL) {
			printf("%s\n", normalized); 
			//free(word);
		}
		free(word);
	}
	webpage_delete(loaded_page);
}
	
