/* indexio.c --- 
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Tue Oct 27 20:55:48 2020 (-0400)
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
#include "queue.h"
#include "indexio.h"

typedef struct document {
	int id;
	int key_wc;
} document_t;

typedef struct word_count {
	char word[50];
	queue_t* word_docs;
	FILE* indexnm;
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

document_t* make_doc(int id, int count) {
	document_t* document = (document_t*) malloc(sizeof(document));
	document->id = id;
	document->key_wc = count;
	return document;
}
// TODO: make_word_count
word_count_t* make_word_count(char* word, FILE* indexnm) {
	word_count_t* word_count = (word_count_t*) malloc(sizeof(word_count_t));
	
	strcpy(word_count->word, word);
	
	word_count->word_docs = qopen();

	word_count->indexnm = indexnm;
	
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

bool find_doc(void* elementp, const void* keyp) {
	document_t* document = (document_t*) elementp;
	int* id = (int*) keyp;
	if(document->id == *id) {
		return true;
	}
	return false;
}

static int total_count = 0;
static int word_doc_count = 0;

void sum_word_doc_count(void* elementp) {
	document_t* document = (document_t*) elementp;
	word_doc_count += document->key_wc;
}

void sumwords(void* elementp) {
	word_count_t* word_count = (word_count_t*) elementp;
	qapply(word_count->word_docs, sum_word_doc_count);
	total_count += word_doc_count;
	word_doc_count = 0;
}

static char doc_counts[30000];

void getDocCount(void* elementp) {
	document_t* document = (document_t*)elementp;
	char id_count[100];
	sprintf(id_count, " %d %d", document->id, document->key_wc);
	strcat(doc_counts, id_count);
}

void write_file(void* elementp) {
	word_count_t* word_count = (word_count_t*) elementp;
	qapply(word_count->word_docs, getDocCount);
	FILE* indexnm = word_count->indexnm;
	if(fprintf(indexnm, "%s%s\n", word_count->word, doc_counts) < 0) {
		//fclose(indexnm);
		//memset(doc_counts, 0, sizeof(doc_counts));
		return;
	}
	memset(doc_counts, 0, sizeof(doc_counts));
}

int32_t indexsave(hashtable_t* indexer, char* indexnm) {
	FILE *index_file = fopen(indexnm, "w");
	if(index_file == NULL) {
		printf("Error opening file\n");
		return 1;
	}
	happly(indexer, write_file);
	fclose(index_file);
	return 0;
}

hashtable_t *indexload(int id, char* indexnm) {
	FILE *index_file = fopen(indexnm, "r");
	if(index_file == NULL) {
		printf("Error opening file\n");
		return NULL;
	}

	hashtable_t* indexer = hopen(300*id);
	char line[300];
	while(fgets(line, sizeof(line), index_file)) {
		char word[50];
		char doc_counts[250];
		sscanf(line, "%s %[^\n]", word, doc_counts);

		word_count_t* word_count = make_word_count(word, index_file);
		
		char* token;
		const char* delim = " ";

		token = strtok(doc_counts, delim);
		int doc_id = atoi(token);
		token = strtok(NULL, delim);
		int count = atoi(token);
		document_t* document = make_doc(doc_id, count);
		qput(word_count->word_docs, document);

		while(token != NULL) {
			token = strtok(NULL, delim);
			if(token == NULL) {
				break;
			}
			int doc_id = atoi(token);
			token = strtok(NULL, delim);
			int count = atoi(token);
			document_t* document = make_doc(doc_id, count);
			qput(word_count->word_docs, document);
		}
		hput(indexer, word_count, word, strlen(word));
	}
	
	fclose(index_file);

	return indexer;
	
}
