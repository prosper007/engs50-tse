#pragma once
/* indexio.h --- 
 * 
 * 
 * Author: Chikezie Onungwa
 * Created: Tue Oct 27 20:48:56 2020 (-0400)
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

int32_t indexsave(hashtable_t* indexer, char* indexnm);

hashtable_t *indexload(int id, char* indexnm);
