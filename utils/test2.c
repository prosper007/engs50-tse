/* test2.c --- 
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Tue Oct 27 21:42:16 2020 (-0400)
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

typedef struct word_count {
	char word[50];
	queue_t* word_docs;
	FILE* indexnm;
} word_count_t;

void close_queue(void* elementp) {
	word_count_t* word_count = (word_count_t*) elementp;
	qclose(word_count->word_docs);
}

int main(void) {
	//indexsave(3, "indexnm");
	hashtable_t* indexer = indexload(3, "indexnm");
	indexsave(indexer, "indexnm_loaded");
	happly(indexer, close_queue);
	hclose(indexer);
	exit(EXIT_SUCCESS);
}
