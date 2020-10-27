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
#include "hash.h"

typedef struct word_count {
	char word[50];
	int count;
} word_count_t;
 
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

// TODO: make_word_count
word_count_t* make_word_count(char* word, int count) {
	word_count_t* word_count = (word_count_t*) malloc(sizeof(word_count_t));
	strcpy(word_count->word, word);
	word_count->count = count;
	return word_count; 
}	

// TODO: search_word
bool search_word(void* elementp, const void* searchkeyp) {
	word_count_t* word_count = (word_count_t*) elementp;
	char* word = (char*) searchkeyp;
	if(strcmp(word_count->word, word) == 0) {
		return true;
	} else {
		return false;
	}
}

static int total_count = 0;

void add_word_count(void* elementp) {
	word_count_t* word_count = (word_count_t*) elementp;
	total_count += word_count->count;
}

int main(void) {
	//Load webpage w/ ID 1
	webpage_t* loaded_page = pageload(1, "../pages");
	hashtable_t* indexer = hopen(webpage_getHTMLlen(loaded_page));
	//print words from HTML
	char *word;
	int pos = 0;
	while((pos = webpage_getNextWord(loaded_page,pos, &word)) > 0) {
		char* normalized = NormalizeWord(word);
		//strcpy(word,);
		if(normalized != NULL) {
			printf("%s\n", normalized); 
			word_count_t* search_result = (word_count_t*) hsearch(indexer, search_word, normalized, strlen(normalized));
			// normalized word is not in hash table
			if(search_result == NULL) {
				word_count_t* word_count = make_word_count(normalized, 1);
				hput(indexer, word_count, normalized, strlen(normalized));
			} else { // normalized word is already in hash table
				search_result->count += 1; 
			}
		}
		free(word);
	}
	happly(indexer, add_word_count);
	printf("total count: %d\n", total_count);
	hclose(indexer);
	webpage_delete(loaded_page);
}
	
