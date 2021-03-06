/*
 * (C) 2010 Michael J. Beer
 * All rights reserved.
 *
 * Redistribution  and use in source and binary forms, with or with‐
 * out modification, are permitted provided that the following  con‐
 * ditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above  copy‐
 * right  notice,  this  list  of  conditions and the following dis‐
 * claimer in the documentation and/or other materials provided with
 * the distribution.
 *
 * 3.  Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote  products  derived
 * from this software without specific prior written permission.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBU‐
 * TORS "AS IS" AND ANY EXPRESS OR  IMPLIED  WARRANTIES,  INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT
 * SHALL  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DI‐
 * RECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS IN‐
 * TERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY  THEORY  OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  NEGLI‐
 * GENCE  OR  OTHERWISE)  ARISING  IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct HashEntry *getHashEntry(struct HashTable *hash, char *key) {
    unsigned int i = hash->hashFunc(hash, key);
    struct HashEntry *found = hash->hash[i];
    while (found->next) {
        if (strcmp(found->next->key, key) == 0) {
            return found;
        };
        found = found->next;
    };
    return found;
}

struct HashTable *hashTableCreate(
    unsigned int n, unsigned int (*hashFunc)(struct HashTable *hash, char *)) {
    int i;
    struct HashTable *hash =
        (struct HashTable *)malloc(sizeof(struct HashTable));
    hash->hashFunc = (hashFunc) ? hashFunc : stdHashFunction;
    hash->size = n;
    hash->hash = malloc(sizeof(struct HashEntry *) * hash->size);
    for (i = 0; i < hash->size; i++) {
        hash->hash[i] = (struct HashEntry *)malloc(sizeof(struct HashEntry));
        hash->hash[i]->key = 0;
        hash->hash[i]->next = NULL;
    };
    return hash;
}

void hashTableDispose(struct HashTable *hash) {
    int i = 0;
    for (i = 0; i < hash->size; i++) {
        struct HashEntry *entry = hash->hash[i];
        struct HashEntry *next;
        while ((entry)) {
            next = entry->next;
#ifdef HASH_TABLE_SAFE_KEY
            if (entry->key) free(entry->key);
#endif
            free(entry);
            entry = next;
        };
        free(entry);
    };
    free(hash->hash);
    free(hash);
}

void *hashTableSet(struct HashTable *hashTable, char *key, void *val) {
    struct HashEntry *found = getHashEntry(hashTable, key);
    void *old = NULL;
    if (!found->next) {
        found->next = malloc(sizeof(struct HashEntry));
        found->next->next = NULL;
#ifdef HASH_TABLE_SAFE_KEY
        found->next->key = malloc(sizeof(char) * (1 + strlen(key)));
        strcpy(found->next->key, key);
#else
        found->next->key = key;
#endif
    } else {
        old = found->next->value;
    }
    found->next->value = val;
    return old; /* found->next->value; */
}

void *hashTableDelete(struct HashTable *hashTable, char *key) {
    struct HashEntry *found = getHashEntry(hashTable, key);
    struct HashEntry *next;
    void *value;
    if (!found->next) return found->next;
    next = found->next->next;
    value = found->next->value;
#ifdef HASH_TABLE_SAFE_KEY
    free(found->next->key);
#endif
    free(found->next);
    found->next = next;
    return value;
}

void *hashTableGet(struct HashTable *hashTable, char *key) {
    struct HashEntry *found = getHashEntry(hashTable, key);
    return (found->next) ? found->next->value : found->next;
}

unsigned int stdHashFunction(struct HashTable *hash, char *key) {
    unsigned int i = 0;
    unsigned int sum = 0;
    while (key[i] != '\0') {
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
    for (i = 0; i < hash->size; i++) {
        entry = hash->hash[i];
        while ((entry = entry->next)) j++;
    };

    keys = (char **)malloc(sizeof(char *) * (j + 1));

    for (i = j = 0; i < hash->size; i++) {
        entry = hash->hash[i];
        while ((entry = entry->next)) keys[j++] = entry->key;
    };

    keys[j] = 0;
    return keys;
}
