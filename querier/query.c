/* query.c --- 
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Mon Nov  2 20:11:20 2020 (-0500)
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
#include "indexio.h"
#include "hash.h"
#include "queue.h"

#define MAX_LEN 501

char* normalize_word(char* word) {
	int str_len = strlen(word);

	for(int i = 0; i < str_len; i++) {
		if(isalpha(word[i]) == 0) {
			return NULL;
		}
		if(isupper(word[i]) != 0) {
			word[i] = tolower(word[i]);
		}
	}
	return word;
}	

bool search_word(void* elementp, const void* searchkeyp) {
	word_count_t* word_count = (word_count_t*) elementp;
	char* word = (char*) searchkeyp;
	if(strcmp(word_count->word, word) == 0) {
		return true;
	} else {
		return false;
	}
}

char doc_buffer[MAX_LEN];
int min = INT_MAX;

void get_count(void* elementp) {
	document_t* document = (document_t*)elementp;
	char count[50];
	sprintf(count, ":%d ", document->key_wc);
	strcat(doc_buffer, count);
	if(document->key_wc < min) {
		min = document->key_wc;
	}
}

int main(void) {
	char input[MAX_LEN];
	printf("> ");

	hashtable_t* index = indexload(1, "../indexer/indexnm");
	char output[MAX_LEN];
	
	while(fgets(input, MAX_LEN, stdin) != NULL) {
		char* delim = " \t\n";
		char* token;
		token = strtok(input, delim);
		while(token != NULL) {
			char* normalized = normalize_word(token);
			// if normalized word is less than 3 characters or is reserved word, "and", continue 
			if(strlen(normalized) >= 3 && strcmp(normalized, "and") != 0) { 
				if(normalized == NULL) {
					printf("[invalid query]\n");
					break;
				}
				word_count_t* word_count = (word_count_t*) hsearch(index, search_word, normalized, strlen(normalized));
				if(word_count != NULL) { // word exists in index
					strcat(output, word_count->word);
					qapply(word_count->word_docs, get_count);
					strcat(output, doc_buffer);
				}
				
			}
			
			token = strtok(NULL, delim);
			memset(doc_buffer, 0, MAX_LEN);
		}
		if(strlen(output) > 0) {
			printf("%s - %d\n", output, min);
		}
		min = INT_MAX;
		memset(output, 0, MAX_LEN);
		printf("> ");
	}
	exit(EXIT_SUCCESS);
}
