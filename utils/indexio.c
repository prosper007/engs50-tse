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

/*
typedef struct document {
	int id;
	int key_wc;
} document_t;

typedef struct word_count {
	char word[50];
	queue_t* word_docs;
} word_count_t;
*/

static document_t* make_doc(int id, int count) {
	document_t* document = (document_t*) malloc(sizeof(document));
	document->id = id;
	document->key_wc = count;
	return document;
}
// TODO: make_word_count
static word_count_t* make_word_count(char* word) {
	word_count_t* word_count = (word_count_t*) malloc(sizeof(word_count_t));
	
	strcpy(word_count->word, word);
	
	word_count->word_docs = qopen();
	
	return word_count; 
}	

static char indexnm_buffer[300000];
static char doc_counts[30000];

static void getDocCount(void* elementp) {
	document_t* document = (document_t*)elementp;
	char id_count[1000];
	sprintf(id_count, " %d %d", document->id, document->key_wc);
	strcat(doc_counts, id_count);
}

static void write_to_buffer(void* elementp) {
	word_count_t* word_count = (word_count_t*) elementp;

	qapply(word_count->word_docs, getDocCount);
	char word_doc_counts[40000];
	sprintf(word_doc_counts, "%s%s\n", word_count->word, doc_counts);
	strcat(indexnm_buffer, word_doc_counts);
	memset(doc_counts, 0, strlen(doc_counts));
}

int32_t indexsave(hashtable_t* indexer, char* indexnm) {
	FILE *index_file = fopen(indexnm, "w");
	if(index_file == NULL) {
		printf("Error opening file\n");
		return 1;
	}
	happly(indexer, write_to_buffer);
	fprintf(index_file, "%s", indexnm_buffer);
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
	char line[id*300];
	while(fgets(line, sizeof(line), index_file)) {
		char word[id*50];
		char doc_counts[id*250];
		sscanf(line, "%s %[^\n]", word, doc_counts);

		word_count_t* word_count = make_word_count(word);
		
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
