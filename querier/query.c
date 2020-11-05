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
#include <dirent.h>

#define MAX_LEN 501

typedef struct doc_tally {
	int id;
	queue_t* word_idxs;
	int rank;
	char url[100];
} doc_tally_t;

typedef struct and_sequence {
	queue_t* and_queries; // list of consecutive implict and explict "and" operands
	int seq_len; // length of and_queries
	int seq_idx;
} and_seq_t;

typedef struct word_idx {
	char word[50];
	int seq_idx;
	int key_wc;
} word_idx_t;

// Create a global hashtable to put ID and search tallies
queue_t* query;
hashtable_t* query_results;
hashtable_t* doc_tally_hash;
int seq_idx = 1;
char* curr_word;
int curr_seq_idx;
int curr_filter_idx;
int curr_filter_len;
int doc_actual_len = 0;
int curr_doc_rank = INT_MAX;

doc_tally_t* make_doc_tally(int id, int rank) {
	doc_tally_t* doc_tally = (doc_tally_t*) malloc(sizeof(doc_tally_t));
	doc_tally->id = id;
	doc_tally->word_idxs = qopen();
	doc_tally->rank = rank;
	return doc_tally;
}

and_seq_t* make_and_sequence(int seq_idx) {
	and_seq_t* and_seq = (and_seq_t*) malloc(sizeof(and_seq_t));
	and_seq->and_queries = qopen();
	and_seq->seq_len = 0;
	and_seq->seq_idx = seq_idx;
	return and_seq;
}

word_idx_t* make_word_idx(char* word, int seq_idx, int key_wc) {
	word_idx_t* word_idx = (word_idx_t*) malloc(sizeof(word_idx_t));
	strcpy(word_idx->word, word);
	word_idx->seq_idx = seq_idx;
	word_idx->key_wc = key_wc;
	return word_idx;
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

	word_idx_t* word_idx = make_word_idx(curr_word, curr_seq_idx, document->key_wc);

	doc_tally_t* search_res = hsearch(query_results, find_doc_id, doc_id, strlen(doc_id));
	if(search_res != NULL) { // doc_id is already is doc_tally hashtable
		qput(search_res->word_idxs, word_idx);
		if(document->key_wc < search_res->rank) {
			search_res->rank = document->key_wc;
		}
	} else {
		doc_tally_t* doc_tally = make_doc_tally(document->id, document->key_wc);
		qput(doc_tally->word_idxs, word_idx);
		hput(doc_tally_hash, doc_tally, doc_id, strlen(doc_id)); //put search tally for each id
	}
}

void get_actual_len(void* elementp) {
	word_idx_t* word_idx = (word_idx_t*) elementp;
	if(word_idx->seq_idx == curr_filter_idx) {
		doc_actual_len += 1;
	}
	if(word_idx->key_wc < curr_doc_rank) {
		curr_doc_rank = word_idx->key_wc;
	}
}

void filter_doc_tallies(void* elementp) {
	doc_tally_t* doc_tally = (doc_tally_t*) elementp;
	qapply(doc_tally->word_idxs, get_actual_len);
	if(doc_actual_len == curr_filter_len) {
		char id[50];
		sprintf(id, "%d", doc_tally->id);
		doc_tally_t* search_result = hsearch(query_results, find_doc_id, id, strlen(id));
		if(search_result == NULL) { // document is not already in result hashtable
			doc_tally_t* doc_tally_copy = make_doc_tally(doc_tally->id, curr_doc_rank);
			char file_loc[50];
			sprintf(file_loc, "../pages/%d", doc_tally_copy->id);
			FILE *id_file = fopen(file_loc, "r");
			if(id_file == NULL) {
				return;
			}
			fgets(doc_tally_copy->url, sizeof(doc_tally_copy->url), id_file);
			hput(query_results, doc_tally_copy, id, strlen(id));
			fclose(id_file);
		} else { // document is already in result hashtable. just increase rank
			search_result->rank += curr_doc_rank;
		}
	}
	curr_doc_rank = INT_MAX;
	doc_actual_len = 0;
}

void process_query(void* elementp) { // process all "and" sequences, connected by "or"s
	and_seq_t* and_seq = (and_seq_t*) elementp;
	curr_filter_idx = and_seq->seq_idx;
	curr_filter_len = and_seq->seq_len;
	happly(doc_tally_hash, filter_doc_tallies); 
}

void print_query_results(void* elementp) {
	doc_tally_t* doc_tally = (doc_tally_t*) elementp;
	printf("rank: %d, doc: %d, url: %s", doc_tally->rank, doc_tally->id, doc_tally->url);
}

void close_queue(void* elementp) {
	word_count_t* word_count = (word_count_t*) elementp;
	qclose(word_count->word_docs);
}

void close_doc_tallies(void* elementp) {
	doc_tally_t* doc_tally = (doc_tally_t*) elementp;
	qclose(doc_tally->word_idxs);
}

void close_and_seqs(void* elementp) {
	and_seq_t* and_seq = (and_seq_t*) elementp;
	qclose(and_seq->and_queries);
}

void put_word(and_seq_t* and_seq, char* word) {
	char* word_copy = (char*) malloc(strlen(word)+1);
	strcpy(word_copy, word);
	qput(and_seq->and_queries, word_copy);
	and_seq->seq_len += 1;
}

void cleanup() {
	happly(doc_tally_hash, close_doc_tallies);
	hclose(doc_tally_hash);
		
	happly(query_results, close_doc_tallies);
	hclose(query_results);

	qapply(query, close_and_seqs);
	qclose(query);	
}

int main(int argc, char* argv[]) {
	if(argc < 3 || argc > 4) {
		printf("usage: query <pageDirectory> <indexFile> [-q]\n");
		exit(EXIT_FAILURE);
	}

	struct stat path_stat;
	if(stat(argv[1], &path_stat) != 0) {
		printf("error accessing path\n");
		exit(EXIT_FAILURE);
	}
	if(!S_ISDIR(path_stat.st_mode)) {
		printf("usage: query <pageDirectory> <indexFile> [-q]\n");
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
	
	char indexer_command[100];
	sprintf(indexer_command, "../indexer/indexer %s ../indexer/%s", argv[1], argv[2]);
	system(indexer_command);
	
	char index_file_path[100];
	sprintf(index_file_path, "../indexer/%s", argv[2]);
	hashtable_t* index = indexload(id, index_file_path);
	
	char input[MAX_LEN];
	printf("> ");
	
	while(fgets(input, MAX_LEN, stdin) != NULL) {
		query = qopen();
		doc_tally_hash = hopen(MAX_LEN);
		query_results = hopen(MAX_LEN);
		
		char* delim = " \t\n";
		char* token;
		token = strtok(input, delim);
		if(token == NULL) {
			printf("> ");
			cleanup();
			continue;
		}
		bool first_word = true;
		bool existing_seq = false;
		bool found_and = false;
		bool found_or = false;
		bool has_error = false;
		and_seq_t* and_seq = NULL;
		char* normalized;
		char normalized_query[MAX_LEN];
		normalized_query[0] = '\0';
		
		while(token != NULL) {
			normalized = normalize_word(token);
			// normalized word contains non-alphabetic character
			if(normalized == NULL) {
				printf("[invalid query]\n");
				has_error = true;
				//cleanup();
				break;
			}

			// first word of query is operator "and" or "or"
			if(first_word && (strcmp(normalized, "and") == 0 || strcmp(normalized, "or") == 0)) {
				printf("[invalid query]\n");
				has_error = true;
				//cleanup();
				break;
			}
			first_word=false;

			// if normalized word is less than 3 characters and is not reserved word, "or", continue
			if(strlen(normalized) < 3 && strcmp(normalized, "or") != 0) {
				token = strtok(NULL, delim);
				continue;
			}

			if(strcmp(normalized, "and") == 0) {
				if(found_and || found_or) {
					printf("[invalid query]\n");
					has_error = true;
					found_and = false;
					found_or = false;
					//cleanup();
					break;
				}
				found_and = true;
				token = strtok(NULL, delim);
				strcat(normalized_query, normalized);
				strcat(normalized_query, " ");
				continue;
			} else if(found_and && strcmp(normalized, "or") != 0) {
				found_and = false;
			}

			if(strcmp(normalized, "or") == 0) {
				if(found_and || found_or) {
					found_and = false;
					found_or = false;
					printf("[invalid query]\n");
					has_error = true;
					//cleanup();
					break;
				}
				found_or = true;
				qput(query, and_seq);
				and_seq = NULL;
				existing_seq = false;
				token = strtok(NULL, delim);
				strcat(normalized_query, normalized);
				strcat(normalized_query, " ");
				continue;
			} else if(found_or) {
				found_or = false;
			}
			
			if(!existing_seq){
				and_seq = make_and_sequence(seq_idx);
				seq_idx+=1;
				existing_seq = true;
			}
			
			put_word(and_seq, normalized);
			strcat(normalized_query, normalized);
			strcat(normalized_query, " ");
			token = strtok(NULL, delim);

			word_count_t* word_count = (word_count_t*) hsearch(index, search_word, normalized, strlen(normalized));
			if(word_count != NULL) { // word exists in index
				curr_word = normalized;
				curr_seq_idx = and_seq->seq_idx;
				qapply(word_count->word_docs, put_searchtally);
			}
		}

		if(found_and || found_or) {
			printf("[invalid query]\n");
			has_error = true;
		}

		if(!has_error) {
			qput(query, and_seq);
			qapply(query, process_query);
			printf("%s\n", normalized_query);
			happly(query_results, print_query_results);
			and_seq = NULL;
			//cleanup();
		} else if(and_seq != NULL){
			qclose(and_seq->and_queries);
			free(and_seq);
		}
				
		cleanup();
		
		seq_idx = 1;
		memset(normalized_query, 0, sizeof(normalized_query));
		printf("> ");
	}
	happly(index, close_queue);
	hclose(index);
	
	exit(EXIT_SUCCESS);
}
