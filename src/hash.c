/**
 * (C) 2010 Michael J. Beer
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static struct HashEntry *getHashEntry(struct HashTable *hash, char *key) {
    unsigned int i = hash->hashFunc(hash, key);
    struct HashEntry *found = hash->hash[i];
    while(found->next) {
        if(strcmp(found->next->key, key) == 0) {
            return found;
        };
        found = found->next;
    };
    return found;
}


struct HashTable * hashTableCreate(unsigned int n, unsigned int(*
hashFunc)(struct HashTable *hash, char *)) {
    int i;
    struct HashTable *hash = (struct HashTable *)malloc(sizeof(struct HashTable));
    hash->hashFunc = (hashFunc) ? hashFunc : stdHashFunction;
    hash->size = n;
    hash->hash = malloc(sizeof(struct HashEntry *) * hash->size);
    for( i = 0; i < hash->size; i++) {
        hash->hash[i] = (struct HashEntry *)malloc(sizeof(struct HashEntry));
        hash->hash[i]->key = 0;
        hash->hash[i]->next = 0;
    };
    return hash;
}


void hashTableDispose(struct HashTable *hash) {
    int i = 0;
    for(i = 0; i < hash->size; i++) {
        struct HashEntry *entry = hash->hash[i];
        struct HashEntry *next;
        while((entry)) {
#ifdef HASH_TABLE_SAFE_KEY
            next = entry->next;
            if(entry->key) free(entry->key);
#endif
            free(entry);
            entry = next;
        };
        free(entry);
    };
    free(hash->hash);
    free(hash);
}



void *hashTableSet(struct HashTable * hashTable, char *key, void *val) {
    struct HashEntry * found = getHashEntry(hashTable, key);
    void *old = NULL;
    if(!found->next) {
        found->next =  malloc(sizeof(struct HashEntry));
        found->next->next = 0;
#ifdef HASH_TABLE_SAFE_KEY
        found->next->key = malloc(sizeof(char) * (1 + strlen(key)));
        strcpy(found->next->key, key);
#else
        found->next->key = key;
#endif
        old = found->next->value;
    }
    found->next->value = val;
    return old; /* found->next->value; */
}



void *hashTableDelete(struct HashTable * hashTable, char *key) {
    struct HashEntry * found = getHashEntry(hashTable, key);
    struct HashEntry * next;
    void *value;
    if(!found->next) return found->next;
    next = found->next->next;
    value = found->next->value;
#ifdef HASH_TABLE_SAFE_KEY
    free(found->next->key);
#endif
    free(found->next);
    found->next = next;
    return value;
}



void *hashTableGet(struct HashTable * hashTable, char *key) {
    struct HashEntry * found = getHashEntry(hashTable, key);
    return (found->next) ? found->next->value : found->next;
}



unsigned int stdHashFunction(struct HashTable *hash, char *key) {
    unsigned int i = 0;
    unsigned int sum = 0;
    while(key[i] != '\0') {
        sum += key[i++];
    };
    return sum % hash->size;
}


char **hashTableKeys(struct HashTable *hash) {
    unsigned int i = 0, j = 0;
    char **keys;
    struct HashEntry *entry;
    /* we will iterate two times over the entire hash 
       once for evaluating the number of keys
       once for reading the actual keys */
    for(i = 0; i < hash->size; i++) {
        entry = hash->hash[i];
        while((entry = entry->next)) j++;
    };

    keys = (char **)malloc(sizeof(char *) * (j + 1));
    
    for(i = j = 0; i < hash->size; i++)  {
        entry = hash->hash[i];
        while((entry = entry->next)) keys[j++] = entry->key;
    };
        
    keys[j] = 0;
    return keys;
}

