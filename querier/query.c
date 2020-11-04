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

typedef struct doc_tally {
	int id;
	int query_tally;
	int rank;
	char url[100];
} doc_tally_t;

doc_tally_t* make_doc_tally(int id, int query_tally, int rank) {
	doc_tally_t* doc_tally = (doc_tally_t*) malloc(sizeof(doc_tally_t));
	doc_tally->id = id;
	doc_tally->query_tally = query_tally;
	doc_tally->rank = rank;
	return doc_tally;
}
	
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

// Create a global hashtable to put ID and search tallies
hashtable_t* doc_tally_hash;
queue_t* query_docs;
int normalized_count = 0;

bool find_doc_id(void* elementp, const void* searchkeyp) {
	doc_tally_t* doc_tally = (doc_tally_t*) elementp;
	char* search_id = (char*) searchkeyp;
	char doc_id[50];
	sprintf(doc_id, "%d", doc_tally->id);
	if(strcmp(doc_id, search_id) == 0){
		return true;
	}
	return false;
}

//A function (that we will qapply to queue of docs for each word) that puts id's into hashtable
void put_searchtally(void* elementp) {
	document_t* document = (document_t*) elementp;
	char doc_id[50];
	sprintf(doc_id, "%d", document->id);
	doc_tally_t* search_res = hsearch(doc_tally_hash, find_doc_id, doc_id, strlen(doc_id));
	if(search_res != NULL) { // doc_id is already is doc_tally hashtable
		search_res->query_tally += 1;
		if(document->key_wc < search_res->rank) {
			search_res->rank = document->key_wc;
		}
	} else {
		doc_tally_t* doc_tally = make_doc_tally(document->id, 1, document->key_wc);
		hput(doc_tally_hash, doc_tally, doc_id, strlen(doc_id)); //put search tally for each id
	}
}

void print_hash(void* elementp) {
	doc_tally_t* doc_tally = (doc_tally_t*) elementp;
	printf("doc id: %d, query tally: %d, rank: %d\n", doc_tally->id, doc_tally->query_tally, doc_tally->rank);
}

void filter_docs(void* elementp) {
	doc_tally_t* doc_tally = (doc_tally_t*) elementp;
	if(doc_tally->query_tally == normalized_count) {
		doc_tally_t* doc_tally_copy = make_doc_tally(doc_tally->id, doc_tally->query_tally, doc_tally->rank);
		char file_loc[50];
		sprintf(file_loc, "../pages/%d", doc_tally_copy->id);
		FILE *id_file = fopen(file_loc, "r");
		if(id_file == NULL) {
			return;
		}
		fgets(doc_tally_copy->url, sizeof(doc_tally_copy->url), id_file);
		qput(query_docs, doc_tally_copy);
		fclose(id_file);
	}
}

void print_queue(void* elementp) {
	doc_tally_t* doc_tally = (doc_tally_t*) elementp;
	printf("rank: %d, doc: %d, url: %s", doc_tally->rank, doc_tally->id, doc_tally->url);
}

void close_queue(void* elementp) {
	word_count_t* word_count = (word_count_t*) elementp;
	qclose(word_count->word_docs);
}

int main(void) {
	char input[MAX_LEN];
	printf("> ");

	hashtable_t* index = indexload(7, "../indexer/indexnm");
	
	while(fgets(input, MAX_LEN, stdin) != NULL) {
		doc_tally_hash = hopen(MAX_LEN);
		query_docs = qopen();
		
		char* delim = " \t\n";
		char* token;
		normalized_count = 0; // Count to keep track of the number of words in the query
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
					qapply(word_count->word_docs, put_searchtally); // DOUBLE CHECK THIS
				}
			}
			normalized_count++; // increment normalized count 
			token = strtok(NULL, delim);
		}
		//happly(doc_tally_hash, print_hash);
		happly(doc_tally_hash, filter_docs);
		qapply(query_docs, print_queue);
		// NEED TO CHECK IF SEARCH TALLY FOR EACH ID MATCHES NORMALIZED_COUNT
		hclose(doc_tally_hash);
		qclose(query_docs);
		printf("> ");
	}
	happly(index, close_queue);
	hclose(index);
	exit(EXIT_SUCCESS);
}
