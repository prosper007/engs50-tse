/* indexer.c --- 
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Tue Oct 27 19:18:42 2020 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */
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
#include "queue.h"
#include "indexio.h"
#include <dirent.h>

typedef struct document {
	int id;
	int key_wc;
} document_t;

typedef struct word_count {
	char word[50];
	queue_t* word_docs;
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
word_count_t* make_word_count(char* word) {
	word_count_t* word_count = (word_count_t*) malloc(sizeof(word_count_t));
	
	strcpy(word_count->word, word);
	
	word_count->word_docs = qopen();

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

void close_queue(void* elementp) {
	word_count_t* word_count = (word_count_t*) elementp;
	qclose(word_count->word_docs);
}

int main(int argc, char* argv[]) {
	//Load webpage w/ ID 1
	if(argc != 3) {
		printf("usage: indexer <pagedir> <indexnm>\n");
		exit(EXIT_FAILURE);
	}
	
	struct stat path_stat;
	if(stat(argv[1], &path_stat) != 0) {
		printf("error accessing path\n");
		exit(EXIT_FAILURE);
	}
	if(!S_ISDIR(path_stat.st_mode)) {
		printf("usage: indexer <pagedir> <indexnm>\n");
		printf("Not a valid directory\n");
		exit(EXIT_FAILURE);
	}
	
	DIR *pages_dir;
	struct dirent *dir;
	pages_dir = opendir(argv[1]);
	if(pages_dir == NULL) {
		printf("error opening pagedir\n");
		exit(EXIT_FAILURE);
	}
	int id = 0;
	while((dir = readdir(pages_dir)) != NULL) {
		int curr_id = atoi(dir->d_name);
		if(curr_id > id) {
			id = curr_id;
		}
	}
	closedir(pages_dir);
	if(id < 1) {
		printf("pagedir is empty\n");
		exit(EXIT_FAILURE);
	}
	printf("max_id: %d\n", id);
	FILE* index_file = fopen(argv[2], "w");
	if(index_file == NULL) {
		printf("Error opening file\n");
		exit(EXIT_FAILURE);
	}
	
	hashtable_t* indexer = hopen(300*id);
	for(int i = 1; i <= id; i++) {
		webpage_t* loaded_page = pageload(i, argv[1]);
		//int htmlLen = webpage_getHTMLlen(loaded_page);
		char *word;
		int pos = 0;
		while((pos = webpage_getNextWord(loaded_page,pos, &word)) > 0) {
			char* normalized = NormalizeWord(word);
			//strcpy(word,);
			if(normalized != NULL) {
				word_count_t* search_result = (word_count_t*) hsearch(indexer, search_word, normalized, strlen(normalized));
				// normalized word is not in hash table
				if(search_result == NULL) {
					word_count_t* word_count = make_word_count(normalized);
					document_t* document = make_doc(i, 1);
					qput(word_count->word_docs, document);
					hput(indexer, word_count, normalized, strlen(normalized));
				} else { // normalized word is already in hash table
					queue_t* doc_queue = search_result->word_docs;
					document_t* doc_search = qsearch(doc_queue, find_doc, &i);
					if(doc_search == NULL) { // document is not in queue of docs associated with word
						document_t* doc = make_doc(i, 1);
						qput(doc_queue, doc);
					} else { // document is in queue. increase count of normalized word in doc
						doc_search->key_wc += 1;
					}
				}
			}
			free(word);
		}
		webpage_delete(loaded_page);
	}
	happly(indexer, sumwords);
	printf("total count: %d\n", total_count);
	indexsave(indexer, argv[2]);
	fclose(index_file);
	happly(indexer, close_queue);
	hclose(indexer);
}
	
